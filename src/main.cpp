#include <cute.h>
using namespace Cute;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// alloca
#ifdef _WIN32
	#include <malloc.h>
#else
	#include <alloca.h>
#endif

#include <utility>

// An abstract representation of types in C++, used to write generic routines
// for binding things to Lua.
struct REF_Type
{
	virtual const char* name() const = 0;
	virtual int size() const = 0;
	virtual double to_number(void* v) const = 0;
	virtual String to_string(void* v) const = 0;
	virtual void cast(void* to, void* from, const REF_Type* from_type) const = 0;
	virtual void cleanup(void* v) const = 0;
	virtual int lua_set(lua_State* L, void* v) const = 0;
	virtual int lua_get(lua_State* L, int index, void* v) const = 0;
};

// Display a helpful message if a function or constant was attempted to be bound
// to Lua, but had some type not yet represented by the reflection.
template<typename T, typename Enable = void>
struct REF_TypeGetter
{
	static const REF_Type* get()
	{
		static_assert(sizeof(T) == -1, "Type not registered with reflection.");
		return NULL;
	}
};

// Used to consolidate all pointer types (except char*) down to a common void*.
// This lets algorithms bind and copy around pointers by value freely, without requiring
// explicit binds for various pointer types.
template<typename T>
struct REF_TypeGetter<T, typename std::enable_if<std::is_pointer<T>::value && !std::is_same<T, char*>::value && !std::is_same<T, const char*>::value>::type>
{
	static const REF_Type* get()
	{
		static const REF_Type* type = &g_void_ptr_Type;
		return type;
	}
};

// Helper to fetch type info by template parameter.
template<typename T>
const REF_Type* REF_GetType()
{
	return REF_TypeGetter<T>::get();
}

// No-ops representing void, mainly for functions that return nothing.
struct void_Type : public REF_Type
{
	virtual const char* name() const { return "void"; }
	virtual int size() const { return 0; }
	virtual double to_number(void* v) const { return 0; }
	virtual String to_string(void* v) const { return String(); }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const { }
	virtual void cleanup(void* v) const { }
	virtual int lua_set(lua_State* L, void* v) const { return 0; }
	virtual int lua_get(lua_State* L, int index, void* v) const { return 0; }
} g_void_Type;
template <> struct REF_TypeGetter<void> { static const REF_Type* get() { return &g_void_Type; } };

