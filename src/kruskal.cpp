#include "mst/kruskal.hpp"
#include "mst/union_find.hpp"

#include <algorithm>
#include <numeric>

namespace mst {

MSTResult kruskal(uint32_t n, const std::vector<Edge>& edges) {
    MSTResult result;
    if (n == 0) return result;

    std::vector<uint64_t> idx(edges.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(),
        [&](uint64_t a, uint64_t b) {
            if (edges[a].w != edges[b].w) return edges[a].w < edges[b].w;
            return a < b;
        });

    UnionFind uf(n);
    result.edge_ids.reserve(n > 0 ? static_cast<size_t>(n - 1) : 0);
    for (uint64_t k : idx) {
        const Edge& e = edges[k];
        if (uf.unite(e.u, e.v)) {
            result.edge_ids.push_back(k);
            result.total_weight += e.w;
            if (result.edge_ids.size() + 1 == n) break;
        }
    }
    return result;
}

}  // namespace mst
