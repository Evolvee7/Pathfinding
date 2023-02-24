cmake_minimum_required(VERSION 3.7)

project(Pathfinding)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Pathfinding src/main.cpp)
target_link_libraries(Pathfinding ${SDL2_LIBRARIES})

target_include_directories(Pathfinding PRIVATE include/)