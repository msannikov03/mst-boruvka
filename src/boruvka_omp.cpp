#include "mst/boruvka_omp.hpp"
#include "mst/union_find.hpp"

#include <omp.h>

#include <algorithm>
#include <limits>
#include <vector>

namespace mst {

namespace {
constexpr uint64_t kNoEdge = std::numeric_limits<uint64_t>::max();

inline bool better(uint64_t cur_id, double cur_w,
                   uint64_t cand_id, double cand_w) noexcept {
    if (cur_id == kNoEdge) return true;
    if (cand_w != cur_w)   return cand_w < cur_w;
    return cand_id < cur_id;
}
}  // namespace

MSTResult boruvka_omp(const GraphCSR& g, int num_threads) {
    MSTResult result;
    if (g.n == 0 || g.m == 0) return result;

    const int T = num_threads > 0 ? num_threads : omp_get_max_threads();

    UnionFind uf(g.n);
    std::vector<uint64_t> cheapest  (g.n, kNoEdge);
    std::vector<double>   cheapest_w(g.n, 0.0);
    std::vector<char>     in_mst    (g.m, 0);
    std::vector<uint32_t> comp      (g.n);

    std::vector<std::vector<uint64_t>> tl_id(static_cast<size_t>(T),
        std::vector<uint64_t>(g.n, kNoEdge));
    std::vector<std::vector<double>>   tl_w (static_cast<size_t>(T),
        std::vector<double>(g.n, 0.0));

    while (uf.component_count() > 1 && result.edge_ids.size() + 1 < g.n) {
        for (uint32_t v = 0; v < g.n; ++v) comp[v] = uf.find(v);
        for (int t = 0; t < T; ++t) {
            std::fill(tl_id[t].begin(), tl_id[t].end(), kNoEdge);
        }

        const int64_t M = static_cast<int64_t>(g.m);
        #pragma omp parallel num_threads(T)
        {
            const int tid = omp_get_thread_num();
            #pragma omp for schedule(static) nowait
            for (int64_t k = 0; k < M; ++k) {
                const Edge& e = g.edges_unique[static_cast<size_t>(k)];
                uint32_t ca = comp[e.u];
                uint32_t cb = comp[e.v];
                if (ca == cb) continue;

                auto consider = [&](uint32_t root) {
                    if (better(tl_id[tid][root], tl_w[tid][root],
                               static_cast<uint64_t>(k), e.w)) {
                        tl_id[tid][root] = static_cast<uint64_t>(k);
                        tl_w [tid][root] = e.w;
                    }
                };
                consider(ca);
                consider(cb);
            }
        }

        #pragma omp parallel for schedule(static) num_threads(T)
        for (uint32_t c = 0; c < g.n; ++c) {
            uint64_t best_id = kNoEdge;
            double   best_w  = 0.0;
            for (int t = 0; t < T; ++t) {
                uint64_t cid = tl_id[t][c];
                if (cid == kNoEdge) continue;
                if (better(best_id, best_w, cid, tl_w[t][c])) {
                    best_id = cid;
                    best_w  = tl_w[t][c];
                }
            }
            cheapest  [c] = best_id;
            cheapest_w[c] = best_w;
        }

        bool merged_anything = false;
        for (uint32_t c = 0; c < g.n; ++c) {
            uint64_t eid = cheapest[c];
            if (eid == kNoEdge) continue;
            if (in_mst[eid])    continue;
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
