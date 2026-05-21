#include "mst/union_find_atomic.hpp"

#include <gtest/gtest.h>

#include <random>
#include <thread>
#include <vector>

TEST(UnionFindAtomic, Sequential) {
    mst::UnionFindAtomic uf(5);
    EXPECT_TRUE (uf.unite(0, 1));
    EXPECT_FALSE(uf.unite(0, 1));
    EXPECT_EQ(uf.find(0), uf.find(1));
}

TEST(UnionFindAtomic, ConcurrentMerges) {
    constexpr uint32_t N = 1000;
    mst::UnionFindAtomic uf(N);
    const int T = 8;

    std::vector<std::thread> threads;
    threads.reserve(static_cast<size_t>(T));
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&, t]() {
            std::mt19937 rng(0xDEADBEEF ^ static_cast<unsigned>(t));
            std::uniform_int_distribution<uint32_t> dist(0, N - 1);
            for (int i = 0; i < 5000; ++i) {
                uf.unite(dist(rng), dist(rng));
            }
        });
    }
    for (auto& th : threads) th.join();

    for (uint32_t i = 0; i < N; ++i) {
        uint32_t r = uf.find(i);
        EXPECT_EQ(uf.find(r), r);
    }
}

TEST(UnionFindAtomic, ConcurrentConnectivity) {
    constexpr uint32_t N = 256;
    mst::UnionFindAtomic uf(N);
    const int T = 4;

    std::vector<std::thread> threads;
    threads.reserve(static_cast<size_t>(T));
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&, t]() {
            for (uint32_t i = static_cast<uint32_t>(t); i + 1 < N;
                 i += static_cast<uint32_t>(T)) {
                uf.unite(i, i + 1);
            }
        });
    }
    for (auto& th : threads) th.join();

    uint32_t root0 = uf.find(0);
    for (uint32_t i = 1; i < N; ++i) {
        EXPECT_EQ(uf.find(i), root0);
    }
}
