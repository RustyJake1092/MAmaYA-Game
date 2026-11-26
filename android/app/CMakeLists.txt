cmake_minimum_required(VERSION 3.22.1)
project(PixelWorldAndroid LANGUAGES C CXX)

include(FetchContent)

set(PLATFORM Android CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

get_filename_component(PIXEL_WORLD_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

add_library(pixel_world SHARED
    "${PIXEL_WORLD_ROOT}/src/main.cpp"
    "${PIXEL_WORLD_ROOT}/src/Game.cpp"
    "${PIXEL_WORLD_ROOT}/src/Player.cpp"
    "${PIXEL_WORLD_ROOT}/src/World.cpp"
)

target_include_directories(pixel_world PRIVATE "${PIXEL_WORLD_ROOT}/src")
target_compile_definitions(pixel_world PRIVATE PLATFORM_ANDROID)
target_compile_features(pixel_world PRIVATE cxx_std_17)
target_link_libraries(pixel_world PRIVATE raylib)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(pixel_world PRIVATE -Wall -Wextra -Wpedantic)
endif()
