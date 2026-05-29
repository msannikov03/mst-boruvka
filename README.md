# mst-boruvka

Parallel Minimum Spanning Tree benchmark suite around Borůvka's algorithm.
Sequential Kruskal / Prim / Borůvka baselines plus an OpenMP parallel Borůvka,
all cross-validated against `boost::kruskal_minimum_spanning_tree`.

## Build

Requires CMake 3.20+, a C++17 compiler, OpenMP, Boost.Graph.

macOS (Homebrew):

```
brew install libomp boost
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Ubuntu:

```
sudo apt-get install -y cmake g++ libboost-graph-dev libomp-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Windows (MSVC + vcpkg):

```
vcpkg install boost-graph:x64-windows-static-md
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake ^
    -DVCPKG_TARGET_TRIPLET=x64-windows-static-md
cmake --build build --config Release -j
ctest --test-dir build -C Release --output-on-failure
```

Prerequisites on Windows: Visual Studio 2022 with the "Desktop development with C++"
workload (provides MSVC + CMake + Ninja), and [vcpkg](https://vcpkg.io/) for Boost.
OpenMP ships with MSVC; no separate install needed.

## Layout

```
include/mst/   public headers
src/           library implementations
tests/         GoogleTest suites
tools/         gen_graph, gen_grid, gen_knn, bench, dump_mst, profile_omp
scripts/       bench-sweep orchestrators, plot scripts, MST visualisation
results/       experiment CSVs and figures (committed; regenerable)
```

## Algorithms

| Variant       | Header                       | Notes                              |
|---------------|------------------------------|------------------------------------|
| Kruskal       | `mst/kruskal.hpp`            | edge sort + Union-Find             |
| Prim          | `mst/prim.hpp`               | binary-heap `std::priority_queue`  |
| Borůvka (seq) | `mst/boruvka_seq.hpp`        | classical, deterministic tie-break |
| Borůvka (OMP) | `mst/boruvka_omp.hpp`        | OpenMP min-edge reduction          |

## Tools

All tools share the same plain-text edge-list format: line 1 is `n m`, followed by `m`
lines of `u v w`. The three generators write it to stdout; `bench`, `dump_mst`, and
`profile_omp` read it from stdin.

`gen_graph <n> <p> [seed]` — Erdős–Rényi `G(n, p)` generator, weights `U(0,1)`.

```
./build/tools/gen_graph 1000 0.01 42 > graph.txt
```

`gen_grid <rows> <cols> [seed]` — 2D 4-neighbour grid graph, weights `U(0,1)`.
Useful for visualisation since vertex `i` has a natural position `(i % cols, i / cols)`.

```
./build/tools/gen_grid 16 16 7 > grid.txt
```

`gen_knn <n> <k> [seed]` — Euclidean k-nearest-neighbour graph on `n` random
points uniformly sampled from `[0,1]²`; edges are symmetrised (i↔j if either
sees the other in their k-NN list); weight = Euclidean distance. Generation
is brute-force O(n²); fine up to n≈10⁴.

```
./build/tools/gen_knn 1000 10 42 > knn.txt
```

`bench <algo> [threads]` — runs one of `kruskal | prim | boruvka_seq | boruvka_omp`,
prints one CSV row:

```
algo,threads,n,m,mst_edges,rounds,total_weight,wall_ms
```

`threads` is the requested thread count (0 = OMP default; only meaningful for `boruvka_omp`).
`rounds` is the Borůvka iteration count (0 for Kruskal/Prim).

`dump_mst <algo> [threads]` — same input, but instead of one CSV row prints
`n m total_weight` then `u v w in_mst` per edge (in_mst is 0/1). Consumed by
`scripts/visualize_mst.py` to render a static MST overlay.

`profile_omp <threads> [seed_tag]` — runs `boruvka_omp` with per-round per-phase
timing instrumentation; prints CSV with columns
`threads,seed,round,setup_ms,scan_ms,merge_ms,contract_ms,edges_added,components_in,components_out`.
Consumed by `scripts/plot_e5_profile.py`.

End-to-end visualisation example:

```
./build/tools/gen_grid 16 16 7 | ./build/tools/dump_mst boruvka_omp 4 \
    | python3 scripts/visualize_mst.py --layout grid --grid-cols 16 --out mst.png
```

Single-graph sweep helper (bash on macOS/Linux/WSL, PowerShell on Windows):

```
scripts/run_bench_sweep.sh  graph.txt 1 2 4 8 > sweep.csv
scripts/run_bench_sweep.ps1 -Graph graph.txt -Threads 1,2,4,8 > sweep.csv
python3 scripts/plot_bench.py --in sweep.csv --out sweep.png
```

## Experiments

The CSVs and figures under `results/` are produced by a fixed-seed sweep. The
plot and animation scripts need `matplotlib`, `networkx`, and `numpy`:

```
pip install -r requirements.txt
```

Reproduce everything with one command (≈10 min on a multi-core machine):

```
scripts/run_all.sh
```

Or run a single experiment + plot:

| Experiment | Runner | Plot | Output |
|---|---|---|---|
| **E1** algorithm comparison (4 algos × n) | `run_experiments.sh` | `plot_e1_algo_comparison.py` | `results/e1_algo_comparison.{csv,png}` |
| **E2** strong scaling (n=10⁵, threads 1–10) | `run_experiments.sh` | `plot_e2_strong_scaling.py`     | `results/e2_strong_scaling.{csv,png}` |
| **E3** size scaling (n=10³–10⁶, 1 vs 10 thr) | `run_experiments.sh` | `plot_e3_size_scaling.py`     | `results/e3_size_scaling.{csv,png}` |
| **E4** Borůvka rounds vs log₂(n)             | `run_experiments.sh` | `plot_e4_rounds.py`           | `results/e4_rounds.{csv,png}` |
| **E5** per-round phase breakdown             | `run_profile.sh`     | `plot_e5_profile.py`          | `results/e5_profile.{csv,png}` |
| **E6** weak scaling                          | `run_weak_scaling.sh` | `plot_e6_weak_scaling.py`    | `results/e6_weak_scaling.{csv,png}` |
| **E7** density × threads speedup heatmap     | `run_density_heatmap.sh` | `plot_e7_density_heatmap.py` | `results/e7_density_heatmap.{csv,png}` |
| **E8** multi-density Erdős–Rényi (3 densities) | `run_multi_density.sh` | `plot_e8_multi_density.py` | `results/e8_multi_density.{csv,png}` |
| **E9** 2D grid benchmark (3 sizes)           | `run_grid_benchmark.sh` | `plot_e9_grid_benchmark.py` | `results/e9_grid_benchmark.{csv,png}` |
| **E10** geometric k-NN benchmark (3 sizes)   | `run_knn_benchmark.sh`  | `plot_e10_knn_benchmark.py` | `results/e10_knn_benchmark.{csv,png}` |
| **E11** per-round MST animation (10×10 grid) | `gen_grid \| animate_mst.py` (Python replay of Borůvka) | — | `results/e11_mst_animation.gif` |

All bench sweeps use 5 seeds (42–46); plots show medians.

## Notes

- On macOS, OpenMP requires `libomp` from Homebrew (Apple Clang does not ship it).
  The CMake build looks under both `/opt/homebrew/opt/libomp` (Apple Silicon) and
  `/usr/local/opt/libomp` (Intel).
- All four implementations are cross-validated against
  `boost::kruskal_minimum_spanning_tree` on 90 random graphs (60 generic, 30 with
  duplicate weights) in `test_property_vs_boost`.

## License

MIT.
