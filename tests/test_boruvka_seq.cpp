#include "mst/boruvka_seq.hpp"

#include <gtest/gtest.h>

TEST(BoruvkaSeq, Triangle) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {1, 2, 2.0}, {0, 2, 3.0},
    };
    auto g = mst::GraphCSR::from_edges(3, edges);
    auto r = mst::boruvka_seq(g);
    EXPECT_EQ(r.edge_ids.size(), 2u);
    EXPECT_DOUBLE_EQ(r.total_weight, 3.0);
    EXPECT_GE(r.rounds, 1u);
}

TEST(BoruvkaSeq, K4) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {0, 2, 2.0}, {0, 3, 3.0},
        {1, 2, 4.0}, {1, 3, 5.0}, {2, 3, 6.0},
    };
    auto g = mst::GraphCSR::from_edges(4, edges);
    auto r = mst::boruvka_seq(g);
    EXPECT_EQ(r.edge_ids.size(), 3u);
    EXPECT_DOUBLE_EQ(r.total_weight, 6.0);
}

TEST(BoruvkaSeq, Disconnected) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {2, 3, 2.0},
    };
    auto g = mst::GraphCSR::from_edges(4, edges);
    auto r = mst::boruvka_seq(g);
    EXPECT_EQ(r.edge_ids.size(), 2u);
    EXPECT_DOUBLE_EQ(r.total_weight, 3.0);
}

TEST(BoruvkaSeq, DuplicateWeights) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {1, 2, 1.0}, {0, 2, 1.0}, {2, 3, 1.0},
    };
    auto g = mst::GraphCSR::from_edges(4, edges);
    auto r = mst::boruvka_seq(g);
    EXPECT_EQ(r.edge_ids.size(), 3u);
    EXPECT_DOUBLE_EQ(r.total_weight, 3.0);
}

TEST(BoruvkaSeq, RoundsBound) {
    constexpr uint32_t N = 16;
    std::vector<mst::Edge> edges;
    for (uint32_t i = 0; i + 1 < N; ++i) {
        edges.push_back({i, i + 1, static_cast<double>(i + 1)});
    }
    auto g = mst::GraphCSR::from_edges(N, edges);
    auto r = mst::boruvka_seq(g);
    EXPECT_EQ(r.edge_ids.size(), static_cast<size_t>(N - 1));
    uint32_t log2N = 0;
    for (uint32_t v = N - 1; v > 0; v >>= 1) ++log2N;
    EXPECT_LE(r.rounds, log2N);
}

TEST(BoruvkaSeq, SingleVertex) {
    std::vector<mst::Edge> edges;
    auto g = mst::GraphCSR::from_edges(1, edges);
    auto r = mst::boruvka_seq(g);
    EXPECT_EQ(r.edge_ids.size(), 0u);
    EXPECT_DOUBLE_EQ(r.total_weight, 0.0);
}

TEST(BoruvkaSeq, EmptyGraph) {
    std::vector<mst::Edge> edges;
    auto g = mst::GraphCSR::from_edges(0, edges);
    auto r = mst::boruvka_seq(g);
    EXPECT_EQ(r.edge_ids.size(), 0u);
}

TEST(BoruvkaSeq, SelfLoopsIgnored) {
    std::vector<mst::Edge> edges = {
        {0, 0, 0.1}, {0, 1, 1.0}, {1, 1, 0.2}, {1, 2, 2.0},
    };
    auto g = mst::GraphCSR::from_edges(3, edges);
    auto r = mst::boruvka_seq(g);
    EXPECT_EQ(r.edge_ids.size(), 2u);
    EXPECT_DOUBLE_EQ(r.total_weight, 3.0);
}
