cmake_minimum_required(VERSION 3.10)
project(Lab-02d C)
add_executable(Lab-02d Lab-02d/Lab-02d.c)

configure_file(Lab-02d/text.txt ${CMAKE_BINARY_DIR}/bin/text.txt COPYONLY)