#include "mst/boruvka_omp.hpp"
#include "mst/union_find.hpp"

#include <omp.h>

#include <algorithm>
#include <chrono>
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

using clk = std::chrono::steady_clock;
inline double ms_since(const clk::time_point& t0) {
    return std::chrono::duration<double, std::milli>(clk::now() - t0).count();
}
}  // namespace

MSTResult boruvka_omp(const GraphCSR& g, int num_threads, BoruvkaOMPProfile* profile) {
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
        BoruvkaOMPProfile::Round round;
        if (profile) round.components_in = uf.component_count();

        const auto t_setup_begin = clk::now();
        for (uint32_t v = 0; v < g.n; ++v) comp[v] = uf.find(v);
        for (int t = 0; t < T; ++t) {
            std::fill(tl_id[t].begin(), tl_id[t].end(), kNoEdge);
        }
        if (profile) round.setup_ms = ms_since(t_setup_begin);

        const auto t_scan_begin = clk::now();
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
        if (profile) round.scan_ms = ms_since(t_scan_begin);

        const auto t_merge_begin = clk::now();
        const int64_t N = static_cast<int64_t>(g.n);
        #pragma omp parallel for schedule(static) num_threads(T)
        for (int64_t c = 0; c < N; ++c) {
            uint64_t best_id = kNoEdge;
            double   best_w  = 0.0;
            for (int t = 0; t < T; ++t) {
                uint64_t cid = tl_id[t][static_cast<size_t>(c)];
                if (cid == kNoEdge) continue;
                if (better(best_id, best_w, cid, tl_w[t][static_cast<size_t>(c)])) {
                    best_id = cid;
                    best_w  = tl_w[t][static_cast<size_t>(c)];
                }
            }
            cheapest  [static_cast<size_t>(c)] = best_id;
            cheapest_w[static_cast<size_t>(c)] = best_w;
        }
        if (profile) round.merge_ms = ms_since(t_merge_begin);

        const auto t_contract_begin = clk::now();
        bool merged_anything = false;
        uint32_t round_edges = 0;
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
            ++round_edges;
        }
        if (profile) {
            round.contract_ms    = ms_since(t_contract_begin);
            round.edges_added    = round_edges;
            round.components_out = uf.component_count();
            profile->rounds.push_back(round);
        }
        ++result.rounds;
        if (!merged_anything) break;
    }
    return result;
}

}  // namespace mst
