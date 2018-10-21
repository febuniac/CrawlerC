project(crawler)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)

SET(FLAG_SEQUENCIAL "-O2")
SET(FLAG_PARALELO "-O2")

# Point to our own cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

find_package(PkgConfig REQUIRED)
# Find SDL2

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_gfx REQUIRED SDL2_gfx)

# Add global definitions
add_definitions("-Wall")
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_gfx_INCLUDE_DIR})

add_executable(CrawlerSequencial crawler_seq3.cpp)
target_link_libraries(CrawlerSequencial ${SDL2_LIBRARIES} ${SDL2_gfx_LIBRARIES} "-lcurl")
SET_TARGET_PROPERTIES(CrawlerSequencial PROPERTIES COMPILE_FLAGS ${FLAG_SEQUENCIAL} LINK_FLAGS ${FLAG_SEQUENCIAL})

add_executable(CrawlerParalelo curl_example_parallel.cpp)
target_link_libraries(CrawlerParalelo ${SDL2_LIBRARIES} ${SDL2_gfx_LIBRARIES} "-lcurl -lpthread")
SET_TARGET_PROPERTIES(CrawlerParalelo PROPERTIES COMPILE_FLAGS ${FLAG_PARALELO} LINK_FLAGS ${FLAG_PARALELO})