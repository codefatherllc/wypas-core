#ifndef WYPAS_CORE_POSITION_HPP
#define WYPAS_CORE_POSITION_HPP

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace core {

enum Direction : uint8_t {
    DIRECTION_NORTH = 0,
    DIRECTION_EAST = 1,
    DIRECTION_SOUTH = 2,
    DIRECTION_WEST = 3,
    DIRECTION_NE = 4,
    DIRECTION_NW = 5,
    DIRECTION_SE = 6,
    DIRECTION_SW = 7,
    DIRECTION_NONE = 8
};

struct Position {
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t z = 0;

    Position() = default;
    Position(uint16_t x, uint16_t y, uint8_t z) : x(x), y(y), z(z) {}

    bool operator==(const Position& o) const {
        return x == o.x && y == o.y && z == o.z;
    }

    bool operator!=(const Position& o) const {
        return !(*this == o);
    }

    int32_t distanceTo(const Position& o) const {
        return std::max({
            std::abs(static_cast<int32_t>(x) - o.x),
            std::abs(static_cast<int32_t>(y) - o.y),
            std::abs(static_cast<int32_t>(z) - o.z)
        });
    }

    static Direction directionFromDelta(int dx, int dy) {
        if (dx == 0 && dy == -1) return DIRECTION_NORTH;
        if (dx == 1 && dy == 0) return DIRECTION_EAST;
        if (dx == 0 && dy == 1) return DIRECTION_SOUTH;
        if (dx == -1 && dy == 0) return DIRECTION_WEST;
        if (dx == 1 && dy == -1) return DIRECTION_NE;
        if (dx == -1 && dy == -1) return DIRECTION_NW;
        if (dx == 1 && dy == 1) return DIRECTION_SE;
        if (dx == -1 && dy == 1) return DIRECTION_SW;
        return DIRECTION_NONE;
    }
};

} // namespace core

#endif
