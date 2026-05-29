#include "mst/edge.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>
#include <vector>

// Erdős–Rényi G(n, p) generator. Skip-trick (geometric inter-arrival on the
// pair sequence) keeps generation O(n + m) so n=10^6 sparse graphs are feasible.

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: %s <n> <p> [seed]\n", argv[0]);
        return 1;
    }
    const uint32_t n    = static_cast<uint32_t>(std::atoi(argv[1]));
    const double   p    = std::atof(argv[2]);
    const uint64_t seed = argc >= 4 ? std::strtoull(argv[3], nullptr, 10) : 42ull;

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> u01(0.0, 1.0);

    std::vector<mst::Edge> edges;

    if (p > 0.0 && p < 1.0 && n >= 2) {
        const double log_1mp = std::log1p(-p);
        for (uint32_t i = 0; i + 1 < n; ++i) {
            uint64_t j = i;
            while (true) {
                double u = u01(rng);
                // Guard both ends: u==0 makes log(u)=-inf (gap overflow / UB on
                // the uint64 cast); u>=1 makes log(u)>=0. u01 samples [0,1), so
                // only the lower clamp can actually fire, but keep both for safety.
                if (u <= 0.0) u = std::numeric_limits<double>::min();
                else if (u >= 1.0) u = 1.0 - std::numeric_limits<double>::epsilon();
                const uint64_t gap = static_cast<uint64_t>(std::floor(std::log(u) / log_1mp));
                j += gap + 1;
                if (j >= n) break;
                edges.push_back({i, static_cast<uint32_t>(j), u01(rng)});
            }
        }
    } else if (p >= 1.0 && n >= 2) {
        edges.reserve(static_cast<size_t>(n) * (n - 1) / 2);
        for (uint32_t i = 0; i + 1 < n; ++i) {
            for (uint32_t j = i + 1; j < n; ++j) {
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
