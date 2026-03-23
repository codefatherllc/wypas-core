#ifndef WYPAS_CORE_MAP_HPP
#define WYPAS_CORE_MAP_HPP

#include "position.hpp"
#include "tile.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace core {

class Map {
public:
    Map() = default;
    Map(uint16_t width, uint16_t height, uint8_t floors);

    Tile* getTile(uint16_t x, uint16_t y, uint8_t z);
    const Tile* getTile(uint16_t x, uint16_t y, uint8_t z) const;
    Tile& getOrCreateTile(uint16_t x, uint16_t y, uint8_t z);

    void setTile(uint16_t x, uint16_t y, uint8_t z, const Tile& tile);

    bool findPath(const Position& start, const Position& target,
                  int32_t maxDist, std::vector<Direction>& dirs) const;

    std::vector<Position> validateReachability(const std::vector<Position>& positions) const;

    uint16_t getWidth() const { return width_; }
    uint16_t getHeight() const { return height_; }
    uint8_t getFloors() const { return floors_; }
    size_t tileCount() const { return tiles_.size(); }

private:
    static uint64_t packPos(uint16_t x, uint16_t y, uint8_t z);

    std::unordered_map<uint64_t, Tile> tiles_;
    uint16_t width_ = 0;
    uint16_t height_ = 0;
    uint8_t floors_ = 0;
};

} // namespace core

#endif