// Helper to reduce repetitive binds of C numeric types.
#define REF_NUMERIC_TYPE(T) \
	struct T##_Type : public REF_Type \
	{ \
		virtual const char* name() const { return #T; } \
		virtual int size() const { return sizeof(T); } \
		virtual double to_number(void* v) const { return (double)*(T*)v; } \
		virtual String to_string(void* v) const { return *(T*)v; } \
		virtual void cast(void* to, void* from, const REF_Type* from_type) const { *(T*)to = (T)from_type->to_number(from); } \
		virtual void cleanup(void* v) const { } \
		virtual int lua_set(lua_State* L, void* v) const { lua_pushnumber(L, (double)*(T*)v); return 1; } \
		virtual int lua_get(lua_State* L, int index, void* v) const { *(T*)v = (T)lua_tonumber(L, index); return 1; } \
	} g_##T##_Type; \
	template <> struct REF_TypeGetter<T> { static const REF_Type* get() { return &g_##T##_Type; } }

REF_NUMERIC_TYPE(char);
REF_NUMERIC_TYPE(int);
REF_NUMERIC_TYPE(float);
REF_NUMERIC_TYPE(double);
REF_NUMERIC_TYPE(int64_t);
REF_NUMERIC_TYPE(uint64_t);

// Bind some specific types explicitly, as they don't fit nicely into the numeric macro REF_NUMERIC_TYPE.

struct void_ptr_Type : public REF_Type
{
	virtual const char* name() const { return "void*"; }
	virtual int size() const { return sizeof(void*); }
	virtual double to_number(void* v) const { return (double)(uintptr_t)*(void**)v; }
	virtual String to_string(void* v) const { return String::from_hex((uintptr_t)*(void**)v); }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const { assert(from_type->size() == sizeof(void*)); *(void**)to = *(void**)from; }
	virtual void cleanup(void* v) const { }
	virtual int lua_set(lua_State* L, void* v) const { lua_pushlightuserdata(L, *(void**)v); return 1; }
	virtual int lua_get(lua_State* L, int index, void* v) const { *(void**)v = lua_touserdata(L, index); return 1; }
} g_void_ptr_Type;
template <> struct REF_TypeGetter<void*> { static const REF_Type* get() { return &g_void_ptr_Type; } };

struct bool_Type : public REF_Type
{
	virtual const char* name() const { return "bool"; }
	virtual int size() const { return sizeof(bool); }
	virtual double to_number(void* v) const { return *(bool*)v ? 1.0 : 0.0; }
	virtual String to_string(void* v) const { return *(bool*)v ? "true" : "false"; }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const { *(bool*)to = (from_type->to_number(from) == 0.0 ? false : true); }
	virtual void cleanup(void* v) const { }
	virtual int lua_set(lua_State* L, void* v) const { lua_pushboolean(L, *(bool*)v); return 1; }
	virtual int lua_get(lua_State* L, int index, void* v) const { *(bool*)v = lua_toboolean(L, index); return 1; }
} g_bool_Type;
template <> struct REF_TypeGetter<bool> { static const REF_Type* get() { return &g_bool_Type; } };

struct char_ptr_Type : public REF_Type
{
	virtual const char* name() const { return "char*"; }
	virtual int size() const { return sizeof(char*); }
	virtual double to_number(void* v) const { return stodouble(*(char**)v); }
	virtual String to_string(void* v) const { return *(char**)v; }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const { String s = from_type->to_string(from); *(char**)to = s.steal(); }
	virtual void cleanup(void* v) const { sfree(*(char**)v); }
	virtual int lua_set(lua_State* L, void* v) const { lua_pushstring(L, *(char**)v); return 1; }
	virtual int lua_get(lua_State* L, int index, void* v) const { String s = lua_tostring(L, index); *(char**)v = s.steal(); return 1; }
} g_char_ptr_Type;
template <> struct REF_TypeGetter<char*> { static const REF_Type* get() { return &g_char_ptr_Type; } };
template <> struct REF_TypeGetter<const char*> { static const REF_Type* get() { return &g_char_ptr_Type; } };

struct String_Type : public REF_Type
{
	virtual const char* name() const { return "String"; }
	virtual int size() const { return sizeof(String); }
	virtual double to_number(void* v) const { return stodouble(*(String*)v); }
	virtual String to_string(void* v) const { return *(String*)v; }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const{ *(String*)to = from_type->to_string(from); }
	virtual void cleanup(void* v) const { }
	virtual int lua_set(lua_State* L, void* v) const { lua_pushstring(L, ((String*)v)->c_str()); return 1; }
	virtual int lua_get(lua_State* L, int index, void* v) const { *(String*)v = String(lua_tostring(L, index)); return 1; }
} g_String_Type;
template <> struct REF_TypeGetter<String> { static const REF_Type* get() { return &g_String_Type; } };

struct v2_Type : public REF_Type
{
	virtual const char* name() const { return "v2"; }
	virtual int size() const { return sizeof(v2); }
	virtual double to_number(void* v) const { return 0; }
	virtual String to_string(void* v) const { return String(); }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const { assert(from_type == REF_GetType<v2>()); *(v2*)to = *(v2*)from; }
	virtual void cleanup(void* v) const { }
	virtual int lua_set(lua_State* L, void* v) const { lua_pushnumber(L, ((v2*)v)->x); lua_pushnumber(L, ((v2*)v)->y); return 2; }
	virtual int lua_get(lua_State* L, int index, void* v) const { ((v2*)v)->x = (float)lua_tonumber(L, index); ((v2*)v)->y = (float)lua_tonumber(L, index + 1); return 2; }
} g_v2_Type;
template <> struct REF_TypeGetter<v2> { static const REF_Type* get() { return &g_v2_Type; } };

// An abstract representation of a typed pointer, useful for implementing generic utilities
// for calling and binding functions/constants to Lua.
struct REF_Variable
{
	template <typename T>
	REF_Variable(const T& t) { v = (void*)&t; type = REF_GetType<T>(); }
	REF_Variable() {}
	void* v = NULL;
	const REF_Type* type = REF_GetType<void>();
};

// Syntactic sugar helper.
template <typename T>
T Cast(REF_Variable v)
{
	T to;
	REF_GetType<T>()->cast(&to, v.v, v.type);
	return to;
}

// Stores arrays of type information to fully describe the signature of C-style function.
struct REF_FunctionSignature
{
	template <typename R, typename... Params>
	REF_FunctionSignature(R (*)(Params...))
	{
		if constexpr (sizeof...(Params) > 0) {
			static const REF_Type* param_types[] = { REF_GetType<Params>()... };
			param_count = sizeof...(Params);
			this->params = param_types;
		} else {
			this->params = nullptr;
		}
		this->params = params;
		return_type = REF_GetType<R>();
	}

	int param_count = 0;
	const REF_Type** params = NULL;
	const REF_Type* return_type = NULL;
};

// Unpacks the variadic parameters as an array.
template <typename R, typename... Params, std::size_t... I>
R ApplyHelper(R (*fn)(Params...), REF_Variable* params, std::index_sequence<I...>)
{
	return fn(Cast<Params>(params[I])...);
}

// Calls a function in a generic way using typed pointers.
template <typename R, typename... Params>
void Apply(R (*fn)(Params...), REF_Variable ret, REF_Variable* params, int param_count)
{
	assert(param_count == sizeof...(Params));
	R r = ApplyHelper(fn, params, std::make_index_sequence<sizeof...(Params)>());
	ret.type->cast(ret.v, &r, REF_GetType<R>());
}

// Special case to hand void return.
template <typename... Params, std::size_t... I>
void ApplyHelper(void (*fn)(Params...), REF_Variable* params, std::index_sequence<I...>)
{
	return fn(Cast<Params>(params[I])...);
}

// Special case to hand void return.
template <typename... Params>
void Apply(void (*fn)(Params...), REF_Variable ret, REF_Variable* params, int param_count)
{
	assert(param_count == sizeof...(Params));
	ApplyHelper(fn, params, std::make_index_sequence<sizeof...(Params)>());
}

// Mixin-style helper class to form a linked list of instances. Used for easily looping
// over functions/constants to bind to Lua.
template <typename T>
struct REF_List
{
	REF_List()
	{
		next = head();
		head() = (T*)this;
	}

	static T*& head() { static T* p = 0; return p; }

	T* next;
};

// Stores type information of the function pointer, useful for implementing
// the functor object REF_Function.
template <typename T>
void ApplyWrapper(void (*fn)(), REF_Variable ret, REF_Variable* params, int param_count)
{
	Apply((T)fn, ret, params, param_count);
}

// A generic functor object, used to easily bind functions to Lua.
struct REF_Function : public REF_List<REF_Function>
{
	template <typename T>
	REF_Function(const char* name, T fn)
		: m_name(name)
		, m_sig(fn)
		, m_fn((void (*)())fn)
		, m_fn_wrapper(ApplyWrapper<T>)
	{
	}

	void apply(REF_Variable ret, REF_Variable* params, int param_count) const
	{
		m_fn_wrapper(m_fn, ret, params, param_count);
	}

	const char* name() const { return m_name; }
	const REF_FunctionSignature& sig() const { return m_sig; }

private:
	const char* m_name;
	REF_FunctionSignature m_sig;
	void (*m_fn)();
	void (*m_fn_wrapper)(void (*)(), REF_Variable, REF_Variable*, int);
};

// The function used to automatically bind functions to Lua, capable of calling C-style functions.
int REF_LuaCFn(lua_State* L)
{
	// Fetch the function pointer itself.
	void* upval = lua_touserdata(L, lua_upvalueindex(1));
	assert(upval);

	const REF_Function* fn = (const REF_Function*)upval;
	const REF_FunctionSignature& sig = fn->sig();

	// Allocate stack space for the return value and paramaters.
	REF_Variable ret;
	ret.type = sig.return_type;
	ret.v = alloca(sig.return_type->size());

	int param_count = sig.param_count;
	REF_Variable* params = (REF_Variable*)alloca(sizeof(REF_Variable) * param_count);
	for (int i = 0; i < param_count; ++i) {
		params[i].type = sig.params[i];
		params[i].v = alloca(params[i].type->size());
	}

	// Fetch each parameter from Lua.
	for (int i = 0, idx = 0; i < param_count; i++) {
		int n = params[i].type->lua_get(L, idx + 1, params[i].v);
		idx += n;
	}

	// Call the actual function.
	fn->apply(ret, params, param_count);

	// Cleanup any temporary storage (curse you, c-style strings).
	for (int i = 0; i < param_count; ++i) {
		params[i].type->cleanup(params[i].v);
	}

	// Pass return value back to Lua.
	if (ret.type->size() > 0) {
		ret.type->lua_set(L, ret.v);
	}

	return ret.type->size() > 0 ? 1 : 0;
}

// Binds all functions registered with REF_BIND_FUNCTION to Lua.
void REF_BindFunctions(lua_State* L)
{
	for (const REF_Function* fn = REF_Function::head(); fn; fn = fn->next) {
		lua_pushlightuserdata(L, (void*)fn);
		lua_pushcclosure(L, REF_LuaCFn, 1);
		lua_setglobal(L, fn->name());
	}
}

// Represents a global constant for binding to Lua.
struct REF_Constant : REF_List<REF_Constant>
{
	template <typename T>
	REF_Constant(const char* name, T t)
		: name(name)
		, constant((uintptr_t)t)
	{
		if (std::is_same<T, char*>::value || std::is_same<T, const char*>::value) {
			type = REF_GetType<char*>();
		} else if (std::is_floating_point<T>::value) {
			type = REF_GetType<double>();
		} else if (std::is_integral<T>::value || std::is_enum<T>::value) {
			type = REF_GetType<uint64_t>();
		} else {
			assert(false);
		}
	}

	const char* name;
	uintptr_t constant;
	const REF_Type* type;
};

// Binds all constants registered with REF_BIND_CONSTANT to Lua.
void REF_BindAllConstants(lua_State* L)
{
	for (const REF_Constant* c = REF_Constant::head(); c; c = c->next) {
		c->type->lua_set(L, (void*)&c->constant);
		lua_setglobal(L, c->name);
	}
}

#define REF_BIND_FUNCTION(F) \
	REF_Function g_##F##_REF_Function(#F, F)

#define REF_BIND_FUNCTION_EX(name, F) \
	REF_Function g_##name##_REF_Function(#name, F)

#define REF_BIND_CONSTANT(C) \
	REF_Constant g_##C##_REF_Constant(#C, C)

// Bind some functions and constants to Lua.

void wrap_draw_quad(v2 min, v2 max, float thickness, float chubbiness)
{
	draw_quad(make_aabb(min, max), thickness, chubbiness);
}

const char* test = "testing a string constant";
REF_BIND_CONSTANT(APP_OPTIONS_D3D11_CONTEXT);
REF_BIND_CONSTANT(test);

REF_BIND_FUNCTION(app_is_running);
REF_BIND_FUNCTION(app_update);
REF_BIND_FUNCTION(app_draw_onto_screen);
REF_BIND_FUNCTION(text_height);
REF_BIND_FUNCTION(text_width);
REF_BIND_FUNCTION(draw_text);
REF_BIND_FUNCTION(destroy_app);
REF_BIND_FUNCTION_EX(draw_quad, wrap_draw_quad);

int main(int argc, char* argv[])
{
	make_app("Fancy Window Title", 0, 0, 640, 480, APP_OPTIONS_WINDOW_POS_CENTERED, argv[0]);
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	REF_BindFunctions(L);
	REF_BindAllConstants(L);

	if (luaL_loadfile(L, "../../src/main.lua") || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "Error loading or executing file: %s\n", lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}
	lua_close(L);

	return 0;
}
