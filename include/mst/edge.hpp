#pragma once

#include <cstdint>
#include <vector>

namespace mst {

struct Edge {
    uint32_t u;
    uint32_t v;
    double   w;
};

struct MSTResult {
    std::vector<uint64_t> edge_ids;
    double   total_weight = 0.0;
    uint32_t rounds       = 0;
};

}  // namespace mst
