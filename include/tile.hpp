#ifndef WYPAS_CORE_TILE_HPP
#define WYPAS_CORE_TILE_HPP

#include "position.hpp"
#include "defs.hpp"
#include <cstdint>
#include <vector>

namespace core {

class Tile {
public:
    Tile() = default;
    explicit Tile(const Position& pos) : pos_(pos) {}
    Tile(uint16_t x, uint16_t y, uint8_t z) : pos_(x, y, z) {}

    void addItem(uint16_t itemId);
    void setGround(uint16_t itemId);
    void removeItem(uint16_t itemId);

    bool isWalkable() const;
    bool hasFlag(TileFlag flag) const { return (flags_ & static_cast<uint32_t>(flag)) != 0; }
    void setFlag(TileFlag flag) { flags_ |= static_cast<uint32_t>(flag); }
    void resetFlag(TileFlag flag) { flags_ &= ~static_cast<uint32_t>(flag); }

    uint16_t getGround() const { return ground_; }
    const std::vector<uint16_t>& getTopItems() const { return topItems_; }
    const std::vector<uint16_t>& getDownItems() const { return downItems_; }
    const Position& getPosition() const { return pos_; }
    uint32_t getFlags() const { return flags_; }

    uint32_t getItemCount() const;
    bool hasFloorChange() const { return hasFlag(TILESTATE_FLOORCHANGE); }

private:
    void updateTileFlags(uint16_t itemId, bool remove);

    Position pos_;
    uint16_t ground_ = 0;
    std::vector<uint16_t> topItems_;
    std::vector<uint16_t> downItems_;
    uint32_t flags_ = 0;
};

} // namespace core

#endif
