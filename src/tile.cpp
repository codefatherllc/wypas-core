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
}

void Tile::addItem(uint16_t itemId) {
    const auto& it = Items::getInstance().getItemType(itemId);

    if (it.isGroundTile()) {
        setGround(itemId);
        return;
    }

    if (it.isAlwaysOnTop()) {
        // Remove old splash if this is a splash
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

        // Insert sorted by alwaysOnTopOrder (lower order = front)
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
        // Down items: newest at front (on top visually)
        downItems_.insert(downItems_.begin(), itemId);
    }

    updateTileFlags(itemId, false);
}

void Tile::removeItem(uint16_t itemId) {
    for (auto it = topItems_.begin(); it != topItems_.end(); ++it) {
        if (*it == itemId) {
            updateTileFlags(itemId, true);
            topItems_.erase(it);
            return;
        }
    }

    for (auto it = downItems_.begin(); it != downItems_.end(); ++it) {
        if (*it == itemId) {
            updateTileFlags(itemId, true);
            downItems_.erase(it);
            return;
        }
    }
}

bool Tile::isWalkable() const {
    if (ground_ == 0) return false;

    const auto& groundType = Items::getInstance().getItemType(ground_);
    if (groundType.blockSolid) return false;

    for (uint16_t id : topItems_) {
        const auto& t = Items::getInstance().getItemType(id);
        if (t.blockSolid || t.blockPathFind) return false;
    }

    for (uint16_t id : downItems_) {
        const auto& t = Items::getInstance().getItemType(id);
        if (t.blockSolid || t.blockPathFind) return false;
    }

    return true;
}

uint32_t Tile::getItemCount() const {
    uint32_t count = (ground_ != 0) ? 1 : 0;
    count += static_cast<uint32_t>(topItems_.size());
    count += static_cast<uint32_t>(downItems_.size());
    return count;
}

void Tile::updateTileFlags(uint16_t itemId, bool remove) {
    const auto& it = Items::getInstance().getItemType(itemId);

    if (!remove) {
        if (it.blockSolid) setFlag(TILESTATE_BLOCKSOLID);
        if (it.blockPathFind) setFlag(TILESTATE_BLOCKPATH);

        if (it.hasFloorChange()) {
            setFlag(TILESTATE_FLOORCHANGE);
            if (it.floorChange[CHANGE_DOWN]) setFlag(TILESTATE_FLOORCHANGE_DOWN);
            if (it.floorChange[CHANGE_NORTH]) setFlag(TILESTATE_FLOORCHANGE_NORTH);
            if (it.floorChange[CHANGE_SOUTH]) setFlag(TILESTATE_FLOORCHANGE_SOUTH);
            if (it.floorChange[CHANGE_EAST]) setFlag(TILESTATE_FLOORCHANGE_EAST);
            if (it.floorChange[CHANGE_WEST]) setFlag(TILESTATE_FLOORCHANGE_WEST);
        }
    } else {
        // Simplified: just recalculate all flags
        // A full implementation would track refcounts per flag
        flags_ = 0;
        if (ground_ != 0) {
            updateTileFlags(ground_, false);
        }
        for (uint16_t id : topItems_) {
            const auto& t = Items::getInstance().getItemType(id);
            if (t.blockSolid) setFlag(TILESTATE_BLOCKSOLID);
            if (t.blockPathFind) setFlag(TILESTATE_BLOCKPATH);
            if (t.hasFloorChange()) {
                setFlag(TILESTATE_FLOORCHANGE);
                if (t.floorChange[CHANGE_DOWN]) setFlag(TILESTATE_FLOORCHANGE_DOWN);
                if (t.floorChange[CHANGE_NORTH]) setFlag(TILESTATE_FLOORCHANGE_NORTH);
                if (t.floorChange[CHANGE_SOUTH]) setFlag(TILESTATE_FLOORCHANGE_SOUTH);
                if (t.floorChange[CHANGE_EAST]) setFlag(TILESTATE_FLOORCHANGE_EAST);
                if (t.floorChange[CHANGE_WEST]) setFlag(TILESTATE_FLOORCHANGE_WEST);
            }
        }
        for (uint16_t id : downItems_) {
            const auto& t = Items::getInstance().getItemType(id);
            if (t.blockSolid) setFlag(TILESTATE_BLOCKSOLID);
            if (t.blockPathFind) setFlag(TILESTATE_BLOCKPATH);
            if (t.hasFloorChange()) {
                setFlag(TILESTATE_FLOORCHANGE);
                if (t.floorChange[CHANGE_DOWN]) setFlag(TILESTATE_FLOORCHANGE_DOWN);
                if (t.floorChange[CHANGE_NORTH]) setFlag(TILESTATE_FLOORCHANGE_NORTH);
                if (t.floorChange[CHANGE_SOUTH]) setFlag(TILESTATE_FLOORCHANGE_SOUTH);
                if (t.floorChange[CHANGE_EAST]) setFlag(TILESTATE_FLOORCHANGE_EAST);
                if (t.floorChange[CHANGE_WEST]) setFlag(TILESTATE_FLOORCHANGE_WEST);
            }
        }
    }
}

} // namespace core
