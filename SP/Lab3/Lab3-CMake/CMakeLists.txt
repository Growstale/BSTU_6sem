cmake_minimum_required(VERSION 3.20)

project(Lab-03)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(Lab-03a)
add_subdirectory(VAVLib)
add_subdirectory(VAVDLib)
add_subdirectory(VAVDLib++)
add_subdirectory(Lab-03b-Explicit)
add_subdirectory(Lab-03b-Implicit)