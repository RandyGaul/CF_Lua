FetchContent_Declare(
	box2d
	GIT_REPOSITORY https://github.com/erincatto/box2d
	GIT_TAG b864f533c3d851d5c7b55fd6fb4bac00466ff854
)
FetchContent_MakeAvailable(box2d)
set_target_properties(box2d PROPERTIES CMAKE_COMPILE_WARNING_AS_ERROR OFF)