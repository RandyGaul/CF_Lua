// Binds most kinds of C code to Lua using template metaprogramming techniques. The techniques
// were learned from John Edwards - https://www.youtube.com/watch?v=hWDZ3Yy-NMA
// This technique heavily reduces boilerplate, and scales up very well, typically only costing
// one-line per type/function you wish to bind.
// 
// This was written specifically to bind Cute Framework to Lua and relies on CF's string type,
// but you can easily replace this with your own string type, and it will all work agnostically.
// https://github.com/RandyGaul/cute_framework

#pragma once

// -------------------------------------------------------------------------------------------------
// Documentation

// REF stands for reflection. This header works by collecting information about C-types in code, and
// implements some higher level algorithms for automating Lua bindings. It's also quite possible to
// implement a variety of automated extensions such as serialization, or generalized functors, but
// I highly recommend to not go down that rabbit hole. The utility of these sort of automation tools
// lays mostly within decisions on restricting scope down to essentials. This header does *not*
// support non-POD (plain old data) types, and sticks to fundamental types (e.g. ints/floats) and
// structs.
// 
// To get going you must register your types with the reflection by calling a variety of macros. They
// are designed ot be mostly one-liners. Once done, call `REF_BindLua` once from main, and that's it.
// Make sure to call the macros at global scope.
// 
// Happy Lua'ing!

// Call this once from main to bind everything.
void REF_BindLua(lua_State* L);

// You can use this on handles/ids 64-bits in size. This gets the reflection system to treat the type
// as a void* in order to copy the bytes around as necessary.
#define REF_HANDLE_TYPE(H)

// Expose a function to the reflection system. It will be bound to Lua with a matching name.
#define REF_FUNCTION(F)

// Expose a function to the reflection system. It will be bound to Lua with a custom name.
#define REF_FUNCTION_EX(name, F)

// Expose a constant to the reflection system. This is for defines, literals, enums, etc.
#define REF_CONSTANT(C)

// Expose a struct to the reflection system.
// Use it alongside `REF_MEMBER`. Example:
// 
//     REF_STRUCT(MyStruct
//          REF_MEMBER(x),
//          REF_MEMBER(y),
//          REF_MEMBER(data),
//     );
// 
// Just be sure that each member type itself has been exposed to the reflection system with a
// different call to `REF_STRUCT`. You may also 
#define REF_STRUCT(T, ...)

// Expose a member of a struct to the reflection system.
#define REF_MEMBER(T)

// In some more uncommon cases you may need to manually define reflection info for your type
// in order to customize its behavior (mainly to customize `lua_set` and `lua_get`). You can find
// examples of this throughout the header. One good example is to manually flatten vectors. In Lua
// it's not a good idea to store named keys for vector components due to perf costs. Instead, an
// indexed table is superior. We can define reflection information for vectors that automatically
// flattens the vectors for us like so:

// struct v2_Type : public REF_Type
// {
//     virtual const char* name() const { return "v2"; }
//     virtual int size() const { return sizeof(v2); }
//     virtual double to_number(void* v) const { return 0; }
//     virtual String to_string(void* v) const { return String(); }
// 
//     virtual void cast(void* to, void* from, const REF_Type* from_type) const { assert(from_type == REF_GetType<v2>()); *(v2*)to = *(v2*)from; }
//     virtual void cleanup(void* v) const { }
//     virtual void lua_set(lua_State* L, void* v) const { lua_pushnumber(L, ((v2*)v)->x); lua_pushnumber(L, ((v2*)v)->y); }
//     virtual void lua_get(lua_State* L, int index, void* v) const { ((v2*)v)->x = (float)lua_tonumber(L, index); ((v2*)v)->y = (float)lua_tonumber(L, index + 1); }
//     virtual int lua_flatten_count() const { return 2; }
// } g_v2_Type;
// template <> struct REF_TypeGetter<v2> { static const REF_Type* get() { return &g_v2_Type; } };

// Optional helper for manually binding functions that deal with arrays.
template <typename T>
void REF_LuaGetArray(lua_State* L, int index, T** out_ptr, int* out_count);

// Overall your binding .cpp file should look something like this:
// 
// #include <bind.h>
// 
// REF_CONSTANT(KEY_DOWN);
// REF_CONSTANT(KEY_W);
// REF_CONSTANT(KEY_A);
// REF_CONSTANT(KEY_S);
// REF_CONSTANT(KEY_D);
// ...
// 
// REF_FUNCTION(key_down);
// REF_FUNCTION(key_just_pressed);
// REF_FUNCTION(key_just_released);
// REF_FUNCTION(key_repeating);
// ...
// 
// int main(int argc, char* argv[])
// {
//     lua_State* L = luaL_newstate();
//     luaL_openlibs(L);
//     REF_BindLua(L);
// 
//     if (luaL_loadfile(L, "main.lua") || lua_pcall(L, 0, 0, 0)) {
//         fprintf(stderr, lua_tostring(L, -1));
//         lua_close(L);
//         return 1;
//     }
//     lua_close(L);
// 
//     return 0;
// }

