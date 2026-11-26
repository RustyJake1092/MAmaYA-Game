cmake_minimum_required(VERSION 3.16)
project(PixelWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib REQUIRED)

add_executable(pixel_world
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/World.cpp
)

target_include_directories(pixel_world PRIVATE src)
target_link_libraries(pixel_world PRIVATE raylib)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pixel_world PRIVATE -Wall -Wextra -Wpedantic)
endif()
