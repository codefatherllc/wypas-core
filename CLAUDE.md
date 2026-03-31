# wypas-core — Shared C++ Tile & Pathfinding Library

## Build
cmake -B build && cmake --build build
ctest --test-dir build

## Structure
include/     — public headers (position, items, tile, astar, map)
src/         — implementations
wypas_core.hpp/cpp — C API for FFI (CGo, etc.)

## Key Concepts
- Tile stacking: ground → down items (by topOrder) → top items (by alwaysOnTopOrder)
- A* pathfinding: 512-node fixed pool, WalkCostFn callback (no creature dependency)
- Multi-floor: 3D grid (x, y, z), staircase traversal via floorchange item flags
- Item types: populated externally via C API (wypas_set_item_type), no file loading

## C API
See wypas_core.hpp — consumed by wypas-brain via CGo.

## CI

`.github/workflows/build.yml`:
- Builds + tests on push to main and PRs
- On tag push (`v*`): builds linux/amd64, linux/arm64, macos/arm64
- Packages `libwypas-core.a` + headers as `wypas-core-{platform}-{arch}.tar.gz`
- Creates GitHub Release with all platform archives

## Releases

v1.0.0 released. Consumed by:
- **wypas-brain** (Go) -- CGo, downloaded via `make deps` from GitHub Releases
- **wypas-server** (C++) -- cmake `add_subdirectory`
