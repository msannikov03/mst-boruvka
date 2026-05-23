#!/usr/bin/env bash
# 2D grid graph benchmark at the proposal's promised sizes: 32^2, 128^2, 512^2.
# All four algorithms at single thread, 5 seeds.
#
# Output: results/e9_grid_benchmark.csv

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

GEN="$ROOT/build/tools/gen_grid"
BENCH="$ROOT/build/tools/bench"
[ -x "$GEN" ] && [ -x "$BENCH" ] || { echo "build first"; exit 1; }

GRAPH_DIR=/tmp/exp_graphs_grid
OUT="$ROOT/results/e9_grid_benchmark.csv"
mkdir -p "$GRAPH_DIR" "$ROOT/results"

# (rows, cols)
SIZES=("32 32" "128 128" "512 512")
SEEDS=(42 43 44 45 46)

echo "seed,algo,rows,cols,n,m,wall_ms,total_weight" > "$OUT"

for sz in "${SIZES[@]}"; do
    r=${sz%% *}
    c=${sz##* }
    for s in "${SEEDS[@]}"; do
        g="$GRAPH_DIR/grid_${r}x${c}_s${s}.txt"
        if [ ! -f "$g" ]; then
            "$GEN" "$r" "$c" "$s" > "$g"
        fi
        for algo in kruskal prim boruvka_seq boruvka_omp; do
            row=$("$BENCH" "$algo" 1 < "$g")
            n=$(echo "$row" | cut -d, -f3)
            m=$(echo "$row" | cut -d, -f4)
            wt=$(echo "$row" | cut -d, -f7)
            ms=$(echo "$row" | cut -d, -f8)
            echo "$s,$algo,$r,$c,$n,$m,$ms,$wt" >> "$OUT"
        done
    done
done

echo "wrote $OUT" >&2
