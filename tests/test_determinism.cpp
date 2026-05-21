#include "mst/boruvka_omp.hpp"
#include "mst/boruvka_seq.hpp"
#include "mst/graph_csr.hpp"
#include "mst/kruskal.hpp"
#include "mst/prim.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>

namespace {

std::vector<mst::Edge> random_graph(uint32_t n, double p, uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> u01(0.0, 1.0);
    std::vector<mst::Edge> edges;
    for (uint32_t i = 0; i < n; ++i) {
        for (uint32_t j = i + 1; j < n; ++j) {
            if (u01(rng) < p) {
                edges.push_back({i, j, u01(rng)});
            }
        }
    }
    std::vector<uint32_t> perm(n);
    for (uint32_t i = 0; i < n; ++i) perm[i] = i;
    std::shuffle(perm.begin(), perm.end(), rng);
    for (uint32_t i = 0; i + 1 < n; ++i) {
        edges.push_back({perm[i], perm[i + 1], 1.0 + u01(rng)});
    }
    return edges;
}

}  // namespace

TEST(Determinism, BoruvkaOMPReruns) {
    auto edges = random_graph(80, 0.25, 0x1234);
    auto g = mst::GraphCSR::from_edges(80, edges);

    auto a = mst::boruvka_omp(g, 4);
    auto b = mst::boruvka_omp(g, 4);
    auto c = mst::boruvka_omp(g, 4);

    EXPECT_DOUBLE_EQ(a.total_weight, b.total_weight);
    EXPECT_DOUBLE_EQ(b.total_weight, c.total_weight);

    std::vector<uint64_t> ea = a.edge_ids; std::sort(ea.begin(), ea.end());
    std::vector<uint64_t> eb = b.edge_ids; std::sort(eb.begin(), eb.end());
    std::vector<uint64_t> ec = c.edge_ids; std::sort(ec.begin(), ec.end());
    EXPECT_EQ(ea, eb);
    EXPECT_EQ(eb, ec);
}

TEST(Determinism, BoruvkaOMPThreadCountInvariant) {
    auto edges = random_graph(60, 0.3, 0x5678);
    auto g = mst::GraphCSR::from_edges(60, edges);

    auto r1 = mst::boruvka_omp(g, 1);
    auto r2 = mst::boruvka_omp(g, 2);
    auto r4 = mst::boruvka_omp(g, 4);
    auto r8 = mst::boruvka_omp(g, 8);

    EXPECT_DOUBLE_EQ(r1.total_weight, r2.total_weight);
    EXPECT_DOUBLE_EQ(r2.total_weight, r4.total_weight);
    EXPECT_DOUBLE_EQ(r4.total_weight, r8.total_weight);

    std::vector<uint64_t> e1 = r1.edge_ids; std::sort(e1.begin(), e1.end());
    std::vector<uint64_t> e2 = r2.edge_ids; std::sort(e2.begin(), e2.end());
    std::vector<uint64_t> e4 = r4.edge_ids; std::sort(e4.begin(), e4.end());
    std::vector<uint64_t> e8 = r8.edge_ids; std::sort(e8.begin(), e8.end());
    EXPECT_EQ(e1, e2);
    EXPECT_EQ(e2, e4);
    EXPECT_EQ(e4, e8);
}
