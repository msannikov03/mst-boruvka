#include "mst/graph_csr.hpp"

#include <cassert>
#include <cstddef>

namespace mst {

GraphCSR GraphCSR::from_edges(uint32_t n, std::vector<Edge> edges) {
    GraphCSR g;
    g.n = n;

    g.edges_unique.reserve(edges.size());
    for (auto& e : edges) {
        assert(e.u < n && e.v < n && "edge endpoint out of range");
        if (e.u == e.v) continue;        // skip self-loops
        g.edges_unique.push_back(e);
    }
    g.m = g.edges_unique.size();

    g.row_ptr.assign(static_cast<size_t>(n) + 1, 0);
    for (const auto& e : g.edges_unique) {
        ++g.row_ptr[e.u + 1];
        ++g.row_ptr[e.v + 1];
    }
    for (uint32_t i = 0; i < n; ++i) {
        g.row_ptr[i + 1] += g.row_ptr[i];
    }

    const uint64_t total = g.row_ptr.back();
    g.col_idx.resize(total);
    g.weights.resize(total);
    g.edge_ids.resize(total);

    std::vector<uint64_t> cursor(n);
    for (uint32_t i = 0; i < n; ++i) cursor[i] = g.row_ptr[i];

    for (uint64_t eid = 0; eid < g.edges_unique.size(); ++eid) {
        const Edge& e = g.edges_unique[eid];
        const uint64_t pu = cursor[e.u]++;
        g.col_idx[pu]  = e.v;
        g.weights[pu]  = e.w;
        g.edge_ids[pu] = eid;

        const uint64_t pv = cursor[e.v]++;
        g.col_idx[pv]  = e.u;
        g.weights[pv]  = e.w;
        g.edge_ids[pv] = eid;
    }
    return g;
}

}  // namespace mst
