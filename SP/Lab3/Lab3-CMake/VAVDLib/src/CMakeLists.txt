cmake_minimum_required(VERSION 3.20)

include_directories(${CMAKE_SOURCE_DIR}/VAVDLib/include)
add_library(VAVDLib SHARED VAVDLib.c)

target_link_options(VAVDLib PRIVATE "/DEF:${CMAKE_SOURCE_DIR}/VAVDLib/src/VAVDLib.def")
