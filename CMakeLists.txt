cmake_minimum_required(VERSION 3.5)
project(CF_Lua)

set(CMAKE_CXX_STANDARD 20)

# Make sure all binaries are placed into the same build folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CUTE_FRAMEWORK_STATIC ON)
set(CF_FRAMEWORK_BUILD_SAMPLES OFF)
set(CF_FRAMEWORK_BUILD_TESTS OFF)

add_subdirectory(cute_framework)

include(FetchContent)
include(ExternalProject)

FetchContent_Declare(
	box2d
	GIT_REPOSITORY https://github.com/erincatto/box2d
	GIT_TAG b864f533c3d851d5c7b55fd6fb4bac00466ff854
)
FetchContent_MakeAvailable(box2d)
set_target_properties(box2d PROPERTIES CMAKE_COMPILE_WARNING_AS_ERROR OFF)

# Define the LuaJIT library
include(vcvarsall.cmake)
FetchContent_Populate(
	luajit
	GIT_REPOSITORY https://github.com/LuaJIT/LuaJIT
	GIT_TAG v2.1
)
set(luajit_BUILD_BAT "
	if not defined DevEnvDir (
		call \"${MSVC_VCVARSALL_BAT}\" ${MSVC_VCVARSALL_BAT_ARG}
	)
	cd \"${luajit_SOURCE_DIR}/src\"
	cl
	msvcbuild.bat
")
file(WRITE ${luajit_SOURCE_DIR}/build.bat ${luajit_BUILD_BAT})

ExternalProject_Add(luajit
		SOURCE_DIR ${luajit_SOURCE_DIR}
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND ""
		BUILD_COMMAND "${luajit_SOURCE_DIR}/build.bat"
		INSTALL_COMMAND cmake -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
		COMMAND cmake -E copy ${luajit_SOURCE_DIR}/src/lua51.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
		COMMAND cmake -E copy ${luajit_SOURCE_DIR}/src/lua51.lib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
		BUILD_BYPRODUCTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua51.lib" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua51.dll")

set_target_properties(luajit PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${luajit_SOURCE_DIR}/src"
	IMPORTED_IMPLIB "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua51.lib"
	IMPORTED_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua51.dll"
)

set(luajit_LIB "${CMAKE_RUNTIME_OUTPUT_DIR}/lua51.lib")
set(luajit_INCLUDE_DIR "${luajit_SOURCE_DIR}/src")

# Add source for the game.
add_executable(
	CF_Lua
	src/bind.h
	src/main.cpp
)

add_dependencies(CF_Lua luajit)

target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(CF_Lua PUBLIC ${luajit_INCLUDE_DIR})
target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/shaders>)

target_link_libraries(CF_Lua cute)
target_link_libraries(CF_Lua lua51)
target_link_libraries(CF_Lua box2d)

# For convenience set MSVC debugger's working directory in the build folder.
# Also ask MSVC to make CF_Lua the startup project.
if (MSVC)
	set_property(TARGET CF_Lua PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:CF_Lua>)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CF_Lua)
endif()
