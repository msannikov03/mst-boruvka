#include "mst/boruvka_seq.hpp"
#include "mst/union_find.hpp"

#include <algorithm>
#include <limits>

namespace mst {

namespace {
constexpr uint64_t kNoEdge = std::numeric_limits<uint64_t>::max();
}

MSTResult boruvka_seq(const GraphCSR& g) {
    MSTResult result;
    if (g.n == 0 || g.m == 0) return result;

    UnionFind uf(g.n);
    std::vector<uint64_t> cheapest  (g.n, kNoEdge);
    std::vector<double>   cheapest_w(g.n, 0.0);
    std::vector<char>     in_mst    (g.m, 0);

    while (uf.component_count() > 1 && result.edge_ids.size() + 1 < g.n) {
        std::fill(cheapest.begin(), cheapest.end(), kNoEdge);

        for (uint64_t k = 0; k < g.edges_unique.size(); ++k) {
            const Edge& e = g.edges_unique[k];
            uint32_t ca = uf.find(e.u);
            uint32_t cb = uf.find(e.v);
            if (ca == cb) continue;

            auto consider = [&](uint32_t root) {
                if (cheapest[root] == kNoEdge ||
                    e.w < cheapest_w[root] ||
                    (e.w == cheapest_w[root] && k < cheapest[root])) {
                    cheapest  [root] = k;
                    cheapest_w[root] = e.w;
                }
            };
            consider(ca);
            consider(cb);
        }

        bool merged_anything = false;
        for (uint32_t c = 0; c < g.n; ++c) {
            uint64_t eid = cheapest[c];
            if (eid == kNoEdge)  continue;
            if (in_mst[eid])     continue;
            const Edge& e = g.edges_unique[eid];
            if (uf.find(e.u) == uf.find(e.v)) continue;
            in_mst[eid] = 1;
            uf.unite(e.u, e.v);
            result.edge_ids.push_back(eid);
            result.total_weight += e.w;
            merged_anything = true;
        }
        ++result.rounds;
        if (!merged_anything) break;
    }
    return result;
}

}  // namespace mst
