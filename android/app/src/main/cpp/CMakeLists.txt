cmake_minimum_required(VERSION 3.22)
project(inertia_player_android LANGUAGES C CXX)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../../vendor/SDL SDL)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(XMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(XMP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(XMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(XMP_ENABLE_LITE ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../../vendor/libxmp libxmp)

add_library(main SHARED android_player.cpp)
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../../vendor/libxmp/include)
target_link_libraries(main PRIVATE SDL2 SDL2main xmp-lite log android)
