#!/usr/bin/env bash
# Sweep MST algorithms on one graph; for boruvka_omp also sweep thread counts.
# Output: bench CSV rows on stdout.
#
# usage: scripts/run_bench_sweep.sh <graph.txt> [thread_count ...]
# default thread counts: 1 2 4 8

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "usage: $0 <graph.txt> [thread_count ...]" >&2
    exit 1
fi

GRAPH="$1"
shift
if [ $# -gt 0 ]; then
    THREADS=("$@")
else
    THREADS=(1 2 4 8)
fi

BENCH="./build/tools/bench"
if [ ! -x "$BENCH" ]; then
    echo "missing $BENCH; run: cmake -S . -B build && cmake --build build -j" >&2
    exit 1
fi

"$BENCH" kruskal     1 < "$GRAPH"
"$BENCH" prim        1 < "$GRAPH"
"$BENCH" boruvka_seq 1 < "$GRAPH"
for t in "${THREADS[@]}"; do
    "$BENCH" boruvka_omp "$t" < "$GRAPH"
done
