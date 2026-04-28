# DESIGN.md — Top-K k-NN on Cerebras WSE-2

**Start time:** 2026-04-26

## Problem
Given query vector `q` (d dims, f32) and database `D` (N×d, f32),
return the K nearest rows by squared Euclidean distance, sorted
ascending, tie-broken by original row index (lower index wins).


## What runs where
**On-wafer (CSL):** Each PE is a lightweight processor with ~48KB
SRAM. The database D is sharded row-wise across P×P PEs. PE number
`PE_ID = row*P + col` owns rows `[PE_ID * ceil(N/P²), min(..., N))`.
Each PE receives the query q and its shard via host memcpy, computes
squared L2 distances to every row in its shard, and maintains a
sorted local top-K list of (distance, original_index) pairs.

**Off-wafer (Python):** After all PEs finish, the host reads the
top-K list from every PE and merges them using
`np.lexsort((indices, distances))` to produce the global top-K.


## Fixed array sizes
CSL requires array sizes to be compile-time constants:
- `MAX_D = 32` — no test case exceeds d=32
- `MAX_K = 256` — no test case exceeds K=256
- `MAX_ROWS = 256` — worst case is N=1024, P=2 → ceil(1024/4) = 256

We initially used MAX_ROWS=128 which crashed the k=1 case (N=1024,
P=2 gives 256 rows/PE). Per-PE SRAM usage: 256×32×4 + 3×256×4
≈ 35KB, within the 48KB limit.


## Tie-breaking
Equal distance → smaller original index wins. Enforced in two places:

1. `insert()` in pe_kernel.csl: only inserts candidate if
   `dist < existing` or (`dist == existing` and `idx < existing_idx`)
2. `merge_topk()` in run.py: `np.lexsort((flat_idx, flat_dist))`
   sorts by distance as primary key, index as tiebreaker.

The `all-equal` case (every row identical → answer must be [0,1,...,K-1])
and `duplicates` case (rows 100/500 and 250/800 are exact copies)
both pass, confirming tie-breaking is correct end-to-end.


## Hardest bug
The Dockerfile contained a stub `sdk_debug_shell` that printed one
line and exited 0 without invoking the real compiler. `cslc` reported
"Compilation successful" every time but produced 0 PE programs.
Diagnosed via `--verbose` flag which revealed "Produced 0 distinct
PE programs." Fixed by copying the real `sdk_debug_shell` from
`/workspace` into `/opt/sdk/`.


## What I'd improve with more time

1. **On-wafer reduce:** currently P²×K values travel back to the
   host. A fabric reduce (south→north per column, then east→west
   along row 0 using color wavelets) would return only K values.
2. **SIMD distance:** unroll the d-loop using `@fmacs` for 4
   multiply-accumulates per cycle (~4× compute speedup).
3. **DMA pipelining:** process row r while prefetching row r+1
   into a shadow buffer to hide memory latency.