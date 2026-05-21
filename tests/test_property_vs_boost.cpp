#include "mst/boruvka_omp.hpp"
#include "mst/boruvka_seq.hpp"
#include "mst/graph_csr.hpp"
#include "mst/kruskal.hpp"
#include "mst/prim.hpp"

#include <gtest/gtest.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

#include <algorithm>
#include <random>
#include <vector>

namespace {

double boost_mst_weight(uint32_t n, const std::vector<mst::Edge>& edges) {
    using Graph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::undirectedS,
        boost::no_property,
        boost::property<boost::edge_weight_t, double>>;

    Graph g(n);
    for (const auto& e : edges) {
        boost::add_edge(e.u, e.v, e.w, g);
    }
    std::vector<boost::graph_traits<Graph>::edge_descriptor> chosen;
    boost::kruskal_minimum_spanning_tree(g, std::back_inserter(chosen));

    auto wmap = boost::get(boost::edge_weight, g);
    double total = 0.0;
    for (auto e : chosen) total += wmap[e];
    return total;
}

std::vector<mst::Edge> random_connected_graph(uint32_t n, double p,
                                              std::mt19937_64& rng) {
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

TEST(PropertyVsBoost, AllAlgorithmsMatch) {
    std::mt19937_64 rng(0xC0FFEEull);
    constexpr int kTrials = 60;
    for (int trial = 0; trial < kTrials; ++trial) {
        uint32_t n = static_cast<uint32_t>(std::uniform_int_distribution<int>(8, 40)(rng));
        double   p = std::uniform_real_distribution<double>(0.15, 0.6)(rng);
        auto edges = random_connected_graph(n, p, rng);
        auto g = mst::GraphCSR::from_edges(n, edges);

        double oracle = boost_mst_weight(n, edges);

        auto rk = mst::kruskal(n, edges);
        auto rp = mst::prim(g);
        auto rb = mst::boruvka_seq(g);
        auto ro = mst::boruvka_omp(g, 4);

        EXPECT_NEAR(rk.total_weight, oracle, 1e-9)
            << "trial=" << trial << " (kruskal) n=" << n << " p=" << p;
        EXPECT_NEAR(rp.total_weight, oracle, 1e-9)
            << "trial=" << trial << " (prim) n=" << n << " p=" << p;
        EXPECT_NEAR(rb.total_weight, oracle, 1e-9)
            << "trial=" << trial << " (boruvka_seq) n=" << n << " p=" << p;
        EXPECT_NEAR(ro.total_weight, oracle, 1e-9)
            << "trial=" << trial << " (boruvka_omp) n=" << n << " p=" << p;

        EXPECT_EQ(rk.edge_ids.size(), static_cast<size_t>(n - 1));
        EXPECT_EQ(rp.edge_ids.size(), static_cast<size_t>(n - 1));
        EXPECT_EQ(rb.edge_ids.size(), static_cast<size_t>(n - 1));
        EXPECT_EQ(ro.edge_ids.size(), static_cast<size_t>(n - 1));
    }
}

TEST(PropertyVsBoost, DuplicateWeightsStress) {
    std::mt19937_64 rng(0xABCDull);
    constexpr int kTrials = 30;
    for (int trial = 0; trial < kTrials; ++trial) {
        uint32_t n = 32;
        std::uniform_real_distribution<double> u01(0.0, 1.0);
        std::vector<mst::Edge> edges;
        for (uint32_t i = 0; i < n; ++i) {
            for (uint32_t j = i + 1; j < n; ++j) {
                if (u01(rng) < 0.4) {
                    double w = static_cast<double>(
                        std::uniform_int_distribution<int>(1, 3)(rng));
                    edges.push_back({i, j, w});
                }
            }
        }
        std::vector<uint32_t> perm(n);
        for (uint32_t i = 0; i < n; ++i) perm[i] = i;
        std::shuffle(perm.begin(), perm.end(), rng);
        for (uint32_t i = 0; i + 1 < n; ++i) {
            edges.push_back({perm[i], perm[i + 1], 5.0});
        }

        auto g = mst::GraphCSR::from_edges(n, edges);
        double oracle = boost_mst_weight(n, edges);

        auto rk = mst::kruskal(n, edges);
        auto rp = mst::prim(g);
        auto rb = mst::boruvka_seq(g);
        auto ro = mst::boruvka_omp(g, 4);

        EXPECT_NEAR(rk.total_weight, oracle, 1e-9) << "trial=" << trial;
        EXPECT_NEAR(rp.total_weight, oracle, 1e-9) << "trial=" << trial;
        EXPECT_NEAR(rb.total_weight, oracle, 1e-9) << "trial=" << trial;
        EXPECT_NEAR(ro.total_weight, oracle, 1e-9) << "trial=" << trial;
    }
}
