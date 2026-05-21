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

## Layout

```
include/mst/   public headers
src/           library implementations
tests/         GoogleTest suites
tools/         gen_graph, bench
```

## Algorithms

| Variant       | Header                       | Notes                              |
|---------------|------------------------------|------------------------------------|
| Kruskal       | `mst/kruskal.hpp`            | edge sort + locked Union-Find      |
| Prim          | `mst/prim.hpp`               | binary-heap `std::priority_queue`  |
| Borůvka (seq) | `mst/boruvka_seq.hpp`        | classical, deterministic tie-break |
| Borůvka (OMP) | `mst/boruvka_omp.hpp`        | OpenMP min-edge reduction          |

## Tools

`gen_graph` — Erdős–Rényi generator. Writes `n m\n` header then `m` lines of `u v w`.

```
./build/tools/gen_graph 1000 0.01 42 > graph.txt
```

`bench` — reads the same format from stdin, runs one algorithm, prints one CSV row.

```
./build/tools/bench boruvka_omp 8 < graph.txt
```

CSV schema (no header row, one row per run):

```
algo,threads,n,m,mst_edges,rounds,total_weight,wall_ms
```

`threads` is the requested thread count (0 = OMP default; only meaningful for `boruvka_omp`).
`rounds` is the Borůvka iteration count (0 for Kruskal/Prim).

## Notes

- On macOS, OpenMP requires `libomp` from Homebrew (Apple Clang does not ship it).
  The CMake build looks under both `/opt/homebrew/opt/libomp` (Apple Silicon) and
  `/usr/local/opt/libomp` (Intel).
- All four implementations are cross-validated against
  `boost::kruskal_minimum_spanning_tree` on random graphs in `test_property_vs_boost`.

## License

MIT.
