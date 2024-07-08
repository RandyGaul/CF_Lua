#include <cute.h>
using namespace Cute;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

template <typename T>
struct REF_List
{
	REF_List()
	{
		m_next = head();
		head() = (T*)this;
	}

	static T*& head() { static T* p = 0; return p; }

	T* next;
};

struct REF_Type;

struct REF_Variant
{
	REF_Type* type;
	void* data;
};

struct REF_Type
{
	virtual const char* name() const = 0;
	virtual int size() const = 0;
	virtual void to_lua(lua_State *L, REF_Variant v) const = 0;
};

#define REF_REGISTER_TYPE(T) \
	struct T##_Type : public REF_Type \
	{ \
		virtual const char* name() const { return #T; } \
		virtual int size() const { return sizeof(T); } \
	} g_##T##_Type; \
	const REF_Type& REF_GetType(T) { return g_##T##_Type; }

struct REF_FunctionSignature
{
};

struct REF_Function
{
	const char* name;
	const REF_Type* return_type;
	const REF_Type* param(int index) const { assert(index < 8); return param_types[index]; }
	const int arg_count;

	void apply() const;

private:
	const REF_Type* param_types[8];
};

REF_REGISTER_TYPE(int);

int main(int argc, char* argv[])
{
	make_app("Fancy Window Title", 0, 0, 640, 480, APP_OPTIONS_WINDOW_POS_CENTERED, argv[0]);
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	printf("%s\n", REF_GetType(1).name());
	printf("%d\n", REF_GetType(1).size());

	while (app_is_running()) {
		app_update();

		app_draw_onto_screen();
	}

	destroy_app();

	return 0;
}
