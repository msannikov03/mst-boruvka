#include "mst/prim.hpp"

#include <gtest/gtest.h>

TEST(Prim, Triangle) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {1, 2, 2.0}, {0, 2, 3.0},
    };
    auto g = mst::GraphCSR::from_edges(3, edges);
    auto r = mst::prim(g);
    EXPECT_EQ(r.edge_ids.size(), 2u);
    EXPECT_DOUBLE_EQ(r.total_weight, 3.0);
}

TEST(Prim, K4) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {0, 2, 2.0}, {0, 3, 3.0},
        {1, 2, 4.0}, {1, 3, 5.0}, {2, 3, 6.0},
    };
    auto g = mst::GraphCSR::from_edges(4, edges);
    auto r = mst::prim(g);
    EXPECT_EQ(r.edge_ids.size(), 3u);
    EXPECT_DOUBLE_EQ(r.total_weight, 6.0);
}

TEST(Prim, Disconnected) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {2, 3, 2.0},
    };
    auto g = mst::GraphCSR::from_edges(4, edges);
    auto r = mst::prim(g);
    EXPECT_EQ(r.edge_ids.size(), 2u);
    EXPECT_DOUBLE_EQ(r.total_weight, 3.0);
}
