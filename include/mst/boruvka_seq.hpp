#pragma once

#include "mst/edge.hpp"
#include "mst/graph_csr.hpp"

namespace mst {

MSTResult boruvka_seq(const GraphCSR& g);

}  // namespace mst
