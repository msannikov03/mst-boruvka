#pragma once

#include "mst/edge.hpp"
#include "mst/graph_csr.hpp"

#include <cstdint>
#include <vector>

namespace mst {

struct BoruvkaOMPProfile {
    struct Round {
        double   setup_ms       = 0.0;
        double   scan_ms        = 0.0;
        double   merge_ms       = 0.0;
        double   contract_ms    = 0.0;
        uint32_t edges_added    = 0;
        uint32_t components_in  = 0;
        uint32_t components_out = 0;
    };
    std::vector<Round> rounds;
};

MSTResult boruvka_omp(const GraphCSR& g,
                      int num_threads = 0,
                      BoruvkaOMPProfile* profile = nullptr);

}  // namespace mst
