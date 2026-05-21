#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

namespace mst {

class UnionFindAtomic {
public:
    explicit UnionFindAtomic(uint32_t n);

    UnionFindAtomic(const UnionFindAtomic&)            = delete;
    UnionFindAtomic& operator=(const UnionFindAtomic&) = delete;

    uint32_t find(uint32_t x) noexcept;
    bool     unite(uint32_t a, uint32_t b) noexcept;

    uint32_t size() const noexcept { return static_cast<uint32_t>(parent_.size()); }

private:
    std::vector<std::atomic<uint32_t>> parent_;
};

}  // namespace mst
