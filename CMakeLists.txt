project (wtools)
cmake_minimum_required(VERSION 2.8)

include(cmake/build.cmake)
add_subdirectory(kt)
add_definitions(-DHAS_DICO)
add_subdirectory(bkv)

