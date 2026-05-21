#include "mst/boruvka_omp.hpp"
#include "mst/boruvka_seq.hpp"
#include "mst/edge.hpp"
#include "mst/graph_csr.hpp"
#include "mst/kruskal.hpp"
#include "mst/prim.hpp"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

mst::MSTResult run(const std::string& algo,
                   const std::vector<mst::Edge>& edges,
                   uint32_t n, int threads) {
    if (algo == "kruskal") return mst::kruskal(n, edges);
    auto g = mst::GraphCSR::from_edges(n, edges);
    if (algo == "prim")        return mst::prim(g);
    if (algo == "boruvka_seq") return mst::boruvka_seq(g);
    if (algo == "boruvka_omp") return mst::boruvka_omp(g, threads);
    std::fprintf(stderr, "unknown algorithm: %s\n", algo.c_str());
    std::exit(2);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr,
            "usage: %s <kruskal|prim|boruvka_seq|boruvka_omp> [threads] < graph.txt\n",
            argv[0]);
        return 1;
    }
    std::string algo    = argv[1];
    int         threads = argc >= 3 ? std::atoi(argv[2]) : 0;

    uint32_t n = 0;
    unsigned long long m = 0;
    if (std::scanf("%u %llu", &n, &m) != 2) {
        std::fprintf(stderr, "failed to read header\n");
        return 1;
    }
    std::vector<mst::Edge> edges;
    edges.reserve(static_cast<size_t>(m));
    for (unsigned long long i = 0; i < m; ++i) {
        uint32_t u = 0;
        uint32_t v = 0;
        double   w = 0.0;
        if (std::scanf("%u %u %lf", &u, &v, &w) != 3) {
            std::fprintf(stderr, "failed to read edge %llu\n", i);
            return 1;
        }
        edges.push_back({u, v, w});
    }

    auto t0 = std::chrono::steady_clock::now();
    auto r  = run(algo, edges, n, threads);
    auto t1 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::printf("%s,%d,%u,%llu,%llu,%u,%.17g,%.6f\n",
        algo.c_str(), threads, n, m,
        static_cast<unsigned long long>(r.edge_ids.size()),
        r.rounds, r.total_weight, ms);
    return 0;
}
