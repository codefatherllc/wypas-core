#ifndef WYPAS_CORE_ITEMS_HPP
#define WYPAS_CORE_ITEMS_HPP

#include "item_type.hpp"
#include <cstdint>
#include <vector>

namespace core {

class Items {
public:
    static Items& getInstance();

    void clear();

    const ItemType& getItemType(uint16_t id) const;
    ItemType& getItemType(uint16_t id);

    bool addItemType(uint16_t id, const ItemType& type);

    uint16_t size() const { return static_cast<uint16_t>(items_.size()); }

private:
    Items() = default;
    Items(const Items&) = delete;
    Items& operator=(const Items&) = delete;

    std::vector<ItemType> items_;
    ItemType nullType_;

    void ensureSize(uint16_t id);
};

} // namespace core

#endif
