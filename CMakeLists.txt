cmake_minimum_required(VERSION 3.5)
project(CF_Lua)

set(CMAKE_CXX_STANDARD 20)

# Make sure all binaries are placed into the same build folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CUTE_FRAMEWORK_STATIC ON)
set(CF_FRAMEWORK_BUILD_SAMPLES OFF)
set(CF_FRAMEWORK_BUILD_TESTS OFF)

add_subdirectory(cute_framework)
FetchContent_Declare(
	box2d
	GIT_REPOSITORY https://github.com/erincatto/box2d
	GIT_TAG main
)
FetchContent_MakeAvailable(box2d)

set_target_properties(box2d PROPERTIES CMAKE_COMPILE_WARNING_AS_ERROR OFF)

# Define the Lua library
add_library(Lua
	lua/onelua.c
)

# Add source for the game.
add_executable(
	CF_Lua
	src/bind.h
	src/main.cpp
)

target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lua>)
target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/shaders>)

target_link_libraries(CF_Lua cute)
target_link_libraries(CF_Lua Lua)
target_link_libraries(CF_Lua box2d)

# For convenience set MSVC debugger's working directory in the build folder.
# Also ask MSVC to make CF_Lua the startup project.
if (MSVC)
	set_property(TARGET CF_Lua PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:CF_Lua>)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CF_Lua)
endif()
