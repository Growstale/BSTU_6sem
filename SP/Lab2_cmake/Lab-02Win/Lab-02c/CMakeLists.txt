cmake_minimum_required(VERSION 3.30)

project(Lab-02c C)

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
	add_subdirectory(Lab-02c)
	add_subdirectory(Lab-02c2)
else()
	message(STATUS "No C# support on Linux/macOS")
endif()
