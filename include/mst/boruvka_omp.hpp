#pragma once

#include "mst/edge.hpp"
#include "mst/graph_csr.hpp"

namespace mst {

MSTResult boruvka_omp(const GraphCSR& g, int num_threads = 0);

}  // namespace mst
