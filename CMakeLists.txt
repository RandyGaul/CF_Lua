cmake_minimum_required(VERSION 3.0)
project(CF_Lua)

# Must have at least C++17.
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	set(EMSCRIPTEN TRUE)
endif()

# Make sure all binaries are placed into the same build folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CUTE_FRAMEWORK_STATIC ON)
set(CF_FRAMEWORK_BUILD_SAMPLES OFF)
set(CF_FRAMEWORK_BUILD_TESTS OFF)

include(FetchContent)
FetchContent_Declare(
	cute
	GIT_REPOSITORY https://github.com/RandyGaul/cute_framework
)
FetchContent_MakeAvailable(cute)

# Define the Lua library
add_library(Lua
	lua/onelua.c
)

# Add source for the game.
add_executable(
	CF_Lua
	src/main.cpp
)

target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lua>)
target_include_directories(CF_Lua PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/shaders>)

if(EMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	target_compile_options(CF_Lua PUBLIC -O1 -fno-rtti -fno-exceptions)
	target_link_options(CF_Lua PRIVATE -o CF_Lua.html --preload-file ${CMAKE_SOURCE_DIR}/content@/content --emrun -s ASYNCIFY=1 -O1)
endif()

target_link_libraries(CF_Lua cute)
target_link_libraries(CF_Lua Lua)

# For convenience set MSVC debugger's working directory in the build folder.
# Also ask MSVC to make CF_Lua the startup project.
if (MSVC)
	set_property(TARGET CF_Lua PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:CF_Lua>)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CF_Lua)
endif()
