#ifndef WYPAS_CORE_API_H
#define WYPAS_CORE_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WypasMap WypasMap;

WypasMap* wypas_map_create(int width, int height, int floors);
void      wypas_map_destroy(WypasMap* m);

int wypas_load_items(const char* otb_path);
void wypas_items_clear(void);

int wypas_tile_set_ground(WypasMap* m, int x, int y, int z, uint16_t item_id);
int wypas_tile_add_item(WypasMap* m, int x, int y, int z, uint16_t item_id);
int wypas_tile_is_walkable(WypasMap* m, int x, int y, int z);
int wypas_tile_get_item_count(WypasMap* m, int x, int y, int z);

int wypas_find_path(WypasMap* m,
                    int x1, int y1, int z1,
                    int x2, int y2, int z2,
                    int max_dist,
                    int* dirs_out, int max_dirs);

int wypas_validate_map(WypasMap* m, int* unreachable_count);

// Item type registration (for use without OTB file)
int wypas_register_item(uint16_t id, uint8_t group, int block_solid, int always_on_top, int top_order);

#ifdef __cplusplus
}
#endif

#endif
