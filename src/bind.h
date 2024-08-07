// Binds most C code to Lua using template metaprogramming techniques. The techniques
// were learned from John Edwards - https://www.youtube.com/watch?v=hWDZ3Yy-NMA
// 
// This technique heavily reduces boilerplate, and scales up very well, typically only costing
// one-line per type/function you wish to bind.
// 
// This was written specifically to bind Cute Framework to Lua and relies on CF's string type,
// but you can easily replace this with your own string type, and it will all work agnostically.
// https://github.com/RandyGaul/cute_framework

#pragma once

#include <cute.h>
using namespace Cute;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// -------------------------------------------------------------------------------------------------
// Documentation

// REF stands for reflection. This header works by collecting information about C-types in code, and
// implements some higher level algorithms for automating Lua bindings.
// 
// To get going you must register your types with the reflection by calling a variety of macros. They
// are designed to be mostly one-liners. Once done, call `REF_BindLua` once from main, and that's it.
// Make sure to call the macros at global scope, and keep it all in one translation unit (cpp file).
// 
// Happy Lua'ing!

// Overall your binding .cpp file should look something like this (all in one .cpp file/translation unit):
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

// Call this once from main to bind everything.
void REF_BindLua(lua_State* L);

// Syncs all global variables to Lua.
// Callable from Lua. Recommended to call this once per frame after gathering application inputs.
int REF_SyncGlobals(lua_State* L);

// Expose a handle type to the reflection system.
// You can use this on handles/ids 32/64-bits in size.
#define REF_HANDLE_TYPE(H)

// Expose a function to the reflection system. It will be bound to Lua with a matching name.
// Arrays must be explicitly marked by parameter indices, starting at index 0. Example:
// 
//     ConvexHull make_hull(v2* pts, int count);
//     REF_FUNCTION(make_hull, {0,1});
// 
// The brackets {0,1} specify an array/count parameter pair. You may set as many of these
// array pairs as you like in a comma separated list. The pair can be any indices, including
// any order, or even various pointers referring to the same count parameter. More examples
// and details found later in the docs (see "Notes on arrays").
#define REF_FUNCTION(F)

// Expose a function to the reflection system. It will be bound to Lua with a custom name.
#define REF_FUNCTION_EX(name, F)

// Wraps a manually written function and removes prefix "wrap_" from the name bound to Lua.
// This means functions with the signature: int func(lua_State* L)
#define REF_WRAP_MANUAL(F)

// Expose a constant to the reflection system. This is for defines, string literals, enums, etc.
// The constant will be cast to 64-bit `uintptr_t`.
#define REF_CONSTANT(C)

// Expose a constant to the reflection system with a custom name.
#define REF_CONSTANT_EX(C, name)

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
// different call to `REF_STRUCT`. You may also reference array members with REF_MEMBER_ARRAY.
#define REF_STRUCT(T, ...)

// Expose a member of a struct to the reflection system. Example:
// 
//     REF_STRUCT(MyStruct
//          REF_MEMBER(x),
//          REF_MEMBER(y),
//          REF_MEMBER(data),
//     );
#define REF_MEMBER(M)

// Expose a member array of a struct to the reflection system. the count member does *not*
// need to bound explicitly with another REF_MEMBER. Example:
// 
//     struct ConvexHull
//     {
//         v2* pts;
//         int count;
//     };
//     
//     REF_STRUCT(ConvexHull,
//         REF_MEMBER_ARRAY(pts, count)
//     );
// 
// This will also work for inline arrays.
// 
//     struct ConvexHull
//     {
//         int count;
//         v2 pts[8];
//     };
//     
//     REF_STRUCT(ConvexHull,
//         REF_MEMBER_ARRAY(pts, count)
//     );
// 
// The order the array pointer and count members does not matter within the struct layout.
#define REF_MEMBER_ARRAY(T, count)

// Expose a global variable to the reflection system. It will be bound to Lua with a matching name.
// This will get sync'd to Lua each time REF_SyncGlobals is called. Note that REF_SyncGlobals is also
// callable from Lua.
#define REF_GLOBAL(G)

// Expose a global variable to the reflection system with a custom name.
#define REF_GLOBAL_EX(G, name)

// Call a function in Lua.
// The return values *must* be in an initializer list. Be sure they are proper l-values (no literals), because
// we must write to the return values.
// Returns the number of return values from Lua, and zero's out any remaining return value's you passed in.
// Extra parameters in Lua will remain nil as usual.
// You need to call `sfree` on any return'd strings, and/or call REF_GetType<T>()->cleanup(ptr) for all return values that allocated memory.
// 
//     template <typename... Params>
//     int REF_CallLuaFunction(lua_State* L, const char* fn_name, std::initializer_list<REF_Variable> return_values, Params... params)
// 
// Example
// 
//     int a, b; // Return values.
//     REF_CallLuaFunction(L, "my_function", { a, b }, "hello", 0, -10);
//     printf("%d, %d\n", a, b);
// 
//     -- In lua we have:
//     function my_function(s, i, d)
//         print(s, i, d)
//         return -5, 200
//     end
// 
//     Which would print the following:
//     "hello"    0    -10
//     -5
//     200

// You may flatten structs down for structs that contain all-floats or all-ints. This is
// a nice thing to do for math types. This will send each member of the struct
// by value rather than as a key/value pair.
#define REF_FLAT_FLOATS(T)
#define REF_FLAT_INTS(T)

// Passing Lua callbacks into C functions is a little tricky, but not too difficult.
// Bind a function to Lua to collect the callback name from Lua as a string. Store this
// somewhere, for example as a userdata pointer. Pass a wrapper callback to your C code.
// The wrapper callback simply fetches the Lua string name, and then calls the Lua
// callback. It's highly recommended to use REF_CallLuaFunction to make this process simple.
// If done correctly, the C callback wrapper should look something like this:
// 
// void wrap_Callback(int a, float y, void* userdata)
// {
//     lua_State* L = ((MyLuaContext*)userdata)->L;
//     const char* lua_fn_name = ((MyLuaContext*)userdata)->lu_fn_name;
//     int error = 0;
//     v2 pts* = NULL;
//     int count = 0;
//     
//     // a, y are function parameters sent to lua_fn_name.
//     // The return values are error, and an array comprised of {pts, count}.
//     REF_CallLuaFunction(L, lua_fn_name, { error, REF_Array(pts, count) }, a, y);
//     
//     // Cleanup any arrays/strings that Lua sent back as return values.
//     free(pts);
// }

