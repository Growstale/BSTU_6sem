cmake_minimum_required(VERSION 3.10)
project(Lab-02b C)
add_executable(Lab-02b Lab-02b/Lab-02b.c)

configure_file(Lab-02b/text.txt ${CMAKE_BINARY_DIR}/Debug/text.txt COPYONLY)