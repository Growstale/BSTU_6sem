cmake_minimum_required(VERSION 3.20)

project(Lab-03)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(VAVLib)
add_subdirectory(Lab-03a)
add_subdirectory(LibVAVd)
add_subdirectory(Lab-03c)