// Notes on arrays.
// 
// In Lua arrays are treated as indexed tables. Whenever we send an array from
// Lua to C, as a function parameter, the count of the array is *not* sent explicitly
// on the Lua side. It will get automatically injected into the correct C parameter.
// This works since we explicitly state arrays when binding functions via REF_FUNCTION.
// Example:
// 
//    ConvexHull make_hull(v2* pts, int count);
// 
// We can call this in Lua like so:
// 
//     hull = make_hull(vertices)
//
// This will work so long as the array parameter pair is marked when binding, like so:
// 
//     REF_FUNCTION(make_hull, {0,1});
// 
// You can also deal with multiple arrays in any order.
// 
//     void example(int data0*, int count0, const char* name, int count1, float f, int* data1);
//     REF_FUNCTION(example, {0,1}, {5,3});
// 
// Which would be called in Lua like so (yes this function is crazy, but it can be done):
// 
//     ex = example(data0, name, f, data1)
// 
// Whenever arrays are sent as return values back from Lua we must clean them up
// on the C-side. See `REF_CallLuaFunction` for details. This includes strings.

// -------------------------------------------------------------------------------------------------
// REF - Reflection implementation.

// alloca
#ifdef _WIN32
	#include <malloc.h>
#else
	#include <alloca.h>
#endif

#include <utility>

