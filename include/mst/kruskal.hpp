#pragma once

#include "mst/edge.hpp"

#include <cstdint>
#include <vector>

namespace mst {

MSTResult kruskal(uint32_t n, const std::vector<Edge>& edges);

}  // namespace mst
