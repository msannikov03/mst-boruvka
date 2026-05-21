#pragma once

#include <cstdint>
#include <vector>

namespace mst {

class UnionFind {
public:
    explicit UnionFind(uint32_t n);

    uint32_t find(uint32_t x) noexcept;
    bool     unite(uint32_t a, uint32_t b) noexcept;

    uint32_t component_count() const noexcept { return components_; }
    uint32_t size()            const noexcept { return static_cast<uint32_t>(parent_.size()); }

private:
    std::vector<uint32_t> parent_;
    std::vector<uint8_t>  rank_;
    uint32_t components_ = 0;
};

}  // namespace mst