// -------------------------------------------------------------------------------------------------
// REF - Reflection implementation.

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
	virtual void lua_set(lua_State* L, void* v) const = 0;
	virtual void lua_get(lua_State* L, int index, void* v) const = 0;
	virtual int lua_flatten_count() const { return 1; }
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

// Treat all enums as int.
template<typename T>
struct REF_TypeGetter<T, typename std::enable_if<std::is_enum<T>::value>::type>
{
	static const REF_Type* get()
	{
		static_assert(sizeof(T) == 4, "Assumed enums are 4-bytes.");
		static const REF_Type* type = &g_int_Type;
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
	virtual void lua_set(lua_State* L, void* v) const { }
	virtual void lua_get(lua_State* L, int index, void* v) const { }
	virtual int lua_flatten_count() const { return 0; }
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
		virtual void lua_set(lua_State* L, void* v) const { lua_pushnumber(L, (double)*(T*)v); } \
		virtual void lua_get(lua_State* L, int index, void* v) const { *(T*)v = (T)lua_tonumber(L, index); } \
	} g_##T##_Type; \
	template <> struct REF_TypeGetter<T> { static const REF_Type* get() { return &g_##T##_Type; } }

REF_NUMERIC_TYPE(char);
REF_NUMERIC_TYPE(short);
REF_NUMERIC_TYPE(int);
REF_NUMERIC_TYPE(float);
REF_NUMERIC_TYPE(double);
REF_NUMERIC_TYPE(uint8_t);
REF_NUMERIC_TYPE(int8_t);
REF_NUMERIC_TYPE(uint16_t);
//REF_NUMERIC_TYPE(int16_t); // Just a typedef of short.
REF_NUMERIC_TYPE(uint32_t);
//REF_NUMERIC_TYPE(int32_t); // Just a typedef of int.
REF_NUMERIC_TYPE(uint64_t);
REF_NUMERIC_TYPE(int64_t);

// Bind some specific types explicitly, as they don't fit nicely into the numeric macro REF_NUMERIC_TYPE.

struct void_ptr_Type : public REF_Type
{
	virtual const char* name() const { return "void*"; }
	virtual int size() const { return sizeof(void*); }
	virtual double to_number(void* v) const { return (double)(uintptr_t)*(void**)v; }
	virtual String to_string(void* v) const { return String::from_hex((uintptr_t)*(void**)v); }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const { assert(from_type->size() == sizeof(void*)); *(void**)to = *(void**)from; }
	virtual void cleanup(void* v) const { }
	virtual void lua_set(lua_State* L, void* v) const { lua_pushlightuserdata(L, *(void**)v); }
	virtual void lua_get(lua_State* L, int index, void* v) const { *(void**)v = lua_touserdata(L, index); }
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
	virtual void lua_set(lua_State* L, void* v) const { lua_pushboolean(L, *(bool*)v); }
	virtual void lua_get(lua_State* L, int index, void* v) const { *(bool*)v = lua_toboolean(L, index); }
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
	virtual void lua_set(lua_State* L, void* v) const { lua_pushstring(L, *(char**)v); }
	virtual void lua_get(lua_State* L, int index, void* v) const { String s = lua_tostring(L, index); *(char**)v = s.steal(); }
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
	virtual void lua_set(lua_State* L, void* v) const { lua_pushstring(L, ((String*)v)->c_str()); }
	virtual void lua_get(lua_State* L, int index, void* v) const { *(String*)v = String(lua_tostring(L, index)); }
} g_String_Type;
template <> struct REF_TypeGetter<String> { static const REF_Type* get() { return &g_String_Type; } };

// Describes a data member of a struct.
// Assumes plain-old-data.
struct REF_Member
{
	const char* name;
	size_t offset;
	const REF_Type* type;
};

// Represents a struct as a collection of REF_Member pointers. This maps to key'd tables in Lua.
struct REF_Struct : public REF_Type
{
	virtual const char* name() const = 0;
	virtual int size() const = 0;
	virtual const REF_Member* members() const = 0;
	virtual int member_count() const = 0;

	virtual double to_number(void* v) const { return 0; }
	virtual String to_string(void* v) const { return String(); }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const override { assert(from_type == this); CF_MEMCPY(to, from, size()); }

	virtual void cleanup(void* v) const
	{
		int count = member_count();
		const REF_Member* mptr = members();
		for (int i = 0; i < count; ++i) {
			const REF_Member* m = mptr + i;
			m->type->cleanup((char*)v + m->offset);
		}
	}

	virtual void lua_set(lua_State* L, void* v) const
	{
		lua_newtable(L);
		int count = member_count();
		const REF_Member* mptr = members();
		for (int i = 0; i < count; ++i) {
			const REF_Member* m = mptr + i;
			lua_pushstring(L, m->name);
			m->type->lua_set(L, (char*)v + m->offset);
			lua_settable(L, -3);
		}
	}

	virtual void lua_get(lua_State* L, int index, void* v) const
	{
		assert(index > 0);
		assert(lua_istable(L, index));
		int count = member_count();
		const REF_Member* mptr = members();
		for (int i = 0; i < count; ++i) {
			const REF_Member* m = mptr + i;
			int n = m->type->lua_flatten_count();
			for (int j = 0; j < n; ++j) {
				lua_pushstring(L, m->name);
				lua_gettable(L, index + j);
				m->type->lua_get(L, -1, (char*)v + m->offset);
				lua_pop(L, 1);
			}
		}
	}
};

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
int REF_LuaCFunction(lua_State* L)
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

	// Allocate space on the C-stack for passing parameters to `fn`.
	int param_count = sig.param_count;
	REF_Variable* params = (REF_Variable*)alloca(sizeof(REF_Variable) * param_count);
	for (int i = 0; i < param_count; ++i) {
		params[i].type = sig.params[i];
		params[i].v = alloca(params[i].type->size());
	}

	// Fetch each parameter from Lua.
	for (int i = 0, idx = 0; i < param_count; i++) {
		params[i].type->lua_get(L, idx + 1, params[i].v);
		idx += params[i].type->lua_flatten_count();
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

#if 0 // Untested.
// Optional helper for manually binding functions that deal with arrays.
template <typename T>
void REF_LuaSetArray(lua_State* L, T* data, int count)
{
	const REF_Type* type = REF_GetType<T>();
	lua_newtable(L);
	int n = type->lua_flatten_count();
	for (int i = 0; i < count; ++i) {
		type->lua_set(L, data + i);
		for (int j = n; j >= 0; n--) {
			lua_rawseti(L, -2, i + 1 + j);
		}
	}
}
#endif

// Optional helper for manually binding functions that deal with arrays.
template <typename T>
void REF_LuaGetArray(lua_State* L, int index, T** out_ptr, int* out_count)
{
	const REF_Type* type = REF_GetType<T>();
	int count = (int)luaL_len(L, index);
	T* out = (T*)cf_alloc(type->size() * count);
	T* v = out;
	int n = type->lua_flatten_count();
	for (int i = 0; i < count; i += n) {
		for (int j = 0; j < n; ++j) {
			lua_rawgeti(L, index, i + 1 + j);
		}
		type->lua_get(L, -n, v++);
		lua_pop(L, n);
	}
	*out_ptr = out;
	if (out_count) *out_count = count;
}

// -------------------------------------------------------------------------------------------------
// User API.

void REF_BindLua(lua_State* L)
{
	// Call this from your `main`, binds all constants to Lua.
	for (const REF_Constant* c = REF_Constant::head(); c; c = c->next) {
		c->type->lua_set(L, (void*)&c->constant);
		lua_setglobal(L, c->name);
	}
	
	// Call this from `main`.
	// Binds all functions registered with REF_BIND_FUNCTION to Lua.
	for (const REF_Function* fn = REF_Function::head(); fn; fn = fn->next) {
		lua_pushlightuserdata(L, (void*)fn);
		lua_pushcclosure(L, REF_LuaCFunction, 1);
		lua_setglobal(L, fn->name());
	}
}

// Treat handles as void* within the reflection system.
#undef REF_HANDLE_TYPE
#define REF_HANDLE_TYPE(H) \
	template <> \
	struct REF_TypeGetter<H> \
	{ \
		static const REF_Type* get() \
		{ \
			return &g_void_ptr_Type; \
		} \
	}

// Expose a function to the reflection system.
// It will be automatically bound to Lua.
#undef REF_FUNCTION
#define REF_FUNCTION(F) \
	REF_Function g_##F##_REF_Function(#F, F)

// Expose a function to the reflection system.
// It will be automatically bound to Lua with a custom name.
#undef REF_FUNCTION_EX
#define REF_FUNCTION_EX(name, F) \
	REF_Function g_##name##_REF_Function(#name, F)

// Automatically bind a constant to Lua.
#undef REF_CONSTANT
#define REF_CONSTANT(C) \
	REF_Constant g_##C##_REF_Constant(#C, C)

// Expose a struct to the reflection system.
#undef REF_STRUCT
#define REF_STRUCT(T, ...) \
	static int T##_Type_members_data_sizeof(); \
	struct T##_Type : public REF_Struct \
	{ \
		using Type = T; \
		static const REF_Member members_data[]; \
		virtual const char* name() const { return #T; } \
		virtual int size() const { return sizeof(T); } \
		virtual const REF_Member* members() const { return members_data; } \
		virtual int member_count() const { return T##_Type_members_data_sizeof(); } \
	} g_##T##_Type; \
	const REF_Member T##_Type::members_data[] = { __VA_ARGS__ }; \
	static int T##_Type_members_data_sizeof() { return sizeof(T##_Type::members_data) / sizeof(*T##_Type::members_data); } \
	template <> struct REF_TypeGetter<T> { static const REF_Type* get() { return &g_##T##_Type; } }

// Expose a member of a struct to the reflection system.
#undef REF_MEMBER
#define REF_MEMBER(m) { #m, CF_OFFSET_OF(Type, m), REF_GetType<decltype(((Type*)0)->m)>() }
