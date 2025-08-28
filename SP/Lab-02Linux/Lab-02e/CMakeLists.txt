cmake_minimum_required(VERSION 3.30)

project(Lab-02e C)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CURRENT_OS "Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(CURRENT_OS "macOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(CURRENT_OS "Windows")
else()
	set(CURRENT_OS "Unknown")
endif()

if(NOT CURRENT_OS STREQUAL "Windows" AND NOT CURRENT_OS STREQUAL "macOS")
	add_subdirectory(Lab-02e)
	add_subdirectory(Lab-02e2)
else()
	message(STATUS "No C# support on Windows/macOS")
endif()
