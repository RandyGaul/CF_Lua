#include <bind.h>

lua_State* L;

#include <wrap_cf.cpp>
#include <wrap_box2d.cpp>

// -------------------------------------------------------------------------------------------------
// Manually bind shaders.

#include "../shaders/flash_shader.h"

// Keeps track of all available custom shaders in a global table. These can get fetched by Lua.
// This requires us to compile shaders into the executable in C++.
Map<const char*, CF_Shader> g_shaders;

#define ADD_SHADER(S) \
	g_shaders.add(sintern(#S "_shader"), CF_MAKE_SOKOL_SHADER(S##_shader))

void LoadShaders()
{
	// Add more shaders here as necessary. Don't forget to include it's associated header.
	ADD_SHADER(flash);
}

int wrap_make_shader(lua_State* L)
{
	char* name_ptr;
	REF_GetType<char*>()->lua_get(L, -1, &name_ptr);
	lua_pop(L, 1);
	String name = String(name_ptr) + "_shader";
	CF_Shader shd = g_shaders.find(sintern(name));
	assert(shd.id);
	REF_LuaSet(L, &shd);
	return 1;
}
REF_WRAP_MANUAL(wrap_make_shader);

// -------------------------------------------------------------------------------------------------
// Main

int main(int argc, char* argv[])
{
	make_app("CF in Lua", 0, 0, 640, 480, APP_OPTIONS_WINDOW_POS_CENTERED | APP_OPTIONS_NO_AUDIO, argv[0]);
	::L = luaL_newstate();
	luaL_openlibs(L);
	REF_BindLua(L);
	LoadShaders();

	if (luaL_dofile(L, "../../src/main.lua")) {
		fprintf(stderr, lua_tostring(L, -1));
		return -1;
	}
	REF_CallLuaFunction(L, "main");
	lua_close(L);

	return 0;
}
