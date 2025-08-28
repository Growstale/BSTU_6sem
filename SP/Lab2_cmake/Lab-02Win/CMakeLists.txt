cmake_minimum_required(VERSION 3.30)

project(Lab-02WIN C)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CURRENT_OS "Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(CURRENT_OS "macOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(CURRENT_OS "Windows")
else()
	set(CURRENT_OS "Unknown")
endif()

if(NOT CURRENT_OS STREQUAL "Linux" AND NOT CURRENT_OS STREQUAL "macOS")
	add_subdirectory(Lab-02a)
	add_subdirectory(Lab-02b)
	add_subdirectory(Lab-02c)
else()
	message(STATUS "No C# support on Linux/macOS")
endif()


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)