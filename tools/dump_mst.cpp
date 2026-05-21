#include "mst/boruvka_omp.hpp"
#include "mst/boruvka_seq.hpp"
#include "mst/edge.hpp"
#include "mst/graph_csr.hpp"
#include "mst/kruskal.hpp"
#include "mst/prim.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include <vector>

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
        uint32_t u = 0, v = 0;
        double w = 0.0;
        if (std::scanf("%u %u %lf", &u, &v, &w) != 3) return 1;
        edges.push_back({u, v, w});
    }

    mst::MSTResult r;
    if (algo == "kruskal") {
        r = mst::kruskal(n, edges);
    } else {
        auto g = mst::GraphCSR::from_edges(n, edges);
        if      (algo == "prim")        r = mst::prim(g);
        else if (algo == "boruvka_seq") r = mst::boruvka_seq(g);
        else if (algo == "boruvka_omp") r = mst::boruvka_omp(g, threads);
        else { std::fprintf(stderr, "unknown algorithm: %s\n", algo.c_str()); return 2; }
    }

    std::unordered_set<uint64_t> in_mst(r.edge_ids.begin(), r.edge_ids.end());

    std::printf("%u %llu %.17g\n", n, m, r.total_weight);
    for (uint64_t i = 0; i < edges.size(); ++i) {
        const auto& e = edges[i];
        std::printf("%u %u %.17g %d\n",
            e.u, e.v, e.w, in_mst.count(i) ? 1 : 0);
    }
    return 0;
}
