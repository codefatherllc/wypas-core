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
- OTB loader: parses items.otb for item properties (blockSolid, alwaysOnTop, etc.)

## C API
See wypas_core.hpp — consumed by wypas-brain via CGo.

## CI
.github/workflows/build.yml on main — builds + runs tests
