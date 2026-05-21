#include "mst/edge.hpp"

#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: %s <n> <p> [seed]\n", argv[0]);
        return 1;
    }
    uint32_t n   = static_cast<uint32_t>(std::atoi(argv[1]));
    double   p   = std::atof(argv[2]);
    uint64_t seed = argc >= 4 ? std::strtoull(argv[3], nullptr, 10) : 42ull;

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

    std::printf("%u %zu\n", n, edges.size());
    for (const auto& e : edges) {
        std::printf("%u %u %.17g\n", e.u, e.v, e.w);
    }
    return 0;
}
