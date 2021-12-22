project (wtools)
cmake_minimum_required(VERSION 2.8)

set(WTOOLS_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(${WTOOLS_BASE_DIR}/cmake/build.cmake)
add_subdirectory(lg)
add_subdirectory(kt)
add_definitions(-DHAS_DICO)
add_subdirectory(bkv)
add_subdirectory(rbt)

