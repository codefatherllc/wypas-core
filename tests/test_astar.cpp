#include "astar.hpp"
#include "items.hpp"
#include "map.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace core;

static void registerTestItems() {
    auto& items = Items::getInstance();
    items.clear();

    ItemType ground;
    ground.id = 100;
    ground.group = ITEM_GROUP_GROUND;
    items.addItemType(100, ground);

    ItemType wall;
    wall.id = 400;
    wall.blockSolid = true;
    items.addItemType(400, wall);
}

static void testDirectPath() {
    Map map(20, 20, 1);

    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 7);
            tile.setGround(100);
        }
    }

    Position start(0, 0, 7);
    Position target(3, 0, 7);
    std::vector<Direction> dirs;

    bool found = map.findPath(start, target, 20, dirs);
    assert(found);
    assert(dirs.size() == 3);
    for (auto d : dirs) {
        assert(d == DIRECTION_EAST);
    }

    std::cout << "  PASS: direct east path" << std::endl;
}

static void testPathAroundObstacle() {
    Map map(20, 20, 1);

    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 7);
            tile.setGround(100);
        }
    }

    // Wall blocking direct east path at (2,0), (2,1), (2,2)
    for (int y = 0; y <= 2; ++y) {
        auto* tile = map.getTile(2, y, 7);
        tile->addItem(400);
    }

    Position start(0, 0, 7);
    Position target(4, 0, 7);
    std::vector<Direction> dirs;

    bool found = map.findPath(start, target, 50, dirs);
    assert(found);

    // Verify path avoids obstacles by replaying
    int cx = 0, cy = 0;
    for (auto d : dirs) {
        switch (d) {
            case DIRECTION_NORTH: cy--; break;
            case DIRECTION_SOUTH: cy++; break;
            case DIRECTION_EAST:  cx++; break;
            case DIRECTION_WEST:  cx--; break;
            case DIRECTION_NE:    cx++; cy--; break;
            case DIRECTION_NW:    cx--; cy--; break;
            case DIRECTION_SE:    cx++; cy++; break;
            case DIRECTION_SW:    cx--; cy++; break;
            default: break;
        }
        auto* tile = map.getTile(cx, cy, 7);
        assert(tile && tile->isWalkable());
    }
    assert(cx == 4 && cy == 0);

    std::cout << "  PASS: path around obstacle" << std::endl;
}

static void testNoPath() {
    Map map(20, 20, 1);

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 7);
            tile.setGround(100);
        }
    }

    // Completely wall off target
    for (int x = 0; x < 5; ++x) {
        map.getTile(x, 2, 7)->addItem(400);
    }

    Position start(0, 0, 7);
    Position target(0, 4, 7);
    std::vector<Direction> dirs;

    bool found = map.findPath(start, target, 50, dirs);
    assert(!found);

    std::cout << "  PASS: no path when fully blocked" << std::endl;
}

static void testSamePosition() {
    Position start(5, 5, 7);
    std::vector<Direction> dirs;

    Map map(10, 10, 1);
    auto& tile = map.getOrCreateTile(5, 5, 7);
    tile.setGround(100);

    bool found = map.findPath(start, start, 10, dirs);
    assert(found);
    assert(dirs.empty());

    std::cout << "  PASS: same start and target" << std::endl;
}

static void testDiagonalPath() {
    Map map(20, 20, 1);
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 7);
            tile.setGround(100);
        }
    }

    Position start(0, 0, 7);
    Position target(3, 3, 7);
    std::vector<Direction> dirs;

    bool found = map.findPath(start, target, 20, dirs);
    assert(found);
    assert(!dirs.empty());

    // Replay path and verify we reach the target
    int cx = 0, cy = 0;
    for (auto d : dirs) {
        switch (d) {
            case DIRECTION_NORTH: cy--; break;
            case DIRECTION_SOUTH: cy++; break;
            case DIRECTION_EAST:  cx++; break;
            case DIRECTION_WEST:  cx--; break;
            case DIRECTION_NE:    cx++; cy--; break;
            case DIRECTION_NW:    cx--; cy--; break;
            case DIRECTION_SE:    cx++; cy++; break;
            case DIRECTION_SW:    cx--; cy++; break;
            default: break;
        }
    }
    assert(cx == 3 && cy == 3);

    std::cout << "  PASS: diagonal path" << std::endl;
}

int main() {
    registerTestItems();

    std::cout << "test_astar:" << std::endl;
    testDirectPath();
    testPathAroundObstacle();
    testNoPath();
    testSamePosition();
    testDiagonalPath();

    std::cout << "All A* tests passed." << std::endl;
    return 0;
}
