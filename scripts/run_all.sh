#!/usr/bin/env bash
# Single-command reproducibility: regenerate every experiment CSV and figure
# under results/. Builds the project if needed, runs the bench sweeps, then
# the profile sweep, then renders all plots.
#
# usage: scripts/run_all.sh
#
# Takes ~10 minutes on a multi-core x86_64 or Apple Silicon machine.

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

if [ ! -x "$ROOT/build/tools/bench" ]; then
    echo "==> building project" >&2
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release >/dev/null
    cmake --build build -j >/dev/null
fi

echo "==> E1-E4: core experiments" >&2
scripts/run_experiments.sh

echo "==> E5: per-phase profile breakdown" >&2
scripts/run_profile.sh

echo "==> E6: weak scaling" >&2
scripts/run_weak_scaling.sh

echo "==> E7: density-by-threads speedup heatmap" >&2
scripts/run_density_heatmap.sh

echo "==> E8: multi-density Erdős-Rényi sweep" >&2
scripts/run_multi_density.sh

echo "==> E9: 2D grid benchmark" >&2
scripts/run_grid_benchmark.sh

echo "==> E10: geometric k-NN benchmark" >&2
scripts/run_knn_benchmark.sh

echo "==> rendering plots" >&2
python3 scripts/plot_e1_algo_comparison.py
python3 scripts/plot_e2_strong_scaling.py
python3 scripts/plot_e3_size_scaling.py
python3 scripts/plot_e4_rounds.py
python3 scripts/plot_e5_profile.py
python3 scripts/plot_e6_weak_scaling.py
python3 scripts/plot_e7_density_heatmap.py
python3 scripts/plot_e8_multi_density.py
python3 scripts/plot_e9_grid_benchmark.py
python3 scripts/plot_e10_knn_benchmark.py

echo "==> E11: per-round MST animation on a 10x10 grid" >&2
"$ROOT/build/tools/gen_grid" 10 10 7 \
    | python3 scripts/animate_mst.py --layout grid --grid-cols 10 \
                                     --out "$ROOT/results/e11_mst_animation.gif" \
                                     --fps 1.5

echo "==> done; all CSVs and figures in $ROOT/results/" >&2
ls "$ROOT/results"
