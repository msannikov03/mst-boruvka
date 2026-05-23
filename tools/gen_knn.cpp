#include "mst/edge.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <set>
#include <vector>

// Geometric k-NN graph: sample n points uniformly in [0,1]^2, find each
// point's k nearest neighbours by brute force, symmetrise (edge i↔j if j
// is in i's k-NN OR i is in j's k-NN), weight = Euclidean distance.
// O(n^2) generation; fine up to n ≈ 10^4.

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: %s <n> <k> [seed]\n", argv[0]);
        return 1;
    }
    const uint32_t n    = static_cast<uint32_t>(std::atoi(argv[1]));
    const uint32_t k    = static_cast<uint32_t>(std::atoi(argv[2]));
    const uint64_t seed = argc >= 4 ? std::strtoull(argv[3], nullptr, 10) : 42ull;
    if (n < 2 || k == 0) {
        std::fprintf(stderr, "need n>=2 and k>=1\n");
        return 1;
    }

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> u01(0.0, 1.0);

    std::vector<double> xs(n), ys(n);
    for (uint32_t i = 0; i < n; ++i) {
        xs[i] = u01(rng);
        ys[i] = u01(rng);
    }

    std::set<std::pair<uint32_t, uint32_t>> edge_set;
    std::vector<std::pair<double, uint32_t>> buf(n);

    const uint32_t want = std::min<uint32_t>(k + 1, n);
    for (uint32_t i = 0; i < n; ++i) {
        for (uint32_t j = 0; j < n; ++j) {
            const double dx = xs[i] - xs[j];
            const double dy = ys[i] - ys[j];
            buf[j] = {dx * dx + dy * dy, j};
        }
        std::partial_sort(buf.begin(), buf.begin() + want, buf.end());
        for (uint32_t kk = 0; kk < want; ++kk) {
            const uint32_t j = buf[kk].second;
            if (j == i) continue;
            const uint32_t lo = i < j ? i : j;
            const uint32_t hi = i < j ? j : i;
            edge_set.insert({lo, hi});
        }
    }

    std::vector<mst::Edge> edges;
    edges.reserve(edge_set.size());
    for (auto [lo, hi] : edge_set) {
        const double dx = xs[lo] - xs[hi];
        const double dy = ys[lo] - ys[hi];
        edges.push_back({lo, hi, std::sqrt(dx * dx + dy * dy)});
    }

    std::printf("%u %zu\n", n, edges.size());
    for (const auto& e : edges) {
        std::printf("%u %u %.17g\n", e.u, e.v, e.w);
    }
    return 0;
}
