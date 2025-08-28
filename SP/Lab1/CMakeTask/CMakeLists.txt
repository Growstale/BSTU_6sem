cmake_minimum_required(VERSION 3.30)

project(Lab1)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(Lab-01C)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CURRENT_OS "Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(CURRENT_OS "macOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(CURRENT_OS "Windows")
else()
	set(CURRENT_OS "Unknown")
endif()

message(STATUS "Hello, I am ${CURRENT_OS}!")

if(NOT CURRENT_OS STREQUAL "Linux" AND NOT CURRENT_OS STREQUAL "macOS")
	add_subdirectory(Lab-01CS)
else()
	message(STATUS "No C# support on Linux/macOS")
endif()
