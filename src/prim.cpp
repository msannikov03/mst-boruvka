#include "mst/prim.hpp"

#include <queue>
#include <vector>

namespace mst {

namespace {

struct PqEntry {
    double   w;
    uint64_t edge_id;
    uint32_t to;
};

struct PqGreater {
    bool operator()(const PqEntry& a, const PqEntry& b) const noexcept {
        if (a.w != b.w) return a.w > b.w;
        return a.edge_id > b.edge_id;
    }
};

}  // namespace

MSTResult prim(const GraphCSR& g) {
    MSTResult result;
    if (g.n == 0) return result;

    std::vector<char> in_tree(g.n, 0);
    std::priority_queue<PqEntry, std::vector<PqEntry>, PqGreater> pq;

    for (uint32_t start = 0; start < g.n; ++start) {
        if (in_tree[start]) continue;

        in_tree[start] = 1;
        for (uint64_t i = g.row_ptr[start]; i < g.row_ptr[start + 1]; ++i) {
            pq.push({g.weights[i], g.edge_ids[i], g.col_idx[i]});
        }

        while (!pq.empty()) {
            PqEntry top = pq.top();
            pq.pop();
            if (in_tree[top.to]) continue;
            in_tree[top.to] = 1;
            result.edge_ids.push_back(top.edge_id);
            result.total_weight += top.w;
            for (uint64_t i = g.row_ptr[top.to]; i < g.row_ptr[top.to + 1]; ++i) {
                uint32_t nbr = g.col_idx[i];
                if (!in_tree[nbr]) {
                    pq.push({g.weights[i], g.edge_ids[i], nbr});
                }
            }
        }
    }
    return result;
}

}  // namespace mst
