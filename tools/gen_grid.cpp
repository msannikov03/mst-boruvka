#include "mst/edge.hpp"

#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: %s <rows> <cols> [seed]\n", argv[0]);
        return 1;
    }
    uint32_t R = static_cast<uint32_t>(std::atoi(argv[1]));
    uint32_t C = static_cast<uint32_t>(std::atoi(argv[2]));
    uint64_t seed = argc >= 4 ? std::strtoull(argv[3], nullptr, 10) : 42ull;

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> u01(0.0, 1.0);

    const uint32_t n = R * C;
    auto idx = [C](uint32_t r, uint32_t c) { return r * C + c; };

    std::vector<mst::Edge> edges;
    edges.reserve(static_cast<size_t>(2u * R * C));
    for (uint32_t r = 0; r < R; ++r) {
        for (uint32_t c = 0; c < C; ++c) {
            if (c + 1 < C) edges.push_back({idx(r, c), idx(r, c + 1), u01(rng)});
            if (r + 1 < R) edges.push_back({idx(r, c), idx(r + 1, c), u01(rng)});
        }
    }

    std::printf("%u %zu\n", n, edges.size());
    for (const auto& e : edges) {
        std::printf("%u %u %.17g\n", e.u, e.v, e.w);
    }
    return 0;
}
