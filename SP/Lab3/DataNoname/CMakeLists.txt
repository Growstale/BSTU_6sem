cmake_minimum_required(VERSION 3.20)

project(Lab-03)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(Main)
add_subdirectory(MyDll)