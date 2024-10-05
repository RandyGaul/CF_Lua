cmake_minimum_required(VERSION 3.5)
project(CF_Lua)

set(CMAKE_CXX_STANDARD 20)

# Make sure all binaries are placed into the same build folder.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
include(ExternalProject)

include(cute.cmake)
include(box2d.cmake)
include(luajit.cmake)

# Add source for the game.
add_executable(
	CF_Lua
	src/bind.h
	src/main.cpp
)

target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/shaders>)

target_link_libraries(CF_Lua cute box2d luajit)

# For convenience set MSVC debugger's working directory in the build folder.
# Also ask MSVC to make CF_Lua the startup project.
if (MSVC)
	set_property(TARGET CF_Lua PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:CF_Lua>)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CF_Lua)
endif()
