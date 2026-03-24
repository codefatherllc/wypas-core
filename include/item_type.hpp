#ifndef WYPAS_CORE_ITEM_TYPE_HPP
#define WYPAS_CORE_ITEM_TYPE_HPP

#include "defs.hpp"
#include <cstdint>
#include <string>

namespace core {

struct ItemType {
    uint16_t id = 0;
    uint16_t clientId = 0;
    uint16_t speed = 0;

    ItemGroup group = ITEM_GROUP_NONE;
    ItemTypes_t type = ITEM_TYPE_NONE;
    int32_t alwaysOnTopOrder = 0;

    bool blockSolid = false;
    bool blockProjectile = false;
    bool blockPathFind = false;
    bool hasHeight = false;
    bool alwaysOnTop = false;
    bool isVertical = false;
    bool isHorizontal = false;
    bool isHangable = false;
    bool stackable = false;
    bool pickupable = false;
    bool movable = true;
    bool walkStack = true;
    bool lookThrough = false;
    bool isAnimation = false;
    bool usable = false;
    bool rotable = false;

    bool floorChange[CHANGE_LAST] = {};

    bool isGroundTile() const { return group == ITEM_GROUP_GROUND; }
    bool isSplash() const { return group == ITEM_GROUP_SPLASH; }
    bool isAlwaysOnTop() const { return alwaysOnTop; }
    bool isTeleport() const { return type == ITEM_TYPE_TELEPORT; }
    bool isMagicField() const { return type == ITEM_TYPE_MAGICFIELD; }
    bool isMailbox() const { return type == ITEM_TYPE_MAILBOX; }
    bool isTrashHolder() const { return type == ITEM_TYPE_TRASHHOLDER; }
    bool isBed() const { return type == ITEM_TYPE_BED; }
    bool isDepot() const { return type == ITEM_TYPE_DEPOT; }
    bool isDoor() const { return type == ITEM_TYPE_DOOR; }

    bool hasFloorChange() const {
        for (int i = 0; i < CHANGE_LAST; ++i) {
            if (floorChange[i]) return true;
        }
        return false;
    }

    std::string name;
};

} // namespace core

#endif
