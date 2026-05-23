#!/usr/bin/env bash
# Run all four MST experiments and write tidy CSVs into results/.
#
#   E1  algorithm comparison at fixed sizes
#   E2  strong scaling at n=100000
#   E3  size scaling (Borůvka-OMP at 1 and 10 threads)
#   E4  Borůvka rounds vs log2(n)
#
# Graphs are cached under /tmp/exp_graphs/ so reruns skip generation.
# Density: p = c * ln(n) / n with c=4, which sits above the connectivity threshold.

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

GEN="$ROOT/build/tools/gen_graph"
BENCH="$ROOT/build/tools/bench"
[ -x "$GEN" ]   || { echo "missing $GEN"; exit 1; }
[ -x "$BENCH" ] || { echo "missing $BENCH"; exit 1; }

GRAPH_DIR=/tmp/exp_graphs
OUT_DIR="$ROOT/results"
mkdir -p "$GRAPH_DIR" "$OUT_DIR"

SEEDS=(42 43 44 45 46)
C=4

p_for() {
    awk -v n=$1 -v c=$C 'BEGIN{printf "%.10f", c*log(n)/n}'
}

gen_if_missing() {
    local n=$1 seed=$2
    local graph="$GRAPH_DIR/g_${n}_s${seed}.txt"
    if [ ! -f "$graph" ]; then
        local p=$(p_for $n)
        printf "  gen n=%-7d seed=%d p=%s ...\n" "$n" "$seed" "$p" >&2
        "$GEN" "$n" "$p" "$seed" > "$graph"
    fi
    echo "$graph"
}

bench_row() {
    "$BENCH" "$1" "$2" < "$3"
}

HEADER="seed,algo,threads,n,m,mst_edges,rounds,total_weight,wall_ms"

echo "=== Generating graphs ===" >&2
for n in 100 1000 10000 100000 1000000; do
    for s in "${SEEDS[@]}"; do
        gen_if_missing "$n" "$s" > /dev/null
    done
done

echo "=== E1: algorithm comparison ===" >&2
echo "$HEADER" > "$OUT_DIR/e1_algo_comparison.csv"
for n in 1000 10000 100000 1000000; do
    for s in "${SEEDS[@]}"; do
        graph=$(gen_if_missing "$n" "$s")
        for algo in kruskal prim boruvka_seq boruvka_omp; do
            echo "$s,$(bench_row "$algo" 1 "$graph")" >> "$OUT_DIR/e1_algo_comparison.csv"
        done
    done
done

echo "=== E2: strong scaling at n=100000 ===" >&2
echo "$HEADER" > "$OUT_DIR/e2_strong_scaling.csv"
for t in 1 2 4 6 8 10; do
    for s in "${SEEDS[@]}"; do
        graph=$(gen_if_missing 100000 "$s")
        echo "$s,$(bench_row boruvka_omp "$t" "$graph")" >> "$OUT_DIR/e2_strong_scaling.csv"
    done
done

echo "=== E3: size scaling (Borůvka-OMP 1 vs 10 threads) ===" >&2
echo "$HEADER" > "$OUT_DIR/e3_size_scaling.csv"
for n in 1000 10000 100000 1000000; do
    for t in 1 10; do
        for s in "${SEEDS[@]}"; do
            graph=$(gen_if_missing "$n" "$s")
            echo "$s,$(bench_row boruvka_omp "$t" "$graph")" >> "$OUT_DIR/e3_size_scaling.csv"
        done
    done
done

echo "=== E4: Borůvka rounds vs log2(n) ===" >&2
echo "$HEADER" > "$OUT_DIR/e4_rounds.csv"
for n in 100 1000 10000 100000 1000000; do
    for s in "${SEEDS[@]}"; do
        graph=$(gen_if_missing "$n" "$s")
        echo "$s,$(bench_row boruvka_seq 1 "$graph")" >> "$OUT_DIR/e4_rounds.csv"
    done
done

echo "=== Done; CSVs in $OUT_DIR/ ===" >&2
