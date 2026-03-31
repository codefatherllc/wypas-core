# wypas-core

Shared C++17 library providing tile management, item loading, A* pathfinding, and map validation. Exposes a C API for FFI consumption (CGo, ctypes, etc.).

## Features

- **Tile stacking** — ground, down items (by top order), and top items with automatic flag management
- **Item types via C API** — populated externally (block solid, floor change, always on top, etc.)
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
| `wypas_set_item_type(id, flags, speed, top_order)` | Register a single item type via C API |
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

func main() {
    // Push item types from Go-side DB query
    C.wypas_set_item_type(100, 0x01, 200, 0) // id=100, flags=blockSolid, speed=200, topOrder=0
    C.wypas_set_item_type(200, 0x00, 0, 1)   // id=200, no flags, topOrder=1

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
    items.hpp         Item registry singleton (C API populated)
    map.hpp           Map class (3D tile grid, pathfinding, validation)
    position.hpp      Position struct, Direction enum
    tile.hpp          Tile class (ground, stacking, walkability)
  src/              Implementations
  tests/            Unit tests (tile, astar, map)
  wypas_core.hpp    C API header
  wypas_core.cpp    C API implementation
  CMakeLists.txt    Build configuration
```

## Releases

Pre-built static libraries are published as GitHub Releases on tag push.

Available platforms:
- `wypas-core-linux-amd64.tar.gz`
- `wypas-core-linux-arm64.tar.gz`
- `wypas-core-macos-arm64.tar.gz`

Each archive contains `lib/libwypas-core.a`, `include/*.hpp`, and `wypas_core.hpp`.

## CI

`.github/workflows/build.yml` runs on push to main, PRs, and tag push (`v*`).

On tags: builds all three platforms, packages archives, creates GitHub Release.

## License

GNU General Public License v2.0 — see [LICENSE](LICENSE).
