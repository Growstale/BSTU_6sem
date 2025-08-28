cmake_minimum_required(VERSION 3.10)
project(Lab-02d-shared C)
add_executable(Lab-02d-shared Lab-02d-shared/Lab-02d-shared.c)

configure_file(Lab-02d-shared/text-shared.txt ${CMAKE_BINARY_DIR}/bin/text-shared.txt COPYONLY)