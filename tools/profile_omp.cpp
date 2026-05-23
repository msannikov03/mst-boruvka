#include "mst/boruvka_omp.hpp"
#include "mst/edge.hpp"
#include "mst/graph_csr.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <threads> [seed_tag] < graph.txt\n", argv[0]);
        return 1;
    }
    const int threads = std::atoi(argv[1]);
    const std::string tag = argc >= 3 ? argv[2] : "";

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
        if (std::scanf("%u %u %lf", &u, &v, &w) != 3) {
            std::fprintf(stderr, "failed to read edge %llu\n", i);
            return 1;
        }
        edges.push_back({u, v, w});
    }
    auto g = mst::GraphCSR::from_edges(n, edges);

    mst::BoruvkaOMPProfile profile;
    auto r = mst::boruvka_omp(g, threads, &profile);

    std::printf("threads,seed,round,setup_ms,scan_ms,merge_ms,contract_ms,"
                "edges_added,components_in,components_out\n");
    for (size_t i = 0; i < profile.rounds.size(); ++i) {
        const auto& rd = profile.rounds[i];
        std::printf("%d,%s,%zu,%.6f,%.6f,%.6f,%.6f,%u,%u,%u\n",
            threads, tag.c_str(), i,
            rd.setup_ms, rd.scan_ms, rd.merge_ms, rd.contract_ms,
            rd.edges_added, rd.components_in, rd.components_out);
    }
    // Quick sanity to stderr.
    std::fprintf(stderr, "n=%u m=%llu threads=%d rounds=%u total_weight=%.6f mst_edges=%zu\n",
        n, m, threads, r.rounds, r.total_weight, r.edge_ids.size());
    return 0;
}
