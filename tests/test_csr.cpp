#include "mst/graph_csr.hpp"

#include <gtest/gtest.h>

TEST(GraphCSR, Triangle) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {1, 2, 2.0}, {0, 2, 3.0},
    };
    auto g = mst::GraphCSR::from_edges(3, edges);
    EXPECT_EQ(g.n, 3u);
    EXPECT_EQ(g.m, 3u);
    EXPECT_EQ(g.row_ptr.size(), 4u);
    EXPECT_EQ(g.col_idx.size(), 6u);
    EXPECT_EQ(g.row_ptr.back(), 6u);
    for (uint32_t v = 0; v < 3; ++v) {
        EXPECT_EQ(g.deg_end(v) - g.deg_begin(v), 2u);
    }
}

TEST(GraphCSR, IsolatedVertex) {
    std::vector<mst::Edge> edges = {{0, 1, 5.0}};
    auto g = mst::GraphCSR::from_edges(3, edges);
    EXPECT_EQ(g.deg_end(2) - g.deg_begin(2), 0u);
    EXPECT_EQ(g.deg_end(0) - g.deg_begin(0), 1u);
}

TEST(GraphCSR, EdgeIdsMatchUnique) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {1, 2, 2.0}, {2, 3, 3.0},
    };
    auto g = mst::GraphCSR::from_edges(4, edges);
    for (uint64_t i = 0; i < g.col_idx.size(); ++i) {
        const auto& e = g.edges_unique[g.edge_ids[i]];
        EXPECT_DOUBLE_EQ(e.w, g.weights[i]);
    }
}
