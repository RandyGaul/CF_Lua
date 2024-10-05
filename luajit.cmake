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
	msvcbuild.bat amalg
")
file(WRITE ${luajit_SOURCE_DIR}/build.bat ${luajit_BUILD_BAT})

ExternalProject_Add(luajit_build
		SOURCE_DIR ${luajit_SOURCE_DIR}
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND ""
		BUILD_COMMAND "${luajit_SOURCE_DIR}/build.bat"
		INSTALL_COMMAND ""
		COMMAND cmake -E copy ${luajit_SOURCE_DIR}/src/lua51.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua51.dll
		COMMAND cmake -E copy ${luajit_SOURCE_DIR}/src/lua51.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lua51.lib
		BUILD_BYPRODUCTS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lua51.lib" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua51.dll")

set(luajit_LIB ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lua51.lib)
set(luajit_DLL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua51.dll)
set(luajit_INCLUDE ${luajit_SOURCE_DIR}/src)

add_library(luajit INTERFACE)
add_dependencies(luajit luajit_build)
target_include_directories(luajit INTERFACE ${luajit_INCLUDE})
target_link_libraries(luajit INTERFACE ${luajit_LIB})