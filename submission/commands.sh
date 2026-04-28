#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$SCRIPT_DIR/src"
OUT_DIR="$SCRIPT_DIR/out"
export COMPILE_DIR="$OUT_DIR"

CASES=(
  "baseline   2048  32   16   4"
  "k_1        1024  32    1   2"
  "k_256      1024  16  256   2"
  "uneven     1009  32   16   4"
  "all_equal  1024  16   16   2"
  "duplicates 1024  16    8   2"
)

declare -A CASE_DISPLAY=(
  [baseline]="baseline"
  [k_1]="k=1"
  [k_256]="k=256"
  [uneven]="uneven"
  [all_equal]="all-equal"
  [duplicates]="duplicates"
)

TARGET="${1:-}"
mkdir -p "$OUT_DIR"

echo "=== Compiling ==="
for ENTRY in "${CASES[@]}"; do
  read -r KEY N D K P <<< "$ENTRY"
  if [[ -n "$TARGET" && "$KEY" != "$TARGET" ]]; then continue; fi
  OUTDIR="$OUT_DIR/$KEY"
  FABRIC_W=$((P + 7))
  FABRIC_H=$((P + 2))
  echo "  [${CASE_DISPLAY[$KEY]}]  N=$N d=$D K=$K P=$P  -> $OUTDIR"
  cslc --arch=wse2 "$SRC_DIR/layout.csl" \
    --fabric-dims="${FABRIC_W},${FABRIC_H}" \
    --fabric-offsets=4,1 \
    --params="P:${P},N:${N},d:${D},K:${K}" \
    --memcpy --channels=1 \
    -o "$OUTDIR"
done

echo ""
echo "=== Running ==="
for ENTRY in "${CASES[@]}"; do
  read -r KEY N D K P <<< "$ENTRY"
  if [[ -n "$TARGET" && "$KEY" != "$TARGET" ]]; then continue; fi
  cs_python "$SCRIPT_DIR/run.py" --case "${CASE_DISPLAY[$KEY]}"
done

echo "Done."
