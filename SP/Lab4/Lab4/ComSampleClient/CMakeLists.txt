cmake_minimum_required(VERSION 3.20)

project(ComSampleClient CXX)

include_directories(../ComSampleService ../ComSampleServer)
include_directories(${ComSampleProxy_BINARY_DIR}/Generated/)
add_executable(Client Main.cpp)
add_dependencies(Client ComSampleProxy)

target_sources(Client PRIVATE
    "${CMAKE_BINARY_DIR}/ComSampleProxy/Generated/IComTest_i.c" 
)
