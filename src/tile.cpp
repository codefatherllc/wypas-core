#include "tile.hpp"
#include "items.hpp"

namespace core {

void Tile::setGround(uint16_t itemId) {
    if (ground_ != 0) {
        updateTileFlags(ground_, true);
    }
    ground_ = itemId;
    if (itemId != 0) {
        updateTileFlags(itemId, false);
    }
    updateWalkFlags();
}

void Tile::addItem(uint16_t itemId) {
    const auto& it = Items::getInstance().getItemType(itemId);

    if (it.isGroundTile()) {
        setGround(itemId);
        return;
    }

    if (it.isAlwaysOnTop()) {
        if (it.isSplash()) {
            for (auto iter = topItems_.begin(); iter != topItems_.end(); ++iter) {
                const auto& existing = Items::getInstance().getItemType(*iter);
                if (existing.isSplash()) {
                    updateTileFlags(*iter, true);
                    topItems_.erase(iter);
                    break;
                }
            }
        }

        bool inserted = false;
        for (auto iter = topItems_.begin(); iter != topItems_.end(); ++iter) {
            const auto& existing = Items::getInstance().getItemType(*iter);
            if (existing.alwaysOnTopOrder > it.alwaysOnTopOrder) {
                topItems_.insert(iter, itemId);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            topItems_.push_back(itemId);
        }
    } else {
        downItems_.insert(downItems_.begin(), itemId);
    }

    updateTileFlags(itemId, false);
    updateWalkFlags();
}

void Tile::removeItem(uint16_t itemId) {
    for (auto it = topItems_.begin(); it != topItems_.end(); ++it) {
        if (*it == itemId) {
            topItems_.erase(it);
            updateTileFlags(itemId, true);
            updateWalkFlags();
            return;
        }
    }

    for (auto it = downItems_.begin(); it != downItems_.end(); ++it) {
        if (*it == itemId) {
            downItems_.erase(it);
            updateTileFlags(itemId, true);
            updateWalkFlags();
            return;
        }
    }
}

bool Tile::isWalkable() const {
    if (ground_ == 0) return false;
    uint8_t wf = walkFlags_.load(std::memory_order_relaxed);
    return (wf & (WALK_FLAG_SOLID | WALK_FLAG_PATHBLOCK)) == 0;
}

uint32_t Tile::getItemCount() const {
    uint32_t count = (ground_ != 0) ? 1 : 0;
    count += static_cast<uint32_t>(topItems_.size());
    count += static_cast<uint32_t>(downItems_.size());
    return count;
}

bool Tile::hasFloorChange(FloorChange dir) const {
    switch (dir) {
        case CHANGE_DOWN:     return hasFlag(TILESTATE_FLOORCHANGE_DOWN);
        case CHANGE_NORTH:    return hasFlag(TILESTATE_FLOORCHANGE_NORTH);
        case CHANGE_SOUTH:    return hasFlag(TILESTATE_FLOORCHANGE_SOUTH);
        case CHANGE_EAST:     return hasFlag(TILESTATE_FLOORCHANGE_EAST);
        case CHANGE_WEST:     return hasFlag(TILESTATE_FLOORCHANGE_WEST);
        case CHANGE_NORTH_EX: return hasFlag(TILESTATE_FLOORCHANGE_NORTH_EX);
        case CHANGE_SOUTH_EX: return hasFlag(TILESTATE_FLOORCHANGE_SOUTH_EX);
        case CHANGE_EAST_EX:  return hasFlag(TILESTATE_FLOORCHANGE_EAST_EX);
        case CHANGE_WEST_EX:  return hasFlag(TILESTATE_FLOORCHANGE_WEST_EX);
        case CHANGE_NONE:     return hasFlag(TILESTATE_FLOORCHANGE);
        default: break;
    }
    return false;
}

void Tile::updateWalkFlags() {
    uint8_t flags = 0;

    if (ground_ != 0) {
        const auto& type = Items::getInstance().getItemType(ground_);
        if (type.blockSolid) flags |= WALK_FLAG_SOLID;
        if (type.blockPathFind) flags |= WALK_FLAG_PATHBLOCK;
        if (type.blockProjectile) flags |= WALK_FLAG_PROJECTILE;
    }

    for (uint16_t id : topItems_) {
        const auto& type = Items::getInstance().getItemType(id);
        if (type.blockSolid) flags |= WALK_FLAG_SOLID;
        if (type.blockPathFind) flags |= WALK_FLAG_PATHBLOCK;
        if (type.blockProjectile) flags |= WALK_FLAG_PROJECTILE;
    }

    for (uint16_t id : downItems_) {
        const auto& type = Items::getInstance().getItemType(id);
        if (type.blockSolid) flags |= WALK_FLAG_SOLID;
        if (type.blockPathFind) flags |= WALK_FLAG_PATHBLOCK;
        if (type.blockProjectile) flags |= WALK_FLAG_PROJECTILE;
    }

    if (hasFlag(TILESTATE_DIMENSION))
        flags |= WALK_FLAG_DIMENSION;

    walkFlags_.store(flags, std::memory_order_relaxed);
}

void Tile::updateTileFlags(uint16_t itemId, bool remove) {
    const auto& it = Items::getInstance().getItemType(itemId);

    if (!remove) {
        if (it.hasFloorChange()) {
            setFlag(TILESTATE_FLOORCHANGE);
            if (it.floorChange[CHANGE_DOWN]) setFlag(TILESTATE_FLOORCHANGE_DOWN);
            if (it.floorChange[CHANGE_NORTH]) setFlag(TILESTATE_FLOORCHANGE_NORTH);
            if (it.floorChange[CHANGE_SOUTH]) setFlag(TILESTATE_FLOORCHANGE_SOUTH);
            if (it.floorChange[CHANGE_EAST]) setFlag(TILESTATE_FLOORCHANGE_EAST);
            if (it.floorChange[CHANGE_WEST]) setFlag(TILESTATE_FLOORCHANGE_WEST);
            if (it.floorChange[CHANGE_NORTH_EX]) setFlag(TILESTATE_FLOORCHANGE_NORTH_EX);
            if (it.floorChange[CHANGE_SOUTH_EX]) setFlag(TILESTATE_FLOORCHANGE_SOUTH_EX);
            if (it.floorChange[CHANGE_EAST_EX]) setFlag(TILESTATE_FLOORCHANGE_EAST_EX);
            if (it.floorChange[CHANGE_WEST_EX]) setFlag(TILESTATE_FLOORCHANGE_WEST_EX);
        }

        if (it.isTeleport()) setFlag(TILESTATE_TELEPORT);
        if (it.isMagicField()) setFlag(TILESTATE_MAGICFIELD);
        if (it.isMailbox()) setFlag(TILESTATE_MAILBOX);
        if (it.isTrashHolder()) setFlag(TILESTATE_TRASHHOLDER);
        if (it.isBed()) setFlag(TILESTATE_BED);
        if (it.isDepot()) setFlag(TILESTATE_DEPOT);

        if (it.blockSolid) setFlag(TILESTATE_BLOCKSOLID);
        if (it.blockPathFind) setFlag(TILESTATE_BLOCKPATH);

        if (it.blockSolid && !it.movable) setFlag(TILESTATE_IMMOVABLEBLOCKSOLID);
        if (it.blockPathFind && !it.movable) setFlag(TILESTATE_IMMOVABLEBLOCKPATH);
        if (it.blockPathFind && !it.movable && !it.isMagicField()) setFlag(TILESTATE_IMMOVABLENOFIELDBLOCKPATH);
        if (it.blockPathFind && !it.isMagicField()) setFlag(TILESTATE_NOFIELDBLOCKPATH);
    } else {
        // On removal: full recalculation of flags (simple + correct)
        uint32_t preserved = flags_ & (
            TILESTATE_PROTECTIONZONE | TILESTATE_OPTIONALZONE |
            TILESTATE_HARDCOREZONE | TILESTATE_NOLOGOUT |
            TILESTATE_REFRESH | TILESTATE_HOUSE |
            TILESTATE_DYNAMIC_TILE | TILESTATE_DIMENSION
        );
        flags_ = preserved;

        if (ground_ != 0) {
            updateTileFlags(ground_, false);
        }
        for (uint16_t id : topItems_) {
            updateTileFlags(id, false);
        }
        for (uint16_t id : downItems_) {
            updateTileFlags(id, false);
        }
    }
}

} // namespace core
