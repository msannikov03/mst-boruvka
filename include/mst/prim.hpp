#pragma once

#include "mst/edge.hpp"
#include "mst/graph_csr.hpp"

namespace mst {

MSTResult prim(const GraphCSR& g);

}  // namespace mst
