cmake_minimum_required(VERSION 3.10)
project(Lab-02a C)
add_executable(Lab-02a Lab-02a/Lab-02a.c)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

configure_file(Lab-02a/file.txt ${CMAKE_BINARY_DIR}/Debug/file.txt COPYONLY)