#ifndef WYPAS_CORE_TILE_HPP
#define WYPAS_CORE_TILE_HPP

#include "position.hpp"
#include "defs.hpp"
#include <atomic>
#include <cstdint>
#include <vector>

namespace core {

class Tile {
public:
    Tile() = default;
    explicit Tile(const Position& pos) : pos_(pos) {}
    Tile(uint16_t x, uint16_t y, uint8_t z) : pos_(x, y, z) {}
    Tile(const Tile& o)
        : pos_(o.pos_), ground_(o.ground_), topItems_(o.topItems_),
          downItems_(o.downItems_), flags_(o.flags_),
          walkFlags_(o.walkFlags_.load(std::memory_order_relaxed)) {}
    Tile& operator=(const Tile& o) {
        if (this != &o) {
            pos_ = o.pos_;
            ground_ = o.ground_;
            topItems_ = o.topItems_;
            downItems_ = o.downItems_;
            flags_ = o.flags_;
            walkFlags_.store(o.walkFlags_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
        return *this;
    }

    void addItem(uint16_t itemId);
    void setGround(uint16_t itemId);
    void removeItem(uint16_t itemId);

    bool isWalkable() const;
    bool blocksSolid() const { return (walkFlags_.load(std::memory_order_relaxed) & WALK_FLAG_SOLID) != 0; }
    bool blocksProjectile() const { return (walkFlags_.load(std::memory_order_relaxed) & WALK_FLAG_PROJECTILE) != 0; }
    bool blocksPathFind() const { return (walkFlags_.load(std::memory_order_relaxed) & WALK_FLAG_PATHBLOCK) != 0; }
    uint8_t walkFlags() const { return walkFlags_.load(std::memory_order_relaxed); }
    void updateWalkFlags();

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
    bool hasFloorChange(FloorChange dir) const;

    ZoneType_t getZone() const {
        if (hasFlag(TILESTATE_PROTECTIONZONE)) return ZONE_PROTECTION;
        if (hasFlag(TILESTATE_OPTIONALZONE)) return ZONE_OPTIONAL;
        if (hasFlag(TILESTATE_HARDCOREZONE)) return ZONE_HARDCORE;
        if (hasFlag(TILESTATE_NOLOGOUT)) return ZONE_NOLOGOUT;
        return ZONE_OPEN;
    }

private:
    void updateTileFlags(uint16_t itemId, bool remove);

    Position pos_;
    uint16_t ground_ = 0;
    std::vector<uint16_t> topItems_;
    std::vector<uint16_t> downItems_;
    uint32_t flags_ = 0;
    std::atomic<uint8_t> walkFlags_{0};
};

} // namespace core

#endif
