#ifndef WYPAS_CORE_DEFS_HPP
#define WYPAS_CORE_DEFS_HPP

#include <cstdint>

namespace core {

enum ReturnValue {
    RET_NOERROR = 0,
    RET_NOTPOSSIBLE,
    RET_NOTENOUGHROOM,
    RET_TILEISFULL,
    RET_PLAYERISPZLOCKED,
    RET_DESTINATIONOUTOFREACH
};

enum ItemGroup : uint8_t {
    ITEM_GROUP_NONE = 0,
    ITEM_GROUP_GROUND,
    ITEM_GROUP_CONTAINER,
    ITEM_GROUP_WEAPON,
    ITEM_GROUP_AMMUNITION,
    ITEM_GROUP_ARMOR,
    ITEM_GROUP_CHARGES,
    ITEM_GROUP_TELEPORT,
    ITEM_GROUP_MAGICFIELD,
    ITEM_GROUP_WRITEABLE,
    ITEM_GROUP_KEY,
    ITEM_GROUP_SPLASH,
    ITEM_GROUP_FLUID,
    ITEM_GROUP_DOOR,
    ITEM_GROUP_DEPRECATED,
    ITEM_GROUP_LAST
};

enum TileFlag : uint32_t {
    TILESTATE_NONE              = 0,
    TILESTATE_PROTECTIONZONE    = 1 << 0,
    TILESTATE_TRASHED           = 1 << 1,
    TILESTATE_OPTIONALZONE      = 1 << 2,
    TILESTATE_NOLOGOUT          = 1 << 3,
    TILESTATE_HARDCOREZONE      = 1 << 4,
    TILESTATE_REFRESH           = 1 << 5,
    TILESTATE_HOUSE             = 1 << 6,
    TILESTATE_FLOORCHANGE       = 1 << 7,
    TILESTATE_FLOORCHANGE_DOWN  = 1 << 8,
    TILESTATE_FLOORCHANGE_NORTH = 1 << 9,
    TILESTATE_FLOORCHANGE_SOUTH = 1 << 10,
    TILESTATE_FLOORCHANGE_EAST  = 1 << 11,
    TILESTATE_FLOORCHANGE_WEST  = 1 << 12,
    TILESTATE_BLOCKSOLID        = 1 << 23,
    TILESTATE_BLOCKPATH         = 1 << 24
};

enum FloorChange : uint8_t {
    CHANGE_DOWN = 0,
    CHANGE_NORTH = 1,
    CHANGE_EAST = 2,
    CHANGE_SOUTH = 3,
    CHANGE_WEST = 4,
    CHANGE_NONE = 9
};

enum OtbItemFlag : uint32_t {
    FLAG_BLOCK_SOLID       = 1 << 0,
    FLAG_BLOCK_PROJECTILE  = 1 << 1,
    FLAG_BLOCK_PATHFIND    = 1 << 2,
    FLAG_HAS_HEIGHT        = 1 << 3,
    FLAG_USABLE            = 1 << 4,
    FLAG_PICKUPABLE        = 1 << 5,
    FLAG_MOVABLE           = 1 << 6,
    FLAG_STACKABLE         = 1 << 7,
    FLAG_FLOORCHANGEDOWN   = 1 << 8,
    FLAG_FLOORCHANGENORTH  = 1 << 9,
    FLAG_FLOORCHANGEEAST   = 1 << 10,
    FLAG_FLOORCHANGESOUTH  = 1 << 11,
    FLAG_FLOORCHANGEWEST   = 1 << 12,
    FLAG_ALWAYSONTOP       = 1 << 13,
    FLAG_READABLE          = 1 << 14,
    FLAG_ROTABLE           = 1 << 15,
    FLAG_HANGABLE          = 1 << 16,
    FLAG_VERTICAL          = 1 << 17,
    FLAG_HORIZONTAL        = 1 << 18,
    FLAG_LOOKTHROUGH       = 1 << 23,
    FLAG_ANIMATION         = 1 << 24,
    FLAG_WALKSTACK         = 1 << 25
};

enum OtbAttribute : uint8_t {
    ITEM_ATTR_FIRST     = 0x10,
    ITEM_ATTR_SERVERID  = 0x10,
    ITEM_ATTR_CLIENTID  = 0x11,
    ITEM_ATTR_NAME      = 0x12,
    ITEM_ATTR_SPEED     = 0x14,
    ITEM_ATTR_LIGHT2    = 0x2A,
    ITEM_ATTR_TOPORDER  = 0x2B,
    ITEM_ATTR_WAREID    = 0x2C
};

inline constexpr int32_t MAP_NORMALWALKCOST = 10;
inline constexpr int32_t MAP_DIAGONALWALKCOST = 25;
inline constexpr int32_t MAP_MAX_LAYERS = 16;
inline constexpr int32_t MAX_NODES = 512;

} // namespace core

#endif
