# wypas-core

Shared C++17 library providing tile management, item loading, A* pathfinding, and map validation. Exposes a C API for FFI consumption (CGo, ctypes, etc.).

## Features

- **Tile stacking** — ground, down items (by top order), and top items with automatic flag management
- **OTB item loader** — parses `.otb` files for item properties (block solid, floor change, always on top, etc.)
- **A* pathfinding** — fixed 512-node pool, diagonal support, configurable walk cost callback
- **Multi-floor maps** — 3D grid (x, y, z) with staircase traversal via floor change item flags
- **Map validation** — reachability analysis across walkable tiles
- **C API** — flat `extern "C"` interface in `wypas_core.hpp` for cross-language integration

## Prerequisites

- CMake 3.16+
- C++17 compiler (GCC 8+, Clang 7+, MSVC 2019+)

## Build

```bash
cmake -B build
cmake --build build
ctest --test-dir build
```

Produces a static library (`libwypas-core.a` / `wypas-core.lib`).

## C API Reference

All functions are declared in `wypas_core.hpp`.

### Map Lifecycle

| Function | Description |
|----------|-------------|
| `wypas_map_create(width, height, floors)` | Create a new map grid, returns `WypasMap*` |
| `wypas_map_destroy(m)` | Free all map resources |

### Item Registry

| Function | Description |
|----------|-------------|
| `wypas_load_items(otb_path)` | Load item properties from an OTB file. Returns 0 on success |
| `wypas_register_item(id, group, block_solid, always_on_top, top_order)` | Register a single item type manually (without OTB file) |
| `wypas_items_clear()` | Reset the item registry |

### Tile Operations

| Function | Description |
|----------|-------------|
| `wypas_tile_set_ground(m, x, y, z, item_id)` | Set the ground item on a tile |
| `wypas_tile_add_item(m, x, y, z, item_id)` | Add an item to a tile (respects stacking order) |
| `wypas_tile_is_walkable(m, x, y, z)` | Check if a tile can be traversed. Returns 1 if walkable |
| `wypas_tile_get_item_count(m, x, y, z)` | Count all items on a tile (ground + top + down) |

### Pathfinding & Validation

| Function | Description |
|----------|-------------|
| `wypas_find_path(m, x1, y1, z1, x2, y2, z2, max_dist, dirs_out, max_dirs)` | A* pathfinding between two positions. Writes direction sequence to `dirs_out`, returns step count or -1 on failure |
| `wypas_validate_map(m, unreachable_count)` | Validate map reachability. Writes count of unreachable tiles to `unreachable_count` |

## Usage from Go (CGo)

```go
// #cgo LDFLAGS: -lwypas-core -lstdc++
// #include "wypas_core.hpp"
import "C"
import "unsafe"

func main() {
    otbPath := C.CString("items.otb")
    defer C.free(unsafe.Pointer(otbPath))
    C.wypas_load_items(otbPath)

    m := C.wypas_map_create(100, 100, 1)
    defer C.wypas_map_destroy(m)

    C.wypas_tile_set_ground(m, 50, 50, 0, 100)
    C.wypas_tile_add_item(m, 50, 50, 0, 200)

    walkable := C.wypas_tile_is_walkable(m, 50, 50, 0)
    _ = walkable
}
```

## Directory Structure

```
wypas-core/
  include/          Public headers
    astar.hpp         A* pathfinding (nodes, findPath)
    defs.hpp          Enums, constants (ItemGroup, TileFlag, FloorChange)
    item_type.hpp     ItemType struct (properties, flags)
    items.hpp         Item registry singleton (OTB loader)
    map.hpp           Map class (3D tile grid, pathfinding, validation)
    position.hpp      Position struct, Direction enum
    tile.hpp          Tile class (ground, stacking, walkability)
  src/              Implementations
  tests/            Unit tests (tile, astar, map)
  wypas_core.hpp    C API header
  wypas_core.cpp    C API implementation
  CMakeLists.txt    Build configuration
```

## License

GNU General Public License v2.0 — see [LICENSE](LICENSE).
