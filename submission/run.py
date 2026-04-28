#!/usr/bin/env python3
from __future__ import annotations
import argparse, math, os, sys
import numpy as np
from cerebras.sdk.runtime.sdkruntimepybind import SdkRuntime, MemcpyDataType, MemcpyOrder

sys.path.insert(0, os.path.dirname(__file__))
from reference import topk_reference, ALL_CASES

COMPILE_DIR = os.environ.get("COMPILE_DIR", os.path.join(os.path.dirname(__file__), "out"))
ATOL   = 1e-3
MAX_D  = 32
MAX_K  = 256
MAX_ROWS = 256

def case_compile_dir(name):
    return os.path.join(COMPILE_DIR, name.replace("=","_").replace("-","_"))

def merge_topk(all_dist, all_idx, K):
    flat_dist = all_dist.flatten()
    flat_idx  = all_idx.flatten().astype(np.int32)
    order = np.lexsort((flat_idx, flat_dist))
    top = order[:K]
    return flat_idx[top].astype(np.int32), flat_dist[top].astype(np.float32)

def h2d(runner, sym, data_u32, col, row, length):
    runner.memcpy_h2d(sym, data_u32, col, row, 1, 1, length,
        streaming=False, order=MemcpyOrder.ROW_MAJOR,
        data_type=MemcpyDataType.MEMCPY_32BIT, nonblock=False)

def d2h(runner, buf_u32, sym, col, row, length):
    runner.memcpy_d2h(buf_u32, sym, col, row, 1, 1, length,
        streaming=False, order=MemcpyOrder.ROW_MAJOR,
        data_type=MemcpyDataType.MEMCPY_32BIT, nonblock=False)

def run_case(case: dict) -> bool:
    name = case["name"]
    D    = case["D"].astype(np.float32)
    q    = case["q"].astype(np.float32)
    K    = case["K"]
    P    = case["P"]
    N, d = D.shape

    ref_idx, ref_dist = topk_reference(D, q, K)

    cdir = case_compile_dir(name)
    if not os.path.isdir(cdir):
        print(f"ERROR: {name}: compile dir not found: {cdir}"); return False

    runner = SdkRuntime(cdir, simulator=True)
    runner.load()
    runner.run()

    total_pes   = P * P
    rows_per_pe = math.ceil(N / total_pes)

    q_sym     = runner.get_id("q_buf")
    shard_sym = runner.get_id("shard")
    dist_sym  = runner.get_id("out_dist")
    idx_sym   = runner.get_id("out_idx")

    # Write query to every PE
    q_padded = np.zeros(MAX_D, dtype=np.float32)
    q_padded[:d] = q
    q_u32 = q_padded.view(np.uint32)
    for pe_id in range(total_pes):
        h2d(runner, q_sym, q_u32, pe_id%P, pe_id//P, MAX_D)

    # Write shard to every PE
    INF = np.finfo(np.float32).max
    for pe_id in range(total_pes):
        first = pe_id * rows_per_pe
        last  = min(first + rows_per_pe, N)
        shard = D[first:last]
        buf = np.full((MAX_ROWS, MAX_D), INF, dtype=np.float32)
        buf[:shard.shape[0], :d] = shard
        h2d(runner, shard_sym, buf.flatten().view(np.uint32), pe_id%P, pe_id//P, MAX_ROWS*MAX_D)

    # Launch
    runner.launch("compute", nonblock=False)

    # Read back from every PE
    all_dist = np.zeros((total_pes, MAX_K), dtype=np.uint32)
    all_idx  = np.zeros((total_pes, MAX_K), dtype=np.uint32)
    for pe_id in range(total_pes):
        d2h(runner, all_dist[pe_id], dist_sym, pe_id%P, pe_id//P, MAX_K)
        d2h(runner, all_idx[pe_id],  idx_sym,  pe_id%P, pe_id//P, MAX_K)

    runner.stop()

    all_dist_f = all_dist.view(np.float32)
    out_idx, out_dist = merge_topk(all_dist_f[:, :K], all_idx[:, :K], K)

    idx_ok  = np.array_equal(out_idx,  ref_idx)
    dist_ok = np.allclose(out_dist, ref_dist, atol=ATOL)

    if idx_ok and dist_ok:
        print(f"PASS: {name}"); return True

    print(f"FAIL: {name}")
    if not idx_ok:
        print(f"  idx got: {out_idx[:min(5,K)].tolist()}")
        print(f"  idx exp: {ref_idx[:min(5,K)].tolist()}")
    if not dist_ok:
        print(f"  dist got: {out_dist[:min(5,K)].tolist()}")
        print(f"  dist exp: {ref_dist[:min(5,K)].tolist()}")
    return False

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--case")
    args = parser.parse_args()
    all_makers = {m()["name"]: m for m in ALL_CASES}
    cases = [all_makers[args.case]()] if args.case else [m() for m in ALL_CASES]
    sys.exit(0 if all(run_case(c) for c in cases) else 1)

if __name__ == "__main__":
    main()
