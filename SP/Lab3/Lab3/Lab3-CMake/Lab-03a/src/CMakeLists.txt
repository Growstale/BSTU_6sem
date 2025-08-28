cmake_minimum_required(VERSION 3.20)

include_directories(${PROJECT_SOURCE_DIR}/VAVLib/include)
add_executable(Lab-03a Lab-03a.c)
target_link_libraries(Lab-03a VAVLib)