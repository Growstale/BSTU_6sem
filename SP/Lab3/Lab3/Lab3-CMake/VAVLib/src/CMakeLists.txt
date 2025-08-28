cmake_minimum_required(VERSION 3.20)

include_directories(${CMAKE_SOURCE_DIR}/VAVLib/include/)
add_library(VAVLib STATIC VAVLib.c)