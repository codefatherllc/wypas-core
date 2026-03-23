#include "items.hpp"

namespace core {

Items& Items::getInstance() {
    static Items instance;
    return instance;
}

void Items::clear() {
    items_.clear();
}

void Items::ensureSize(uint16_t id) {
    if (id >= items_.size()) {
        items_.resize(id + 1);
    }
}

const ItemType& Items::getItemType(uint16_t id) const {
    if (id < items_.size()) return items_[id];
    return nullType_;
}

ItemType& Items::getItemType(uint16_t id) {
    ensureSize(id);
    return items_[id];
}

bool Items::addItemType(uint16_t id, const ItemType& type) {
    ensureSize(id);
    items_[id] = type;
    items_[id].id = id;
    return true;
}

} // namespace core
