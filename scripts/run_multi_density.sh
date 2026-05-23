#!/usr/bin/env bash
# Multi-density Erdős-Rényi sweep at fixed n=10^4: matches the proposal §4
# promise of three densities {c/n, c·ln(n)/n, 0.1}. All four algorithms at
# single thread, 5 seeds.
#
# Output: results/e8_multi_density.csv

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

GEN="$ROOT/build/tools/gen_graph"
BENCH="$ROOT/build/tools/bench"
[ -x "$GEN" ] && [ -x "$BENCH" ] || { echo "build first"; exit 1; }

GRAPH_DIR=/tmp/exp_graphs_md
OUT="$ROOT/results/e8_multi_density.csv"
mkdir -p "$GRAPH_DIR" "$ROOT/results"

N=10000
SEEDS=(42 43 44 45 46)

# label : p
DENSITIES=(
    "sparse:0.0004"     # c/n with c=4
    "log:0.003684"      # 4*ln(n)/n at n=10^4
    "dense:0.1"         # proposal's dense regime
)

echo "seed,algo,density,p,n,m,wall_ms,total_weight" > "$OUT"

for d in "${DENSITIES[@]}"; do
    label=${d%%:*}
    p=${d##*:}
    for s in "${SEEDS[@]}"; do
        g="$GRAPH_DIR/g_${N}_${label}_s${s}.txt"
        if [ ! -f "$g" ]; then
            "$GEN" "$N" "$p" "$s" > "$g"
        fi
        for algo in kruskal prim boruvka_seq boruvka_omp; do
            row=$("$BENCH" "$algo" 1 < "$g")
            n=$(echo "$row" | cut -d, -f3)
            m=$(echo "$row" | cut -d, -f4)
            wt=$(echo "$row" | cut -d, -f7)
            ms=$(echo "$row" | cut -d, -f8)
            echo "$s,$algo,$label,$p,$n,$m,$ms,$wt" >> "$OUT"
        done
    done
done

echo "wrote $OUT" >&2