// For debugging.
inline void REF_PrintLuaStack(lua_State *L)
{
	int top = lua_gettop(L);
	printf("Lua stack (top to bottom):\n");
	for (int i = top; i >= 1; i--) {
		int type = lua_type(L, i);
		switch (type) {
			case LUA_TNIL:
				printf("[%d] NIL\n", i);
				break;
			case LUA_TBOOLEAN:
				printf("[%d] %s\n", i, lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("[%d] %f\n", i, lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				printf("[%d] '%s'\n", i, lua_tostring(L, i));
				break;
			case LUA_TTABLE:
				printf("[%d] TABLE\n", i);
				break;
			case LUA_TFUNCTION:
				printf("[%d] FUNCTION\n", i);
				break;
			case LUA_TUSERDATA:
				printf("[%d] USERDATA\n", i);
				break;
			case LUA_TTHREAD:
				printf("[%d] THREAD\n", i);
				break;
			case LUA_TLIGHTUSERDATA:
				printf("[%d] LIGHTUSERDATA\n", i);
				break;
			default:
				printf("[%d] Unknown type\n", i);
				break;
		}
	}
	printf("\n");
}

// An abstract representation of types in C++, used to write generic routines
// for binding things to Lua.
struct REF_Type
{
	virtual const char* name() const = 0;
	virtual int size() const = 0;
	virtual double to_number(void* v) const = 0;
	virtual String to_string(void* v) const = 0;
	virtual void cast(void* to, void* from, const REF_Type* from_type) const = 0;
	virtual void cleanup(void* v) const { }
	virtual bool is_integral() const { return false; }
	virtual bool is_pointer() const = 0;
	virtual const REF_Type* dereference_type() const = 0;
	virtual const REF_Type* address_type() const = 0;
	virtual void lua_set(lua_State* L, void* v) const = 0;
	virtual void lua_get(lua_State* L, int index, void* v) const = 0;
	virtual int flattened_count() const { return 1; }
	void zero(void* v) const { CF_MEMSET(v, 0, size()); }
};

// Display a helpful message if a function or constant was attempted to be bound
// to Lua, but had some type not yet represented by the reflection.
template<typename T, typename Enable = void>
struct REF_TypeGetter
{
	static const REF_Type* get()
	{
		static_assert(sizeof(T) == -1, "Type not registered with reflection (see type below, in the next error).");
		T = T;
		return NULL;
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

template<typename T>
struct remove_array_extents
{
	using type = T;
};

template<typename T, size_t N>
struct remove_array_extents<T[N]>
{
	using type = typename remove_array_extents<T>::type;
};

template<typename T>
struct remove_array_extents<T[]>
{
	using type = typename remove_array_extents<T>::type;
};

// Remove const and volatile qualifiers, including for pointer types.
template<typename T>
struct remove_cv
{
	using type = typename std::remove_cv<T>::type;
};

template<typename T>
struct remove_cv<T*>
{
	using type = typename remove_cv<typename std::remove_cv<T>::type>::type*;
};

// Helper to fetch type info by template parameter.
template<typename T>
const REF_Type* REF_GetType()
{
	return REF_TypeGetter<typename remove_array_extents<typename remove_cv<T>::type>::type>::get();
}

// No-ops representing void, mainly for functions that return nothing.
struct void_Type : public REF_Type
{
	virtual const char* name() const override { return "void"; }
	virtual int size() const override { return 0; }
	virtual double to_number(void* v) const override { return 0; }
	virtual String to_string(void* v) const override { return String(); }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const override { }
	virtual void cleanup(void* v) const override { }
	virtual bool is_pointer() const override { return false; }
	virtual const REF_Type* dereference_type() const override { return NULL; }
	virtual const REF_Type* address_type() const override { return NULL; }
	virtual void lua_set(lua_State* L, void* v) const override { }
	virtual void lua_get(lua_State* L, int index, void* v) const override { }
	virtual int flattened_count() const override { return 0; }
} g_void_Type;
template <> struct REF_TypeGetter<void> { static const REF_Type* get() { return &g_void_Type; } };

struct void_Ptr_Type : public REF_Type
{
	virtual const char* name() const override { return "void*"; } 
	virtual int size() const { return sizeof(void*); }
	virtual double to_number(void* v) const override { return 0; } 
	virtual String to_string(void* v) const override { return String::from_hex((uint64_t)v); } 
	virtual void cast(void* to, void* from, const REF_Type* from_type) const override { assert(this == from_type); *(void**)to = *(void**)from; }
	virtual bool is_pointer() const override { return true; } 
	virtual const REF_Type* dereference_type() const override { return NULL; } 
	virtual const REF_Type* address_type() const override { return REF_GetType<void**>(); } 
	virtual void lua_set(lua_State* L, void* v) const override { lua_pushlightuserdata(L, *(void**)v); } 
	virtual void lua_get(lua_State* L, int index, void* v) const override { *(void**)v = (void*)lua_touserdata(L, index); }
} g_void_Ptr_Type;
template <> struct REF_TypeGetter<void*> { static const REF_Type* get() { return &g_void_Ptr_Type; } };

// Helper to reduce repetitive binds of C numeric types.
#define REF_NUMERIC_TYPE(T) \
	struct T##_Type : public REF_Type \
	{ \
		virtual const char* name() const override { return #T; } \
		virtual int size() const { return sizeof(T); } \
		virtual double to_number(void* v) const override { return (double)*(T*)v; } \
		virtual String to_string(void* v) const override { return *(T*)v; } \
		virtual void cast(void* to, void* from, const REF_Type* from_type) const override { *(T*)to = (T)from_type->to_number(from); } \
		virtual bool is_integral() const override { return true; } \
		virtual bool is_pointer() const override { return false; } \
		virtual const REF_Type* dereference_type() const override { return NULL; } \
		virtual const REF_Type* address_type() const override { return REF_GetType<T*>(); } \
		virtual void lua_set(lua_State* L, void* v) const override { lua_pushnumber(L, (double)*(T*)v); } \
		virtual void lua_get(lua_State* L, int index, void* v) const override { *(T*)v = (T)lua_tonumber(L, index); } \
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

#define REF_PTR_TYPE(T) \
	struct T##_Ptr_Type : public REF_Type \
	{ \
		virtual const char* name() const override { return #T "*"; } \
		virtual int size() const { return sizeof(T*); } \
		virtual double to_number(void* v) const override { return 0; } \
		virtual String to_string(void* v) const override { return *(T**)v; } \
		virtual void cast(void* to, void* from, const REF_Type* from_type) const override { assert(this == from_type); *(T**)to = *(T**)from; } \
		virtual bool is_pointer() const override { return true; } \
		virtual const REF_Type* dereference_type() const override { return REF_GetType<T>(); } \
		virtual const REF_Type* address_type() const override { return REF_GetType<T**>(); } \
		virtual void lua_set(lua_State* L, void* v) const override { lua_pushlightuserdata(L, *(T**)v); } \
		virtual void lua_get(lua_State* L, int index, void* v) const override { *(T**)v = (T*)lua_touserdata(L, index); } \
	} g_##T##_Ptr_Type; \
	template <> struct REF_TypeGetter<T*> { static const REF_Type* get() { return &g_##T##_Ptr_Type; } }

REF_PTR_TYPE(short);
REF_PTR_TYPE(int);
REF_PTR_TYPE(float);
REF_PTR_TYPE(double);
REF_PTR_TYPE(uint8_t);
REF_PTR_TYPE(int8_t);
REF_PTR_TYPE(uint16_t);
//REF_PTR_TYPE(int16_t); // Just a typedef of short.
REF_PTR_TYPE(uint32_t);
//REF_PTR_TYPE(int32_t); // Just a typedef of int.
REF_PTR_TYPE(uint64_t);
REF_PTR_TYPE(int64_t);

#define REF_PTR_PTR_TYPE(T) \
	struct T##_Ptr_Ptr_Type : public REF_Type \
	{ \
		virtual const char* name() const override { return #T "**"; } \
		virtual int size() const { return sizeof(T**); } \
		virtual double to_number(void* v) const override { return 0; } \
		virtual String to_string(void* v) const override { return *(T***)v; } \
		virtual void cast(void* to, void* from, const REF_Type* from_type) const override { assert(this == from_type); *(T***)to = *(T***)from; } \
		virtual bool is_pointer() const override { return true; } \
		virtual const REF_Type* dereference_type() const override { return REF_GetType<T*>(); } \
		virtual const REF_Type* address_type() const override { return NULL; } \
		virtual void lua_set(lua_State* L, void* v) const override { lua_pushlightuserdata(L, *(T***)v); } \
		virtual void lua_get(lua_State* L, int index, void* v) const override { *(T***)v = (T**)lua_touserdata(L, index); } \
	} g_##T##_Ptr_Ptr_Type; \
	template <> struct REF_TypeGetter<T**> { static const REF_Type* get() { return &g_##T##_Ptr_Ptr_Type; } }

REF_PTR_PTR_TYPE(void);
REF_PTR_PTR_TYPE(short);
REF_PTR_PTR_TYPE(int);
REF_PTR_PTR_TYPE(float);
REF_PTR_PTR_TYPE(double);
REF_PTR_PTR_TYPE(uint8_t);
REF_PTR_PTR_TYPE(int8_t);
REF_PTR_PTR_TYPE(uint16_t);
//REF_PTR_PTR_TYPE(int16_t); // Just a typedef of short.
REF_PTR_PTR_TYPE(uint32_t);
//REF_PTR_PTR_TYPE(int32_t); // Just a typedef of int.
REF_PTR_PTR_TYPE(uint64_t);
REF_PTR_PTR_TYPE(int64_t);

// Expose an opaque pointer type to the reflection system.
#define REF_OPAQUE_PTR_TYPE(T) \
	struct T##_Ptr_Type : public REF_Type \
	{ \
		virtual const char* name() const override { return #T "*"; } \
		virtual int size() const { return sizeof(T*); } \
		virtual double to_number(void* v) const override { return 0; } \
		virtual String to_string(void* v) const override { return *(T**)v; } \
		virtual void cast(void* to, void* from, const REF_Type* from_type) const override { assert(this == from_type); *(T**)to = *(T**)from; } \
		virtual bool is_pointer() const override { return true; } \
		virtual const REF_Type* dereference_type() const override { return NULL; } \
		virtual const REF_Type* address_type() const override { return NULL; } \
		virtual void lua_set(lua_State* L, void* v) const override { lua_pushlightuserdata(L, *(T**)v); } \
		virtual void lua_get(lua_State* L, int index, void* v) const override { *(T**)v = (T*)lua_touserdata(L, index); } \
	} g_##T##_Ptr_Type; \
	template <> struct REF_TypeGetter<T*> { static const REF_Type* get() { return &g_##T##_Ptr_Type; } }

struct bool_Type : public REF_Type
{
	virtual const char* name() const { return "bool"; }
	virtual int size() const { return sizeof(bool); }
	virtual double to_number(void* v) const { return *(bool*)v ? 1.0 : 0.0; }
	virtual String to_string(void* v) const { return *(bool*)v ? "true" : "false"; }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const { *(bool*)to = (from_type->to_number(from) == 0.0 ? false : true); }
	virtual void cleanup(void* v) const { }
	virtual bool is_pointer() const override { return false; }
	virtual const REF_Type* dereference_type() const override { return NULL; }
	virtual const REF_Type* address_type() const override { return REF_GetType<bool*>(); }
	virtual void lua_set(lua_State* L, void* v) const { lua_pushboolean(L, *(bool*)v); }
	virtual void lua_get(lua_State* L, int index, void* v) const { *(bool*)v = lua_toboolean(L, index); }
} g_bool_Type;
template <> struct REF_TypeGetter<bool> { static const REF_Type* get() { return &g_bool_Type; } };
REF_PTR_TYPE(bool);
REF_PTR_PTR_TYPE(bool);

struct char_ptr_Type : public REF_Type
{
	virtual const char* name() const { return "char*"; }
	virtual int size() const { return sizeof(char*); }
	virtual double to_number(void* v) const { return stodouble(*(char**)v); }
	virtual String to_string(void* v) const { return *(char**)v; }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const { String s = from_type->to_string(from); *(char**)to = s.steal(); }
	virtual void cleanup(void* v) const { sfree(*(char**)v); }
	virtual bool is_pointer() const override { return true; }
	virtual const REF_Type* dereference_type() const override { return REF_GetType<char>(); }
	virtual const REF_Type* address_type() const override { return NULL; }
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
	virtual bool is_pointer() const override { return false; }
	virtual const REF_Type* dereference_type() const override { return REF_GetType<char>(); }
	virtual const REF_Type* address_type() const override { return NULL; }
	virtual void lua_set(lua_State* L, void* v) const { lua_pushstring(L, ((String*)v)->c_str()); }
	virtual void lua_get(lua_State* L, int index, void* v) const { *(String*)v = String(lua_tostring(L, index)); }
} g_String_Type;
template <> struct REF_TypeGetter<String> { static const REF_Type* get() { return &g_String_Type; } };

void REF_LuaGetArray(lua_State* L, int index, const REF_Type* type, void* v, int count)
{
	int n = type->flattened_count();
	int count_n = count * n;
	for (int i = 0; i < count_n; i += n) {
		for (int j = 0; j < n; ++j) {
			lua_rawgeti(L, index, i + 1 + j);
		}
		type->lua_get(L, -n, v);
		v = (void*)((uintptr_t)v + type->size());
		lua_pop(L, n);
	}
}

// Helper for manually binding functions that deal with arrays.
template <typename T>
int REF_LuaGetDynamicArray(lua_State* L, int index, T** out_ptr)
{
	const REF_Type* type = REF_GetType<T>();
	int count = (int)luaL_len(L, index) / type->flattened_count();
	T* v = (T*)cf_alloc(type->size() * count);
	REF_LuaGetArray(L, index, type, v, count);
	*out_ptr = v;
	return count;
}

void REF_LuaSetArray(lua_State* L, void* v, const REF_Type* type, int count)
{
	int n = type->flattened_count();
	int count_n = count * n;
	int table = lua_gettop(L);
	for (int i = 0; i < count_n; i += n) {
		type->lua_set(L, v);
		v = (void*)(((uintptr_t)v) + type->size());
		for (int j = n - 1; j >= 0; --j) {
			lua_rawseti(L, table, i + j + 1);
		}
	}
}

template <typename T>
void REF_LuaSetArray(lua_State* L, T* items, int count)
{
	REF_LuaSetArray(L, (void*)items, REF_GetType<std::remove_const<T>::type>(), count);
}

// Describes a data member of a struct.
// Assumes plain-old-data.
struct REF_Member
{
	const char* name = NULL;
	size_t offset = 0;
	const REF_Type* type = NULL;

	const char* array_count_name = NULL;
	size_t array_count_offset = 0;
	const REF_Type* array_count_type = NULL;
	bool is_array_external = false;

	bool is_array() const { return array_count_name == NULL ? false : true; }
};

// Represents a struct as a collection of REF_Member pointers. This maps to key'd tables in Lua.
struct REF_Struct : public REF_Type
{
	virtual const char* name() const = 0;
	virtual int size() const = 0;
	virtual const REF_Member* members() const = 0;
	virtual int member_count() const = 0;

	virtual double to_number(void* v) const override { return 0; }
	virtual String to_string(void* v) const override { return String(); }
	virtual void cast(void* to, void* from, const REF_Type* from_type) const override { assert(from_type == this); CF_MEMCPY(to, from, size()); }

	virtual void cleanup(void* v) const
	{
		int count = member_count();
		const REF_Member* mptr = members();
		for (int i = 0; i < count; ++i) {
			const REF_Member* m = mptr + i;
			void* mv = (void*)((uintptr_t)v + m->offset);
			if (m->is_array() && m->is_array_external) {
				// Cleanup any dynamically allocated external arrays.
				void* data = *(void**)mv;
				cf_free(data);
			} else {
				m->type->cleanup(mv);
			}
		}
	}

	virtual bool is_pointer() const override { return false; }

	virtual void lua_set(lua_State* L, void* v) const override
	{
		const char* struct_name = name();
		int count = member_count();
		const REF_Member* mptr = members();
		lua_newtable(L);

		// Set each struct member one at a time.
		for (int i = 0; i < count; ++i) {
			const REF_Member* m = mptr + i;
			void* mv = (void*)((uintptr_t)v + m->offset);
			lua_pushstring(L, m->name);
			if (m->is_array()) {
				// Set an array.
				lua_newtable(L);
				int n = 0;
				REF_GetType<int>()->cast(&n, (void*)((uintptr_t)v + m->array_count_offset), m->array_count_type);
				void* data = mv;
				if (m->is_array_external) {
					void* data = *(void**)mv;
					REF_LuaSetArray(L, data, m->type->dereference_type(), n);
				} else {
					REF_LuaSetArray(L, data, m->type, n);
				}
			} else {
				// Non-array member.
				int n = m->type->flattened_count();
				if (n > 1) {
					// Sets flattened types as indexed arrays.
					lua_newtable(L);
					int table = lua_gettop(L);
					m->type->lua_set(L, mv);
					for (int j = n - 1; j >= 0; --j) {
						lua_rawseti(L, table, j + 1);
					}
				} else {
					// Set the string-key'd value.
					m->type->lua_set(L, mv);
				}
			}
			lua_settable(L, -3);
		}
	}

	virtual void lua_get(lua_State* L, int index, void* v) const override
	{
		assert(index > 0);
		assert(lua_istable(L, index));
		int count = member_count();
		const REF_Member* mptr = members();

		// Read in each struct member one at a time.
		for (int i = 0; i < count; ++i) {
			const REF_Member* m = mptr + i;
			lua_pushstring(L, m->name);
			lua_gettable(L, index);
			void* mv = (void*)((uintptr_t)v + m->offset);
			if (lua_isnil(L, -1)) {
				// Ignore missing keys -- often in Lua it's convenient to just
				// not set certain struct members.
				lua_pop(L, 1);
				m->type->zero(mv);
				continue;
			}
			if (m->is_array()) {
				// Read in an array.
				assert(lua_istable(L, -1));
				if (m->is_array_external) {
					// Dynamically allocate an external array, this is cleaned up in cleanup().
					const REF_Type* element_type = m->type->dereference_type();
					int n = (int)luaL_len(L, -1) / element_type->flattened_count();
					m->array_count_type->cast((void*)((uintptr_t)v + m->array_count_offset), &n, REF_GetType<int>());
					void* data = cf_alloc(n * element_type->size());
					*(void**)mv = data;
					REF_LuaGetArray(L, lua_gettop(L), element_type, data, n);
				} else {
					// Read the array straight into the struct instance.
					int n = (int)luaL_len(L, -1) / m->type->flattened_count();
					m->array_count_type->cast((void*)((uintptr_t)v + m->array_count_offset), &n, REF_GetType<int>());
					REF_LuaGetArray(L, lua_gettop(L), m->type, mv, n);
				}
			} else {
				// Non-array member.
				int n = m->type->flattened_count();
				if (n > 1) {
					// Read in flattened types as indexed arrays.
					assert(lua_istable(L, -1));
					for (int j = 0; j < n; ++j) {
						lua_rawgeti(L, index + 1, j + 1);
					}
					m->type->lua_get(L, lua_gettop(L)-n+1, mv);
					lua_pop(L, n);
				} else {
					// Read in string-key'd member.
					m->type->lua_get(L, lua_gettop(L), mv);
				}
			}
			lua_pop(L, 1);
		}
	}
};

// An abstract representation of a typed pointer, useful for implementing generic utilities
// for calling and binding functions/constants to Lua.
struct REF_Variable
{
	template <typename T>
	REF_Variable(const T& t)
	{
		v = (void*)&t;
		type = REF_GetType<T>();
		is_array = false;
		array_count = 0;
	}
	template <typename T>
	REF_Variable(const T* t, int count)
	{
		v = (void*)t;
		type = REF_GetType<T>();
		is_array = true;
		array_count = count;
	}
	REF_Variable() { }
	void* v = NULL;
	const REF_Type* type = REF_GetType<void>();
	bool is_array = false;
	int array_count = 0;
};

// Very hacky helper to send arrays to Lua.
#define REF_Array(V, count) REF_Variable(V, count)

// Syntactic sugar to cast from one type to another.
template <typename T>
T REF_Cast(REF_Variable v)
{
	T to;
	REF_GetType<T>()->cast(&to, v.v, v.type);
	return to;
}

// Syntactic sugar to get a type off of the Lua stack.
template <typename T>
void REF_LuaGet(lua_State* L, int index, T* t)
{
	REF_GetType<T>()->lua_get(L, index, (void*)t);
}

// Syntactic sugar to set a type onto the Lua stack.
template <typename T>
void REF_LuaSet(lua_State* L, T* t)
{
	REF_GetType<T>()->lua_set(L, (void*)t);
}

// Captures array param+count indices for REF_FunctionSignature.
struct REF_ArrayParameter
{
	int array_index;
	int count_index;
};

// Stores arrays of type information to fully describe the signature of C-style function.
struct REF_FunctionSignature
{
	template <typename R, typename... Params>
	REF_FunctionSignature(R (*)(Params...), std::initializer_list<REF_ArrayParameter> array_param_list)
	{
		if constexpr (sizeof...(Params) > 0) {
			static const REF_Type* params[] = { REF_GetType<Params>()... };
			static int param_array_count_index[sizeof...(Params)];
			static bool param_is_array[sizeof...(Params)];
			static bool param_is_array_count[sizeof...(Params)];
			this->param_count = sizeof...(Params);
			this->params = params;
			for (int i = 0; i < array_param_list.size(); ++i) {
				param_is_array[array_param_list.begin()[i].array_index] = true;
				param_array_count_index[i] = array_param_list.begin()[i].count_index;
				param_is_array_count[param_array_count_index[i]] = true;
			}
			this->param_is_array = param_is_array;
			this->param_is_array_count = param_is_array_count;
			this->param_array_count_index = param_array_count_index;
		}
		return_type = REF_GetType<R>();
	}

	int param_count = 0;
	const REF_Type** params = NULL;
	const bool* param_is_array_count = NULL;
	const bool* param_is_array = NULL;
	const int* param_array_count_index = NULL;
	const REF_Type* return_type = NULL;
};

// Unpacks the variadic parameters as an array.
template <typename R, typename... Params, std::size_t... I>
R REF_ApplyHelper(R (*fn)(Params...), REF_Variable* params, std::index_sequence<I...>)
{
	return fn(REF_Cast<Params>(params[I])...);
}

// Calls a function in a generic way using typed pointers.
template <typename R, typename... Params>
void REF_Apply(R (*fn)(Params...), REF_Variable ret, REF_Variable* params, int param_count)
{
	assert(param_count == sizeof...(Params));
	R r = REF_ApplyHelper(fn, params, std::make_index_sequence<sizeof...(Params)>());
	ret.type->cast(ret.v, &r, REF_GetType<R>());
}

// Special case to hand void return.
template <typename... Params, std::size_t... I>
void REF_ApplyHelper(void (*fn)(Params...), REF_Variable* params, std::index_sequence<I...>)
{
	return fn(REF_Cast<Params>(params[I])...);
}

// Special case to hand void return.
template <typename... Params>
void REF_Apply(void (*fn)(Params...), REF_Variable ret, REF_Variable* params, int param_count)
{
	assert(param_count == sizeof...(Params));
	REF_ApplyHelper(fn, params, std::make_index_sequence<sizeof...(Params)>());
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
void REF_ApplyWrapper(void (*fn)(), REF_Variable ret, REF_Variable* params, int param_count)
{
	REF_Apply((T)fn, ret, params, param_count);
}

// A generic functor object, used to easily bind functions to Lua.
struct REF_Function : public REF_List<REF_Function>
{
	template <typename T>
	REF_Function(const char* name, T fn, std::initializer_list<REF_ArrayParameter> arrays)
		: m_name(name)
		, m_sig(fn, arrays)
		, m_fn((void (*)())fn)
		, m_fn_wrapper(REF_ApplyWrapper<T>)
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

template <typename... Params>
int REF_CallLuaFunction(lua_State* L, const char* fn_name, std::initializer_list<REF_Variable> return_values, Params... params);

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

	// Allocate stack space for each parameter.
	int param_count = sig.param_count;
	int param_count_without_array_counts = param_count;
	REF_Variable* params = (REF_Variable*)alloca(sizeof(REF_Variable) * param_count);
	CF_MEMSET(params, 0, sizeof(REF_Variable) * param_count);
	for (int i = 0; i < param_count; ++i) {
		params[i].type = sig.params[i];
		params[i].v = alloca(params[i].type->size());
		if (sig.param_is_array_count[i]) {
			--param_count_without_array_counts;
		} else if (sig.param_is_array[i]) {
			params[i].is_array = true;
		}
	}

	if (lua_gettop(L) < param_count_without_array_counts) {
		String s = String::fmt("Mismatch of parameter count (%d) when calling %s (expected %d).\n", lua_gettop(L), fn->name(), param_count_without_array_counts);
		luaL_traceback(L, L, s.c_str(), 1);
		const char* error_and_stack_trace = lua_tostring(L, -1);
		s = error_and_stack_trace;
		lua_pop(L, 1);
		REF_CallLuaFunction(L, "REF_ErrorHandler", { }, s.c_str());
		return 0;
	}

	// Read in each parameter from Lua.
	const int alloca_limit = 1024;
	for (int i = 0, idx = 0; i < param_count; i++) {
		REF_Variable* param = params + i;
		int flattened_count = param->type->flattened_count();

		// Don't fetch array counts from Lua, as they are instead set by querying
		// the Lua table itself.
		if (sig.param_is_array_count[i]) {
			continue;
		}

		if (sig.param_is_array[i]) {
			// Fetch the element count from the Lua table.
			assert(lua_istable(L, idx+1));
			const REF_Type* element_type = param->type->dereference_type();
			flattened_count = element_type->flattened_count();
			int count = (int)luaL_len(L, idx+1) / flattened_count;

			// Set the element count in the correct parameter.
			REF_Variable* vcount = params + sig.param_array_count_index[i];
			vcount->type->cast(vcount->v, &count, REF_GetType<int>());
			param->array_count = count;

			// Copy count/data into the parameter.
			int sz = element_type->size() * count;
			void* data = sz > alloca_limit ? cf_alloc(sz) : alloca(sz);
			REF_LuaGetArray(L, idx+1, element_type, data, count);
			*(void**)param->v = data;

			// Only skip over the array's table.
			idx += 1;
		} else {
			// Read in the parameter from Lua.
			param->type->lua_get(L, idx+1, param->v);
			idx += flattened_count;
		}
	}

	// Call the actual function.
	fn->apply(ret, params, param_count);

	// Cleanup any temporary storage (curse you, c-style strings).
	for (int i = 0; i < param_count; ++i) {
		params[i].type->cleanup(params[i].v);

		// Free any dynamically allocated arrays.
		if (params[i].is_array) {
			int sz = params[i].type->size() * params[i].array_count;
			if (sz > alloca_limit) {
				cf_free(params[i].v);
			}
		}
	}

	// Pass return value(s) back to Lua.
	if (ret.type->size() > 0) {
		ret.type->lua_set(L, ret.v);
		return ret.type->flattened_count();
	} else {
		return 0;
	}
}

// Represents a global constant for binding to Lua.
struct REF_Constant : REF_List<REF_Constant>
{
	template <typename T>
	REF_Constant(const char* name, T t)
		: name(name)
	{
		if constexpr (std::is_same<T, char*>::value || std::is_same<T, const char*>::value) {
			type = REF_GetType<char*>();
			constant = (uintptr_t)t;
		} else if constexpr (std::is_floating_point<T>::value) {
			type = REF_GetType<double>();
			constant = (uintptr_t)t;
		} else if constexpr (std::is_integral<T>::value || std::is_enum<T>::value) {
			type = REF_GetType<uint64_t>();
			constant = (uintptr_t)t;
		} else if constexpr (sizeof(T) == sizeof(int)) {
			type = REF_GetType<int>();
			constant = (uintptr_t)*(int*)&t;
		} else if constexpr (sizeof(T) == sizeof(uint64_t)) {
			type = REF_GetType<uint64_t>();
			constant = *(uint64_t*)&t;
		} else {
			assert(false);
		}
	}

	const char* name;
	uintptr_t constant;
	const REF_Type* type;
};

// Facilitates a call to any Lua function.
int REF_CallLuaFunctionHelper(lua_State* L, const char* fn_name, const REF_Variable* rets, int ret_count, const REF_Variable* params, int param_count)
{
	// Print stack trace upon errors.
	static auto traceback = [](lua_State* L) -> int {
		const char* msg = lua_tostring(L, 1);
		if (msg) {
			luaL_traceback(L, L, msg, 1);
		} else if (!lua_isnoneornil(L, 1)) {
			// Print __tostring for error objects, if they exist.
			if (!luaL_callmeta(L, 1, "__tostring"))
				lua_pushliteral(L, "(no error message)");
		}
		return 1;
	};
	lua_pushcfunction(L, traceback);
	int base = lua_gettop(L);

	// Fetch the function in Lua.
	lua_getglobal(L, fn_name);
	if (!lua_isfunction(L, -1)) {
		fprintf(stderr, "Function %s not found in Lua.\n", fn_name);
		exit(-1);
	}

	// Push all parameters onto the Lua stack.
	int flattened_param_count = 0;
	for (int i = 0; i < param_count; ++i) {
		if (params[i].is_array) {
			lua_newtable(L);
			REF_LuaSetArray(L, params[i].v, params[i].type, params[i].array_count);
			++flattened_param_count;
		} else {
			params[i].type->lua_set(L, params[i].v);
			flattened_param_count += params[i].type->flattened_count();
		}
	}

	// Call the actual Lua function.
	if (lua_pcall(L, flattened_param_count, LUA_MULTRET, base) != LUA_OK) {
		REF_CallLuaFunction(L, "REF_ErrorHandler", { }, lua_tostring(L, -1));
		return 0;
	}

	// Remove the stack trace function.
	lua_remove(L, base);

	// Fetch the return values.
	int nresults = lua_gettop(L) - base + 1;
	if (nresults != ret_count) {
		fprintf(stderr, "Mismatch of return values from Lua, expected %d, got %d.\n", ret_count, nresults);
	}
	for (int i = min(ret_count, nresults - 1); i >= 0; --i) {
		rets[i].type->lua_get(L, lua_gettop(L), rets[i].v);
		lua_pop(L, 1);
	}

	// Zero out any remaining return values.
	for (int i = nresults; i < ret_count; ++i) {
		rets[i].type->zero(rets[i].v);
	}

	return nresults;
}

// Call a function in Lua.
template <typename... Params>
int REF_CallLuaFunction(lua_State* L, const char* fn_name, std::initializer_list<REF_Variable> return_values, Params... params)
{
	if constexpr (sizeof...(Params) > 0) {
		REF_Variable params_v[] = { params... };
		return REF_CallLuaFunctionHelper(L, fn_name, return_values.begin(), (int)return_values.size(), params_v, sizeof(params_v) / sizeof(*params_v));
	} else {
		return REF_CallLuaFunctionHelper(L, fn_name, return_values.begin(), (int)return_values.size(), NULL, 0);
	}
}

// Overload for zero return values, zero parameters.
int REF_CallLuaFunction(lua_State* L, const char* fn_name)
{
	return REF_CallLuaFunctionHelper(L, fn_name, NULL, 0, NULL, 0);
}

// -------------------------------------------------------------------------------------------------
// User API.

// Treat handles as void* within the reflection system.
#undef REF_HANDLE_TYPE
#define REF_HANDLE_TYPE(H) \
	template <> \
	struct REF_TypeGetter<H> \
	{ \
		static const REF_Type* get() \
		{ \
			if constexpr (sizeof(H) == sizeof(void*)) { \
				return &g_void_Ptr_Type; \
			} else if constexpr (sizeof(H) == sizeof(int)) { \
				return &g_int_Type; \
			} else { \
				static_assert(sizeof(H) == sizeof(void*) || sizeof(H) == sizeof(int), "Invalid handle size, only 64/32 bit handles are supported."); \
				return NULL; \
			} \
		} \
	}

// Expose a function to the reflection system.
// It will be automatically bound to Lua.
#undef REF_FUNCTION
#define REF_FUNCTION(F, ...) \
	REF_Function g_##F##_REF_Function(#F, F, { __VA_ARGS__ })

// Expose a function to the reflection system.
// It will be automatically bound to Lua with a custom name.
#undef REF_FUNCTION_EX
#define REF_FUNCTION_EX(name, F, ...) \
	REF_Function g_##name##_REF_Function(#name, F, { __VA_ARGS__ })

// Automatically bind a constant to Lua.
#undef REF_CONSTANT
#define REF_CONSTANT(C) \
	REF_Constant g_##C##_REF_Constant(#C, C)

// Automatically bind a constant to Lua with a custom name.
#undef REF_CONSTANT_EX
#define REF_CONSTANT_EX(C, name) \
	REF_Constant g_##C##_REF_Constant(#name, C)

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
		virtual const REF_Type* dereference_type() const override { return NULL; } \
		virtual const REF_Type* address_type() const override { return REF_GetType<T*>(); } \
	} g_##T##_Type; \
	const REF_Member T##_Type::members_data[] = { __VA_ARGS__ }; \
	static int T##_Type_members_data_sizeof() { return sizeof(T##_Type::members_data) / sizeof(*T##_Type::members_data); } \
	template <> struct REF_TypeGetter<T> { static const REF_Type* get() { return &g_##T##_Type; } }; \
	REF_PTR_TYPE(T); \
	REF_PTR_PTR_TYPE(T)

// Expose a flattened struct to the reflection system. The struct will have no
// keys when sent to Lua and use an indexed array instead. This is much more optimized
// and convenient for e.g. math types in Lua.
#undef REF_FLAT_FLOATS
#define REF_FLAT_FLOATS(T) \
	struct T##_Type : public REF_Type \
	{ \
		static const int N = sizeof(T) / sizeof(float); \
		virtual const char* name() const override { return #T; } \
		virtual int size() const override { return sizeof(T); } \
		virtual double to_number(void* v) const override { return 0; } \
		virtual String to_string(void* v) const override { return String(); } \
		virtual void cast(void* to, void* from, const REF_Type* from_type) const override { assert(from_type == REF_GetType<T>()); *(T*)to = *(T*)from; } \
		virtual void cleanup(void* v) const override { } \
		virtual bool is_pointer() const override { return false; } \
		virtual const REF_Type* dereference_type() const override { return NULL; } \
		virtual const REF_Type* address_type() const override { return REF_GetType<T*>(); } \
		virtual void lua_set(lua_State* L, void* v) const override { for (int i = 0; i < N; ++i) lua_pushnumber(L, ((float*)v)[i]); } \
		virtual void lua_get(lua_State* L, int index, void* v) const override { for (int i = 0; i < N; ++i) ((float*)v)[i] = (float)lua_tonumber(L, index + i); } \
		virtual int flattened_count() const override { return N; } \
	} g_##T##_Type; \
	template <> struct REF_TypeGetter<T> { static const REF_Type* get() { return &g_##T##_Type; } }; \
	REF_PTR_TYPE(T); \
	REF_PTR_PTR_TYPE(T)

#undef REF_FLAT_INTS
#define REF_FLAT_INTS(T) \
	struct T##_Type : public REF_Type \
	{ \
		static const int N = sizeof(T) / sizeof(int); \
		virtual const char* name() const override { return #T; } \
		virtual int size() const override { return sizeof(T); } \
		virtual double to_number(void* v) const override { return 0; } \
		virtual String to_string(void* v) const override { return String(); } \
		virtual void cast(void* to, void* from, const REF_Type* from_type) const override { assert(from_type == REF_GetType<T>()); *(T*)to = *(T*)from; } \
		virtual void cleanup(void* v) const override { } \
		virtual bool is_pointer() const override { return false; } \
		virtual const REF_Type* dereference_type() const override { return NULL; } \
		virtual const REF_Type* address_type() const override { return REF_GetType<T*>(); } \
		virtual void lua_set(lua_State* L, void* v) const override { for (int i = 0; i < N; ++i) lua_pushinteger(L, ((int*)v)[i]); } \
		virtual void lua_get(lua_State* L, int index, void* v) const override { for (int i = 0; i < N; ++i) ((int*)v)[i] = (int)lua_tointeger(L, index + i); } \
		virtual int flattened_count() const override { return N; } \
	} g_##T##_Type; \
	template <> struct REF_TypeGetter<T> { static const REF_Type* get() { return &g_##T##_Type; } }; \
	REF_PTR_TYPE(T); \
	REF_PTR_PTR_TYPE(T)

// Expose a member of a struct to the reflection system.
#undef REF_MEMBER
#define REF_MEMBER(m) { #m, CF_OFFSET_OF(Type, m), REF_GetType<decltype(((Type*)0)->m)>(), NULL, 0, NULL, false }

// Expose an array member of a struct to the reflection system.
#undef REF_MEMBER_ARRAY
#define REF_MEMBER_ARRAY(m, count) \
	{ #m, CF_OFFSET_OF(Type, m), REF_GetType<decltype(((Type*)0)->m)>(), \
	  #count, CF_OFFSET_OF(Type, count), REF_GetType<decltype(((Type*)0)->count)>(), !std::is_array<decltype(((Type*)0)->m)>::value }

// Represents a global variable, to be automatically bound to Lua, and sync'd
// whenever REF_SyncGlobals is called.
struct REF_Global : REF_List<REF_Global>
{
	template <typename T>
	REF_Global(const char* name, T* t)
		: name(name)
		, var(*t)
	{
	}
	const char* name;
	REF_Variable var;
};

// Expose a global variable to the reflection system.
#undef REF_GLOBAL
#define REF_GLOBAL(G) \
	REF_Global g_##G(#G, &G)

// Expose a global variable to the reflection system with a custom name.
#undef REF_GLOBAL_EX
#define REF_GLOBAL_EX(G, name) \
	REF_Global g_##G(#name, &G)

// Sync all global variable values to Lua.
int REF_SyncGlobals(lua_State* L)
{
	// Sync all globals.
	for (const REF_Global* g = REF_Global::head(); g; g = g->next) {
		g->var.type->lua_set(L, g->var.v);
		lua_setglobal(L, g->name);
	}
	return 0;
}

// Bind a manually wrapped Lua function. This must be a function like so: int func(lua_State* L)
// The "wrap_" prefix is automatically removed from the bound name.
#undef REF_WRAP_MANUAL
#define REF_WRAP_MANUAL(F) \
	REF_WrapBinder g_##F##_WrapManual(#F, F)

// Facilitates automatic binding of manually wrapped functions.
// Strips off "wrap_" prefix for the name of the function in Lua.
struct REF_WrapBinder : REF_List<REF_WrapBinder>
{
	REF_WrapBinder(const char* name, int (*fn)(lua_State*))
	{
		String s = name;
		if (s.prefix("wrap_")) {
			s.erase(0, 5); 
		}
		this->fn = fn;
		this->name = sintern(s);
	}
	const char* name;
	int (*fn)(lua_State*);
};

// Bind everything to Lua.
void REF_BindLua(lua_State* L)
{
	// Bind all constants.
	for (const REF_Constant* c = REF_Constant::head(); c; c = c->next) {
		c->type->lua_set(L, (void*)&c->constant);
		lua_setglobal(L, c->name);
	}

	// Bind all functions.
	for (const REF_Function* fn = REF_Function::head(); fn; fn = fn->next) {
		lua_pushlightuserdata(L, (void*)fn);
		lua_pushcclosure(L, REF_LuaCFunction, 1);
		lua_setglobal(L, fn->name());
	}

	// Bind all manually wrapped functions.
	for (const REF_WrapBinder* w = REF_WrapBinder::head(); w; w = w->next) {
		lua_pushcfunction(L, w->fn);
		lua_setglobal(L, w->name);
	}

	// Manually bind a stub error handling function.
	luaL_dostring(L, "function REF_ErrorHandler(error_text) print(error_text)\n os.exit(-1) end");

	// Bind all globals.
	REF_SyncGlobals(L);
}

// Make this callable from Lua.
REF_WRAP_MANUAL(REF_SyncGlobals);
