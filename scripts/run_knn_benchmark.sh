#!/usr/bin/env bash
# Geometric k-NN graph benchmark: random 2D points, k=10 Euclidean nearest
# neighbours, symmetrised. Sizes n ∈ {1000, 5000, 10000}. All four algorithms
# at single thread, 5 seeds.
#
# Output: results/e10_knn_benchmark.csv

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

GEN="$ROOT/build/tools/gen_knn"
BENCH="$ROOT/build/tools/bench"
[ -x "$GEN" ] && [ -x "$BENCH" ] || { echo "build first"; exit 1; }

GRAPH_DIR=/tmp/exp_graphs_knn
OUT="$ROOT/results/e10_knn_benchmark.csv"
mkdir -p "$GRAPH_DIR" "$ROOT/results"

K=10
SIZES=(1000 5000 10000)
SEEDS=(42 43 44 45 46)

echo "seed,algo,n,k,m,wall_ms,total_weight" > "$OUT"

for n in "${SIZES[@]}"; do
    for s in "${SEEDS[@]}"; do
        g="$GRAPH_DIR/knn_n${n}_k${K}_s${s}.txt"
        if [ ! -f "$g" ]; then
            "$GEN" "$n" "$K" "$s" > "$g"
        fi
        for algo in kruskal prim boruvka_seq boruvka_omp; do
            row=$("$BENCH" "$algo" 1 < "$g")
            m=$(echo "$row" | cut -d, -f4)
            wt=$(echo "$row" | cut -d, -f7)
            ms=$(echo "$row" | cut -d, -f8)
            echo "$s,$algo,$n,$K,$m,$ms,$wt" >> "$OUT"
        done
    done
done

echo "wrote $OUT" >&2
