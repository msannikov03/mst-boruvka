#include "mst/boruvka_omp.hpp"

#include <gtest/gtest.h>

TEST(BoruvkaOMP, Triangle) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {1, 2, 2.0}, {0, 2, 3.0},
    };
    auto g = mst::GraphCSR::from_edges(3, edges);
    auto r = mst::boruvka_omp(g, 4);
    EXPECT_EQ(r.edge_ids.size(), 2u);
    EXPECT_DOUBLE_EQ(r.total_weight, 3.0);
}

TEST(BoruvkaOMP, K4) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {0, 2, 2.0}, {0, 3, 3.0},
        {1, 2, 4.0}, {1, 3, 5.0}, {2, 3, 6.0},
    };
    auto g = mst::GraphCSR::from_edges(4, edges);
    auto r = mst::boruvka_omp(g, 4);
    EXPECT_EQ(r.edge_ids.size(), 3u);
    EXPECT_DOUBLE_EQ(r.total_weight, 6.0);
}

TEST(BoruvkaOMP, ThreadCountVariations) {
    std::vector<mst::Edge> edges = {
        {0, 1, 1.0}, {0, 2, 2.0}, {0, 3, 3.0},
        {1, 2, 4.0}, {1, 3, 5.0}, {2, 3, 6.0},
        {0, 4, 7.0}, {1, 4, 8.0}, {2, 4, 9.0}, {3, 4, 0.5},
    };
    auto g = mst::GraphCSR::from_edges(5, edges);
    double base = mst::boruvka_omp(g, 1).total_weight;
    for (int t : {1, 2, 4, 8}) {
        auto r = mst::boruvka_omp(g, t);
        EXPECT_DOUBLE_EQ(r.total_weight, base) << "threads=" << t;
        EXPECT_EQ(r.edge_ids.size(), 4u);
    }
}
