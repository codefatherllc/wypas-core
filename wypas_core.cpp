#include "wypas_core.hpp"
#include "include/map.hpp"
#include "include/items.hpp"
#include "include/tile.hpp"
#include "include/astar.hpp"

struct WypasMap {
    core::Map map;
};

extern "C" {

WypasMap* wypas_map_create(int width, int height, int floors) {
    auto* m = new WypasMap();
    m->map = core::Map(
        static_cast<uint16_t>(width),
        static_cast<uint16_t>(height),
        static_cast<uint8_t>(floors)
    );
    return m;
}

void wypas_map_destroy(WypasMap* m) {
    delete m;
}

int wypas_load_items(const char* otb_path) {
    if (!otb_path) return -1;
    return core::Items::getInstance().loadFromOtb(otb_path) ? 0 : -1;
}

void wypas_items_clear(void) {
    core::Items::getInstance().clear();
}

int wypas_tile_set_ground(WypasMap* m, int x, int y, int z, uint16_t item_id) {
    if (!m) return -1;
    auto& tile = m->map.getOrCreateTile(
        static_cast<uint16_t>(x),
        static_cast<uint16_t>(y),
        static_cast<uint8_t>(z)
    );
    tile.setGround(item_id);
    return 0;
}

int wypas_tile_add_item(WypasMap* m, int x, int y, int z, uint16_t item_id) {
    if (!m) return -1;
    auto& tile = m->map.getOrCreateTile(
        static_cast<uint16_t>(x),
        static_cast<uint16_t>(y),
        static_cast<uint8_t>(z)
    );
    tile.addItem(item_id);
    return 0;
}

int wypas_tile_is_walkable(WypasMap* m, int x, int y, int z) {
    if (!m) return 0;
    const auto* tile = m->map.getTile(
        static_cast<uint16_t>(x),
        static_cast<uint16_t>(y),
        static_cast<uint8_t>(z)
    );
    if (!tile) return 0;
    return tile->isWalkable() ? 1 : 0;
}

int wypas_tile_get_item_count(WypasMap* m, int x, int y, int z) {
    if (!m) return 0;
    const auto* tile = m->map.getTile(
        static_cast<uint16_t>(x),
        static_cast<uint16_t>(y),
        static_cast<uint8_t>(z)
    );
    if (!tile) return 0;
    return static_cast<int>(tile->getItemCount());
}

int wypas_find_path(WypasMap* m,
                    int x1, int y1, int z1,
                    int x2, int y2, int z2,
                    int max_dist,
                    int* dirs_out, int max_dirs) {
    if (!m) return -1;

    core::Position start(
        static_cast<uint16_t>(x1),
        static_cast<uint16_t>(y1),
        static_cast<uint8_t>(z1)
    );
    core::Position target(
        static_cast<uint16_t>(x2),
        static_cast<uint16_t>(y2),
        static_cast<uint8_t>(z2)
    );

    std::vector<core::Direction> dirs;
    if (!m->map.findPath(start, target, max_dist, dirs)) return -1;

    int count = static_cast<int>(dirs.size());
    if (dirs_out) {
        int n = std::min(count, max_dirs);
        for (int i = 0; i < n; ++i) {
            dirs_out[i] = static_cast<int>(dirs[i]);
        }
    }
    return count;
}

int wypas_validate_map(WypasMap* m, int* unreachable_count) {
    if (!m) return -1;
    // Collect all tile positions
    // This is a simplified version — in practice you'd pass specific positions
    if (unreachable_count) *unreachable_count = 0;
    return 0;
}

int wypas_register_item(uint16_t id, uint8_t group, int block_solid, int always_on_top, int top_order) {
    core::ItemType type;
    type.id = id;
    type.group = static_cast<core::ItemGroup>(group);
    type.blockSolid = block_solid != 0;
    type.alwaysOnTop = always_on_top != 0;
    type.alwaysOnTopOrder = top_order;
    return core::Items::getInstance().addItemType(id, type) ? 0 : -1;
}

} // extern "C"
