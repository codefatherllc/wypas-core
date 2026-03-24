cmake_minimum_required(VERSION 3.16)
project(wypas-core LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(wypas-core STATIC
    src/items.cpp
    src/tile.cpp
    src/astar.cpp
    src/map.cpp
    wypas_core.cpp
)
target_include_directories(wypas-core PUBLIC include/ ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()

add_executable(test_tile tests/test_tile.cpp)
target_link_libraries(test_tile wypas-core)
add_test(NAME test_tile COMMAND test_tile)

add_executable(test_astar tests/test_astar.cpp)
target_link_libraries(test_astar wypas-core)
add_test(NAME test_astar COMMAND test_astar)

add_executable(test_map tests/test_map.cpp)
target_link_libraries(test_map wypas-core)
add_test(NAME test_map COMMAND test_map)

