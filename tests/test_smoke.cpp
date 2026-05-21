#include "mst/edge.hpp"

#include <gtest/gtest.h>

TEST(Smoke, EdgeConstruction) {
    mst::Edge e{0, 1, 1.5};
    EXPECT_EQ(e.u, 0u);
    EXPECT_EQ(e.v, 1u);
    EXPECT_DOUBLE_EQ(e.w, 1.5);
}

TEST(Smoke, MSTResultDefaults) {
    mst::MSTResult r;
    EXPECT_TRUE(r.edge_ids.empty());
    EXPECT_DOUBLE_EQ(r.total_weight, 0.0);
    EXPECT_EQ(r.rounds, 0u);
}
