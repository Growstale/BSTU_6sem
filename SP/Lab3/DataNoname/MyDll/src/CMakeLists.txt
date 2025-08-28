cmake_minimum_required(VERSION 3.20)

add_library(MyDll SHARED MyDll.c)

target_link_options(MyDll PRIVATE "/DEF:${CMAKE_SOURCE_DIR}/MyDll/src/MyDll.def")
