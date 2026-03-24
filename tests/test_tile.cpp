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

    // ID 401: projectile blocker
    ItemType projBlock;
    projBlock.id = 401;
    projBlock.blockProjectile = true;
    items.addItemType(401, projBlock);

    // ID 402: path blocker
    ItemType pathBlock;
    pathBlock.id = 402;
    pathBlock.blockPathFind = true;
    items.addItemType(402, pathBlock);

    // ID 403: immovable wall (blockSolid + !movable)
    ItemType immovWall;
    immovWall.id = 403;
    immovWall.blockSolid = true;
    immovWall.movable = false;
    items.addItemType(403, immovWall);

    // ID 410: floor change down
    ItemType fcDown;
    fcDown.id = 410;
    fcDown.floorChange[CHANGE_DOWN] = true;
    items.addItemType(410, fcDown);

    // ID 411: floor change north
    ItemType fcNorth;
    fcNorth.id = 411;
    fcNorth.floorChange[CHANGE_NORTH] = true;
    items.addItemType(411, fcNorth);

    // ID 412: floor change south-ex
    ItemType fcSouthEx;
    fcSouthEx.id = 412;
    fcSouthEx.floorChange[CHANGE_SOUTH_EX] = true;
    items.addItemType(412, fcSouthEx);

    // ID 420: teleport
    ItemType tp;
    tp.id = 420;
    tp.type = ITEM_TYPE_TELEPORT;
    items.addItemType(420, tp);

    // ID 421: magic field (blockPathFind)
    ItemType mf;
    mf.id = 421;
    mf.type = ITEM_TYPE_MAGICFIELD;
    mf.blockPathFind = true;
    items.addItemType(421, mf);

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

static void testWalkFlagsComputation() {
    Tile tile(100, 100, 7);
    tile.addItem(100); // ground
    assert(tile.walkFlags() == 0);
    assert(!tile.blocksSolid());
    assert(!tile.blocksProjectile());
    assert(!tile.blocksPathFind());

    tile.addItem(400); // wall (blockSolid)
    assert(tile.blocksSolid());
    assert((tile.walkFlags() & WALK_FLAG_SOLID) != 0);

    tile.addItem(401); // projectile blocker
    assert(tile.blocksProjectile());
    assert((tile.walkFlags() & WALK_FLAG_PROJECTILE) != 0);

    tile.addItem(402); // path blocker
    assert(tile.blocksPathFind());
    assert((tile.walkFlags() & WALK_FLAG_PATHBLOCK) != 0);

    // All three flags set
    uint8_t expected = WALK_FLAG_SOLID | WALK_FLAG_PROJECTILE | WALK_FLAG_PATHBLOCK;
    assert(tile.walkFlags() == expected);

    // Remove wall, solid flag should clear
    tile.removeItem(400);
    assert(!tile.blocksSolid());
    assert(tile.blocksProjectile());
    assert(tile.blocksPathFind());

    std::cout << "  PASS: walk flag computation" << std::endl;
}

static void testFloorChangeDetection() {
    Tile tile(100, 100, 7);
    tile.addItem(100); // ground
    assert(!tile.hasFloorChange());

    tile.addItem(410); // floor change down
    assert(tile.hasFloorChange());
    assert(tile.hasFloorChange(CHANGE_DOWN));
    assert(!tile.hasFloorChange(CHANGE_NORTH));
    assert(tile.hasFlag(TILESTATE_FLOORCHANGE));
    assert(tile.hasFlag(TILESTATE_FLOORCHANGE_DOWN));

    Tile tile2(101, 100, 7);
    tile2.addItem(100);
    tile2.addItem(411); // floor change north
    assert(tile2.hasFloorChange(CHANGE_NORTH));
    assert(!tile2.hasFloorChange(CHANGE_DOWN));

    Tile tile3(102, 100, 7);
    tile3.addItem(100);
    tile3.addItem(412); // floor change south-ex
    assert(tile3.hasFloorChange(CHANGE_SOUTH_EX));
    assert(tile3.hasFlag(TILESTATE_FLOORCHANGE_SOUTH_EX));

    std::cout << "  PASS: floor change detection" << std::endl;
}

static void testBlockingFlagAccumulation() {
    Tile tile(100, 100, 7);
    tile.addItem(100); // ground

    tile.addItem(400); // wall (blockSolid, movable=true by default)
    assert(tile.hasFlag(TILESTATE_BLOCKSOLID));
    assert(!tile.hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID));

    tile.removeItem(400);
    assert(!tile.hasFlag(TILESTATE_BLOCKSOLID));

    tile.addItem(403); // immovable wall (blockSolid + !movable)
    assert(tile.hasFlag(TILESTATE_BLOCKSOLID));
    assert(tile.hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID));

    tile.removeItem(403);
    assert(!tile.hasFlag(TILESTATE_BLOCKSOLID));
    assert(!tile.hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID));

    std::cout << "  PASS: blocking flag accumulation" << std::endl;
}

static void testItemTypeFlags() {
    Tile tile(100, 100, 7);
    tile.addItem(100); // ground

    tile.addItem(420); // teleport
    assert(tile.hasFlag(TILESTATE_TELEPORT));

    tile.addItem(421); // magic field
    assert(tile.hasFlag(TILESTATE_MAGICFIELD));
    assert(tile.hasFlag(TILESTATE_BLOCKPATH));
    assert(tile.hasFlag(TILESTATE_NOFIELDBLOCKPATH) == false);

    tile.removeItem(421);
    assert(!tile.hasFlag(TILESTATE_MAGICFIELD));
    assert(!tile.hasFlag(TILESTATE_BLOCKPATH));

    std::cout << "  PASS: item type flags (teleport, magicfield)" << std::endl;
}

static void testZoneType() {
    Tile tile(100, 100, 7);
    assert(tile.getZone() == ZONE_OPEN);

    tile.setFlag(TILESTATE_PROTECTIONZONE);
    assert(tile.getZone() == ZONE_PROTECTION);

    tile.resetFlag(TILESTATE_PROTECTIONZONE);
    tile.setFlag(TILESTATE_HARDCOREZONE);
    assert(tile.getZone() == ZONE_HARDCORE);

    std::cout << "  PASS: zone type" << std::endl;
}

static void testFloorChangeRemoval() {
    Tile tile(100, 100, 7);
    tile.addItem(100);
    tile.addItem(410); // floor change down
    assert(tile.hasFloorChange());

    tile.removeItem(410);
    assert(!tile.hasFloorChange());
    assert(!tile.hasFlag(TILESTATE_FLOORCHANGE_DOWN));

    std::cout << "  PASS: floor change removal" << std::endl;
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
    testWalkFlagsComputation();
    testFloorChangeDetection();
    testBlockingFlagAccumulation();
    testItemTypeFlags();
    testZoneType();
    testFloorChangeRemoval();

    std::cout << "All tile tests passed." << std::endl;
    return 0;
}
