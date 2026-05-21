#include "mst/union_find.hpp"

namespace mst {

UnionFind::UnionFind(uint32_t n)
    : parent_(n), rank_(n, 0), components_(n) {
    for (uint32_t i = 0; i < n; ++i) parent_[i] = i;
}

uint32_t UnionFind::find(uint32_t x) noexcept {
    while (parent_[x] != x) {
        parent_[x] = parent_[parent_[x]];
        x = parent_[x];
    }
    return x;
}

bool UnionFind::unite(uint32_t a, uint32_t b) noexcept {
    uint32_t ra = find(a);
    uint32_t rb = find(b);
    if (ra == rb) return false;
    if (rank_[ra] < rank_[rb]) {
        parent_[ra] = rb;
    } else if (rank_[ra] > rank_[rb]) {
        parent_[rb] = ra;
    } else {
        parent_[rb] = ra;
        ++rank_[ra];
    }
    --components_;
    return true;
}

}  // namespace mst
