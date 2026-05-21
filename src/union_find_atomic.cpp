#include "mst/union_find_atomic.hpp"

namespace mst {

UnionFindAtomic::UnionFindAtomic(uint32_t n) : parent_(n) {
    for (uint32_t i = 0; i < n; ++i) {
        parent_[i].store(i, std::memory_order_relaxed);
    }
}

uint32_t UnionFindAtomic::find(uint32_t x) noexcept {
    while (true) {
        uint32_t p = parent_[x].load(std::memory_order_acquire);
        if (p == x) return x;
        uint32_t gp = parent_[p].load(std::memory_order_acquire);
        if (gp == p) return p;
        parent_[x].compare_exchange_weak(p, gp,
                                         std::memory_order_release,
                                         std::memory_order_relaxed);
        x = gp;
    }
}

bool UnionFindAtomic::unite(uint32_t a, uint32_t b) noexcept {
    while (true) {
        uint32_t ra = find(a);
        uint32_t rb = find(b);
        if (ra == rb) return false;
        uint32_t lo = ra < rb ? ra : rb;
        uint32_t hi = ra < rb ? rb : ra;
        uint32_t expected = hi;
        if (parent_[hi].compare_exchange_strong(expected, lo,
                                                std::memory_order_acq_rel,
                                                std::memory_order_acquire)) {
            return true;
        }
    }
}

}  // namespace mst
