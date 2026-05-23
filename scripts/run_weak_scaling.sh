#!/usr/bin/env bash
# Weak scaling: keep the work-per-thread roughly constant by growing n with
# the thread count, so each thread sees the same edge volume. Ideal scaling
# would mean constant wall time. Density: p = c * ln(n) / n with c=4.
#
# Output: results/e6_weak_scaling.csv

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

GEN="$ROOT/build/tools/gen_graph"
BENCH="$ROOT/build/tools/bench"
[ -x "$GEN" ] && [ -x "$BENCH" ] || { echo "build first"; exit 1; }

GRAPH_DIR=/tmp/exp_graphs
OUT="$ROOT/results/e6_weak_scaling.csv"
mkdir -p "$GRAPH_DIR" "$ROOT/results"

BASE_N=10000
SEEDS=(42 43 44 45 46)
THREADS=(1 2 4 6 8 10)
C=4

p_for() { awk -v n=$1 -v c=$C 'BEGIN{printf "%.10f", c*log(n)/n}'; }

echo "threads,n,seed,m,wall_ms" > "$OUT"

for t in "${THREADS[@]}"; do
    n=$(( BASE_N * t ))
    p=$(p_for "$n")
    for s in "${SEEDS[@]}"; do
        g="$GRAPH_DIR/g_ws_${n}_s${s}.txt"
        if [ ! -f "$g" ]; then
            "$GEN" "$n" "$p" "$s" > "$g"
        fi
        row=$("$BENCH" boruvka_omp "$t" < "$g")
        m=$(echo "$row" | cut -d, -f4)
        ms=$(echo "$row" | cut -d, -f8)
        echo "$t,$n,$s,$m,$ms" >> "$OUT"
    done
done

echo "wrote $OUT" >&2
