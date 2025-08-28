cmake_minimum_required(VERSION 3.20)

include_directories(${CMAKE_SOURCE_DIR}/VAVDLib++/include)
add_library(VAVDLib++ SHARED VAVDLib.cpp)