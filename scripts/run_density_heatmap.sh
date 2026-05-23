#!/usr/bin/env bash
# Density × thread-count sweep for boruvka_omp at fixed n=10^5.
# Output: results/e7_density_heatmap.csv

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

GEN="$ROOT/build/tools/gen_graph"
BENCH="$ROOT/build/tools/bench"
[ -x "$GEN" ] && [ -x "$BENCH" ] || { echo "build first"; exit 1; }

GRAPH_DIR=/tmp/exp_graphs_dense
OUT="$ROOT/results/e7_density_heatmap.csv"
mkdir -p "$GRAPH_DIR" "$ROOT/results"

N=100000
SEEDS=(42 43 44 45 46)
THREADS=(1 2 4 6 8 10)

# (label, p)
DENSITIES=(
    "sparse:0.00004"     # ~c/n with c=4 at n=10^5
    "log:0.00046"        # ~4 ln(n)/n at n=10^5
    "medium:0.001"       # 2.2x denser than log threshold
)

echo "density,p,threads,n,seed,m,wall_ms" > "$OUT"

for d in "${DENSITIES[@]}"; do
    label=${d%%:*}
    p=${d##*:}
    for s in "${SEEDS[@]}"; do
        g="$GRAPH_DIR/g_${N}_${label}_s${s}.txt"
        if [ ! -f "$g" ]; then
            "$GEN" "$N" "$p" "$s" > "$g"
        fi
        for t in "${THREADS[@]}"; do
            row=$("$BENCH" boruvka_omp "$t" < "$g")
            m=$(echo "$row" | cut -d, -f4)
            ms=$(echo "$row" | cut -d, -f8)
            echo "$label,$p,$t,$N,$s,$m,$ms" >> "$OUT"
        done
    done
done

echo "wrote $OUT" >&2
