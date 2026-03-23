#include "map.hpp"
#include "items.hpp"
#include "wypas_core.hpp"
#include <cassert>
#include <iostream>

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

    // Floor change down item
    ItemType stairDown;
    stairDown.id = 500;
    stairDown.group = ITEM_GROUP_NONE;
    stairDown.floorChange[CHANGE_DOWN] = true;
    items.addItemType(500, stairDown);

    // Floor change north (going up)
    ItemType stairUp;
    stairUp.id = 501;
    stairUp.group = ITEM_GROUP_NONE;
    stairUp.floorChange[CHANGE_NORTH] = true;
    items.addItemType(501, stairUp);
}

static void testMapCreation() {
    Map map(100, 100, 8);
    assert(map.getWidth() == 100);
    assert(map.getHeight() == 100);
    assert(map.getFloors() == 8);
    assert(map.tileCount() == 0);

    std::cout << "  PASS: map creation" << std::endl;
}

static void testTileAccess() {
    Map map(100, 100, 8);

    assert(map.getTile(10, 10, 7) == nullptr);

    auto& tile = map.getOrCreateTile(10, 10, 7);
    tile.setGround(100);

    auto* fetched = map.getTile(10, 10, 7);
    assert(fetched != nullptr);
    assert(fetched->getGround() == 100);
    assert(fetched->isWalkable());

    std::cout << "  PASS: tile access" << std::endl;
}

static void testReachabilitySimple() {
    Map map(20, 20, 1);

    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 7);
            tile.setGround(100);
        }
    }

    // Wall off a section
    for (int x = 0; x < 10; ++x) {
        map.getTile(x, 5, 7)->addItem(400);
    }

    std::vector<Position> positions = {
        {0, 0, 7},  // origin
        {5, 3, 7},  // reachable
        {5, 7, 7}   // unreachable (behind wall)
    };

    auto unreachable = map.validateReachability(positions);
    assert(unreachable.size() == 1);
    assert(unreachable[0].x == 5 && unreachable[0].y == 7);

    std::cout << "  PASS: reachability validation" << std::endl;
}

static void testMultiFloor() {
    Map map(20, 20, 16);

    // Floor 7: walkable area with stair down at (5,5)
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 7);
            tile.setGround(100);
        }
    }
    map.getTile(5, 5, 7)->addItem(500); // stair down

    // Floor 8: walkable area
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 8);
            tile.setGround(100);
        }
    }

    std::vector<Position> positions = {
        {0, 0, 7},   // origin (floor 7)
        {5, 3, 7},   // same floor — reachable
        {5, 5, 8}    // floor below via stair — reachable
    };

    auto unreachable = map.validateReachability(positions);
    assert(unreachable.empty());

    std::cout << "  PASS: multi-floor reachability via stairs" << std::endl;
}

static void testFloorChangeUp() {
    Map map(20, 20, 16);

    // Floor 7: walkable
    for (int x = 3; x < 8; ++x) {
        for (int y = 3; y < 8; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 7);
            tile.setGround(100);
        }
    }
    // Stair north at (5,5) floor 7 — goes to (5,4) floor 6
    map.getTile(5, 5, 7)->addItem(501);

    // Floor 6: walkable
    for (int x = 3; x < 8; ++x) {
        for (int y = 3; y < 8; ++y) {
            auto& tile = map.getOrCreateTile(x, y, 6);
            tile.setGround(100);
        }
    }

    std::vector<Position> positions = {
        {3, 3, 7},   // origin
        {5, 4, 6}    // floor above via stair north
    };

    auto unreachable = map.validateReachability(positions);
    assert(unreachable.empty());

    std::cout << "  PASS: floor change up (north)" << std::endl;
}

static void testCApi() {
    wypas_register_item(100, ITEM_GROUP_GROUND, 0, 0, 0);
    wypas_register_item(400, ITEM_GROUP_NONE, 1, 0, 0);

    WypasMap* m = wypas_map_create(20, 20, 1);
    assert(m != nullptr);

    wypas_tile_set_ground(m, 0, 0, 7, 100);
    wypas_tile_set_ground(m, 1, 0, 7, 100);
    wypas_tile_set_ground(m, 2, 0, 7, 100);

    assert(wypas_tile_is_walkable(m, 0, 0, 7) == 1);
    assert(wypas_tile_is_walkable(m, 5, 5, 7) == 0); // no tile

    int dirs[10];
    int count = wypas_find_path(m, 0, 0, 7, 2, 0, 7, 10, dirs, 10);
    assert(count == 2);
    assert(dirs[0] == DIRECTION_EAST);
    assert(dirs[1] == DIRECTION_EAST);

    wypas_map_destroy(m);

    std::cout << "  PASS: C API smoke test" << std::endl;
}

int main() {
    registerTestItems();

    std::cout << "test_map:" << std::endl;
    testMapCreation();
    testTileAccess();
    testReachabilitySimple();
    testMultiFloor();
    testFloorChangeUp();
    testCApi();

    std::cout << "All map tests passed." << std::endl;
    return 0;
}
