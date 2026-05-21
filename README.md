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
tools/         gen_graph, gen_grid, bench, dump_mst
scripts/       python visualisation + bash/powershell bench sweep
```

## Algorithms

| Variant       | Header                       | Notes                              |
|---------------|------------------------------|------------------------------------|
| Kruskal       | `mst/kruskal.hpp`            | edge sort + locked Union-Find      |
| Prim          | `mst/prim.hpp`               | binary-heap `std::priority_queue`  |
| Borůvka (seq) | `mst/boruvka_seq.hpp`        | classical, deterministic tie-break |
| Borůvka (OMP) | `mst/boruvka_omp.hpp`        | OpenMP min-edge reduction          |

## Tools

All four binaries speak the same plain-text edge-list format on stdin:
line 1 is `n m`, followed by `m` lines of `u v w`.

`gen_graph <n> <p> [seed]` — Erdős–Rényi `G(n, p)` generator, weights `U(0,1)`.

```
./build/tools/gen_graph 1000 0.01 42 > graph.txt
```

`gen_grid <rows> <cols> [seed]` — 2D 4-neighbour grid graph, weights `U(0,1)`.
Useful for visualisation since vertex `i` has a natural position `(i % cols, i / cols)`.

```
./build/tools/gen_grid 16 16 7 > grid.txt
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

End-to-end visualisation example:

```
./build/tools/gen_grid 16 16 7 | ./build/tools/dump_mst boruvka_omp 4 \
    | python3 scripts/visualize_mst.py --layout grid --grid-cols 16 --out mst.png
```

Sweep helper (bash on macOS/Linux/WSL, PowerShell on Windows):

```
scripts/run_bench_sweep.sh  graph.txt 1 2 4 8 > sweep.csv
scripts/run_bench_sweep.ps1 -Graph graph.txt -Threads 1,2,4,8 > sweep.csv
python3 scripts/plot_bench.py --in sweep.csv --out sweep.png
```

## Notes

- On macOS, OpenMP requires `libomp` from Homebrew (Apple Clang does not ship it).
  The CMake build looks under both `/opt/homebrew/opt/libomp` (Apple Silicon) and
  `/usr/local/opt/libomp` (Intel).
- All four implementations are cross-validated against
  `boost::kruskal_minimum_spanning_tree` on random graphs in `test_property_vs_boost`.

## License

MIT.
