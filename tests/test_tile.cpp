#include "tile.hpp"
#include "items.hpp"
#include <cassert>
#include <iostream>

using namespace core;

static void registerTestItems() {
    auto& items = Items::getInstance();
    items.clear();

    // ID 100: ground tile
    ItemType ground;
    ground.id = 100;
    ground.group = ITEM_GROUP_GROUND;
    items.addItemType(100, ground);

    // ID 101: another ground tile
    ItemType ground2;
    ground2.id = 101;
    ground2.group = ITEM_GROUP_GROUND;
    items.addItemType(101, ground2);

    // ID 200: border (alwaysOnTop, order 1)
    ItemType border;
    border.id = 200;
    border.alwaysOnTop = true;
    border.alwaysOnTopOrder = 1;
    items.addItemType(200, border);

    // ID 201: border (alwaysOnTop, order 2)
    ItemType border2;
    border2.id = 201;
    border2.alwaysOnTop = true;
    border2.alwaysOnTopOrder = 2;
    items.addItemType(201, border2);

    // ID 202: border (alwaysOnTop, order 1) — same order as 200
    ItemType border3;
    border3.id = 202;
    border3.alwaysOnTop = true;
    border3.alwaysOnTopOrder = 1;
    items.addItemType(202, border3);

    // ID 300: splash (alwaysOnTop)
    ItemType splash;
    splash.id = 300;
    splash.group = ITEM_GROUP_SPLASH;
    splash.alwaysOnTop = true;
    splash.alwaysOnTopOrder = 1;
    items.addItemType(300, splash);

    // ID 301: another splash
    ItemType splash2;
    splash2.id = 301;
    splash2.group = ITEM_GROUP_SPLASH;
    splash2.alwaysOnTop = true;
    splash2.alwaysOnTopOrder = 1;
    items.addItemType(301, splash2);

    // ID 400: wall (blockSolid)
    ItemType wall;
    wall.id = 400;
    wall.blockSolid = true;
    items.addItemType(400, wall);

    // ID 500: normal down item
    ItemType item;
    item.id = 500;
    items.addItemType(500, item);

    // ID 501: another normal item
    ItemType item2;
    item2.id = 501;
    items.addItemType(501, item2);
}

static void testGroundReplacement() {
    Tile tile(100, 100, 7);
    tile.addItem(100);
    assert(tile.getGround() == 100);
    assert(tile.getItemCount() == 1);

    tile.addItem(101);
    assert(tile.getGround() == 101);
    assert(tile.getItemCount() == 1);

    std::cout << "  PASS: ground replacement" << std::endl;
}

static void testTopItemSorting() {
    Tile tile(100, 100, 7);
    tile.addItem(100); // ground
    tile.addItem(201); // order 2
    tile.addItem(200); // order 1

    const auto& top = tile.getTopItems();
    assert(top.size() == 2);
    assert(top[0] == 200); // order 1 first
    assert(top[1] == 201); // order 2 second

    std::cout << "  PASS: top item sorting by alwaysOnTopOrder" << std::endl;
}

static void testSplashReplacement() {
    Tile tile(100, 100, 7);
    tile.addItem(100); // ground
    tile.addItem(300); // splash

    const auto& top1 = tile.getTopItems();
    assert(top1.size() == 1);
    assert(top1[0] == 300);

    tile.addItem(301); // new splash replaces old
    const auto& top2 = tile.getTopItems();
    assert(top2.size() == 1);
    assert(top2[0] == 301);

    std::cout << "  PASS: splash replacement" << std::endl;
}

static void testDownItems() {
    Tile tile(100, 100, 7);
    tile.addItem(100); // ground
    tile.addItem(500); // first down item
    tile.addItem(501); // second down item (should be on top = front)

    const auto& down = tile.getDownItems();
    assert(down.size() == 2);
    assert(down[0] == 501); // newest first
    assert(down[1] == 500);

    std::cout << "  PASS: down items stacking (newest on top)" << std::endl;
}

static void testWalkability() {
    Tile walkable(100, 100, 7);
    walkable.addItem(100); // ground
    assert(walkable.isWalkable());

    Tile blocked(101, 100, 7);
    blocked.addItem(100); // ground
    blocked.addItem(400); // wall
    assert(!blocked.isWalkable());

    Tile noGround(102, 100, 7);
    assert(!noGround.isWalkable());

    std::cout << "  PASS: walkability checks" << std::endl;
}

static void testItemCount() {
    Tile tile(100, 100, 7);
    assert(tile.getItemCount() == 0);

    tile.addItem(100); // ground
    assert(tile.getItemCount() == 1);

    tile.addItem(200); // top item
    assert(tile.getItemCount() == 2);

    tile.addItem(500); // down item
    assert(tile.getItemCount() == 3);

    std::cout << "  PASS: item count" << std::endl;
}

static void testRemoveItem() {
    Tile tile(100, 100, 7);
    tile.addItem(100);
    tile.addItem(500);
    tile.addItem(501);
    assert(tile.getDownItems().size() == 2);

    tile.removeItem(500);
    assert(tile.getDownItems().size() == 1);
    assert(tile.getDownItems()[0] == 501);

    std::cout << "  PASS: remove item" << std::endl;
}

int main() {
    registerTestItems();

    std::cout << "test_tile:" << std::endl;
    testGroundReplacement();
    testTopItemSorting();
    testSplashReplacement();
    testDownItems();
    testWalkability();
    testItemCount();
    testRemoveItem();

    std::cout << "All tile tests passed." << std::endl;
    return 0;
}
