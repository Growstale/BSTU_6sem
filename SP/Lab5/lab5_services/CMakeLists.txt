cmake_minimum_required(VERSION 3.10) 
project(ServiceLab)
set(CMAKE_C_COMPILER clang) 
set(CMAKE_CXX_COMPILER clang++)


if(WIN32) 
	add_executable(DemoService DemoService.c) 
	target_link_libraries(DemoService shlwapi) 
	add_executable(ServiceCtl ServiceCtl.c) 
	set_target_properties(DemoService ServiceCtl PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" ) 
endif()


if(WIN32) 
	set(CMAKE_GENERATOR "Visual Studio 16 2019") 
endif()