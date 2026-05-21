#pragma once

#include "mst/edge.hpp"

#include <cstdint>
#include <vector>

namespace mst {

class GraphCSR {
public:
    uint32_t n = 0;
    uint64_t m = 0;
    std::vector<uint64_t> row_ptr;
    std::vector<uint32_t> col_idx;
    std::vector<double>   weights;
    std::vector<uint64_t> edge_ids;
    std::vector<Edge>     edges_unique;

    static GraphCSR from_edges(uint32_t n, std::vector<Edge> edges);

    uint64_t deg_begin(uint32_t u) const noexcept { return row_ptr[u]; }
    uint64_t deg_end  (uint32_t u) const noexcept { return row_ptr[u + 1]; }
};

}  // namespace mst
