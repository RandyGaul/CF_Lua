#include <bind.h>

lua_State* L;

#include <wrap_cf.cpp>
#include <wrap_box2d.cpp>

void dump_lua_api()
{
	printf("Constants:\n");
	for (const REF_Constant* c = REF_Constant::head(); c; c = c->next) {
		printf("\t%s\n", c->name);
	}
	printf("\nGlobal Variables:\n");
	for (const REF_Global* g = REF_Global::head(); g; g = g->next) {
		printf("\t%s\n", g->name);
	}
	printf("\nFunctions:\n");
	for (const REF_Function* fn = REF_Function::head(); fn; fn = fn->next) {
		printf("\t%s(", fn->name());
		const REF_FunctionSignature& sig = fn->sig();
		for (int i = 0; i < sig.param_count; i++) {
			const REF_Type* type = sig.params[i];
			int flattened_count = type->flattened_count();

			for (int j = 0; j < flattened_count; ++j) {
				printf("%s%s", (i == 0 && j == 0) ? "" : ", ", type->flattened_type()->name());
			}
		}
		printf(")\n");
	}
	for (const REF_WrapBinder* w = REF_WrapBinder::head(); w; w = w->next) {
		printf("\t%s\n", w->name);
	}
}
REF_FUNCTION(dump_lua_api);

// -------------------------------------------------------------------------------------------------
// Main

bool run_from_msvc_for_testing = false;

void mount_directory_as(const char* to_mount, const char* dir)
{
	Path path = fs_get_base_directory();
	path.normalize();
	if (run_from_msvc_for_testing) {
		path.pop(2); // Pop out of build/debug/
	}
	path += to_mount;
	fs_mount(path.c_str(), dir);
}

REF_FUNCTION(mount_directory_as);

void assert_traceback(const char* string)
{
	luaL_traceback(L, L, string, 1);
	const char* error_and_stack_trace = lua_tostring(L, -1);
	String s = error_and_stack_trace;
	lua_pop(L, 1);
	REF_CallLuaFunction(L, "REF_ErrorHandler", { }, s.c_str());
}

int b2_assert_override(const char* condition, const char* fileName, int lineNumber)
{
	String s = String::fmt("BOX2D ASSERTION: %s, %s, line %d\n", condition, fileName, lineNumber);
	assert_traceback(s.c_str());
	return 1;
}

void cf_assert_override(bool expr, const char* message, const char* file, int line)
{
	if (!expr) {
		String s = String::fmt("CF_ASSERT(%s) : %s, line %d\n", message, file, line);
		assert_traceback(s.c_str());
	}
}

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void wait_for_debugger()
{
	while (!IsDebuggerPresent()) {
		printf("Waiting for debugger to attach...\n");
		Sleep(500);
	}
}
REF_FUNCTION(wait_for_debugger);

#include <direct.h> // chdir

int main(int argc, char* argv[])
{
	b2SetAssertFcn(b2_assert_override);
	cf_set_assert_handler(cf_assert_override);

	::L = luaL_newstate();
	luaL_openlibs(L);
	REF_BindLua(L);

	if (argc < 2) {
		printf("You should supply the path to your `main.lua` file as the first command line parameter.\n");
		printf("Now assuming you've run from MSVC's Debug/Release folder for testing CF_Lua development.\n");
		run_from_msvc_for_testing = true;
	}

	const char* path_to_main_lua = argc < 2 ? "../../src/main.lua" : argv[1];

	const char* main_lua_dir = sppop(path_to_main_lua);
	const char* main_lua_filename = spfname(path_to_main_lua);
	chdir(main_lua_dir);
	if (luaL_dofile(L, main_lua_filename)) {
		fprintf(stderr, lua_tostring(L, -1));
		return -1;
	}
	sfree(main_lua_filename);
	sfree(main_lua_dir);

	REF_CallLuaFunction(L, "main");
	lua_close(L);

	return 0;
}
