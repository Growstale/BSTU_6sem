cmake_minimum_required(VERSION 3.20)

include_directories(${CMAKE_SOURCE_DIR}/VAVDLib/include)
add_executable(Lab-03b-Implicit Lab-03b-Implicit.c)
target_link_libraries(Lab-03b-Implicit VAVDLib)