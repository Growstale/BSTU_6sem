cmake_minimum_required(VERSION 3.20)

include_directories(${CMAKE_SOURCE_DIR}/LibVAVd/include)
add_library(LibVAVd SHARED LibVAVd.c) 