#include "map.hpp"
#include "astar.hpp"
#include "items.hpp"
#include <queue>

namespace core {

Map::Map(uint16_t width, uint16_t height, uint8_t floors)
    : width_(width), height_(height), floors_(floors) {}

uint64_t Map::packPos(uint16_t x, uint16_t y, uint8_t z) {
    return (static_cast<uint64_t>(x) << 24) |
           (static_cast<uint64_t>(y) << 8) |
           static_cast<uint64_t>(z);
}

Tile* Map::getTile(uint16_t x, uint16_t y, uint8_t z) {
    auto it = tiles_.find(packPos(x, y, z));
    return (it != tiles_.end()) ? &it->second : nullptr;
}

const Tile* Map::getTile(uint16_t x, uint16_t y, uint8_t z) const {
    auto it = tiles_.find(packPos(x, y, z));
    return (it != tiles_.end()) ? &it->second : nullptr;
}

Tile& Map::getOrCreateTile(uint16_t x, uint16_t y, uint8_t z) {
    uint64_t key = packPos(x, y, z);
    auto it = tiles_.find(key);
    if (it != tiles_.end()) return it->second;
    auto [inserted, _] = tiles_.emplace(key, Tile(x, y, z));
    return inserted->second;
}

void Map::setTile(uint16_t x, uint16_t y, uint8_t z, const Tile& tile) {
    tiles_[packPos(x, y, z)] = tile;
}

bool Map::findPath(const Position& start, const Position& target,
                   int32_t maxDist, std::vector<Direction>& dirs) const {
    const Map* self = this;

    auto walkCost = [](int x, int y, int z, void* userdata) -> int {
        const Map* map = static_cast<const Map*>(userdata);
        const Tile* tile = map->getTile(
            static_cast<uint16_t>(x),
            static_cast<uint16_t>(y),
            static_cast<uint8_t>(z)
        );
        if (!tile || !tile->isWalkable()) return -1;
        return 0;
    };

    return core::findPath(start, target, maxDist, start.z,
                           walkCost, const_cast<Map*>(this), dirs);
}

std::vector<Position> Map::validateReachability(const std::vector<Position>& positions) const {
    if (positions.empty()) return {};

    // BFS from first position
    Position origin = positions[0];

    struct PosHash {
        size_t operator()(const Position& p) const {
            return (static_cast<size_t>(p.x) << 24) |
                   (static_cast<size_t>(p.y) << 8) |
                   static_cast<size_t>(p.z);
        }
    };

    std::unordered_map<Position, bool, PosHash> visited;
    std::queue<Position> queue;

    queue.push(origin);
    visited[origin] = true;

    static const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    static const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

    while (!queue.empty()) {
        Position cur = queue.front();
        queue.pop();

        // Cardinal + diagonal neighbors on same floor
        for (int i = 0; i < 8; ++i) {
            int nx = static_cast<int>(cur.x) + dx[i];
            int ny = static_cast<int>(cur.y) + dy[i];
            if (nx < 0 || ny < 0) continue;

            Position next(static_cast<uint16_t>(nx), static_cast<uint16_t>(ny), cur.z);
            if (visited.count(next)) continue;

            const Tile* tile = getTile(next.x, next.y, next.z);
            if (!tile || !tile->isWalkable()) continue;

            visited[next] = true;
            queue.push(next);
        }

        // Floor changes
        const Tile* curTile = getTile(cur.x, cur.y, cur.z);
        if (curTile && curTile->hasFloorChange()) {
            if (curTile->hasFlag(TILESTATE_FLOORCHANGE_DOWN) && cur.z < MAP_MAX_LAYERS - 1) {
                Position below(cur.x, cur.y, static_cast<uint8_t>(cur.z + 1));
                if (!visited.count(below)) {
                    const Tile* t = getTile(below.x, below.y, below.z);
                    if (t && t->isWalkable()) {
                        visited[below] = true;
                        queue.push(below);
                    }
                }
            }
            auto tryFloorUp = [&](TileFlag flag, int offx, int offy) {
                if (!curTile->hasFlag(flag) || cur.z == 0) return;
                Position above(
                    static_cast<uint16_t>(cur.x + offx),
                    static_cast<uint16_t>(cur.y + offy),
                    static_cast<uint8_t>(cur.z - 1)
                );
                if (!visited.count(above)) {
                    const Tile* t = getTile(above.x, above.y, above.z);
                    if (t && t->isWalkable()) {
                        visited[above] = true;
                        queue.push(above);
                    }
                }
            };
            tryFloorUp(TILESTATE_FLOORCHANGE_NORTH, 0, -1);
            tryFloorUp(TILESTATE_FLOORCHANGE_SOUTH, 0, 1);
            tryFloorUp(TILESTATE_FLOORCHANGE_EAST, 1, 0);
            tryFloorUp(TILESTATE_FLOORCHANGE_WEST, -1, 0);
        }
    }

    // Collect unreachable
    std::vector<Position> unreachable;
    for (size_t i = 1; i < positions.size(); ++i) {
        if (!visited.count(positions[i])) {
            unreachable.push_back(positions[i]);
        }
    }
    return unreachable;
}

} // namespace core
