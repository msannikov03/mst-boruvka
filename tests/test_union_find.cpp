#include "mst/union_find.hpp"

#include <gtest/gtest.h>

TEST(UnionFind, InitialState) {
    mst::UnionFind uf(5);
    EXPECT_EQ(uf.component_count(), 5u);
    for (uint32_t i = 0; i < 5; ++i) EXPECT_EQ(uf.find(i), i);
}

TEST(UnionFind, BasicUnion) {
    mst::UnionFind uf(5);
    EXPECT_TRUE (uf.unite(0, 1));
    EXPECT_FALSE(uf.unite(0, 1));
    EXPECT_EQ(uf.find(0), uf.find(1));
    EXPECT_EQ(uf.component_count(), 4u);
}

TEST(UnionFind, Chain) {
    mst::UnionFind uf(8);
    for (uint32_t i = 0; i + 1 < 8; ++i) uf.unite(i, i + 1);
    EXPECT_EQ(uf.component_count(), 1u);
    uint32_t r = uf.find(0);
    for (uint32_t i = 1; i < 8; ++i) EXPECT_EQ(uf.find(i), r);
}

TEST(UnionFind, ManyDisjointSets) {
    mst::UnionFind uf(100);
    for (uint32_t i = 0; i < 50; ++i) uf.unite(2 * i, 2 * i + 1);
    EXPECT_EQ(uf.component_count(), 50u);
    for (uint32_t i = 0; i < 50; ++i) {
        EXPECT_EQ(uf.find(2 * i), uf.find(2 * i + 1));
    }
}
