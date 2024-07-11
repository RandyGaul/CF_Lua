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
	virtual void lua_set(lua_State* L, void* v) const { lua_pushlightuserdata(L, *(void**)v);; }
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
		assert(lua_istable(L, index));
		int count = member_count();
		const REF_Member* mptr = members();
		for (int i = 0; i < count; ++i) {
			const REF_Member* m = mptr + i;
			lua_pushstring(L, m->name);
			lua_gettable(L, index);
			m->type->lua_get(L, -1, (char*)v + m->offset);
			lua_pop(L, 1);
		}
	}
};

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

#define REF_MEMBER(m) { #m, CF_OFFSET_OF(Type, m), REF_GetType<decltype(((Type*)0)->m)>() }

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
	for (int i = 0; i < param_count; i++) {
		params[i].type->lua_get(L, i + 1, params[i].v);
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

// Call this from `main`.
// Binds all functions registered with REF_BIND_FUNCTION to Lua.
void REF_BindFunctions(lua_State* L)
{
	for (const REF_Function* fn = REF_Function::head(); fn; fn = fn->next) {
		lua_pushlightuserdata(L, (void*)fn);
		lua_pushcclosure(L, REF_LuaCFunction, 1);
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

// Call this from your `main`, binds all constants to Lua.
void REF_BindAllConstants(lua_State* L)
{
	for (const REF_Constant* c = REF_Constant::head(); c; c = c->next) {
		c->type->lua_set(L, (void*)&c->constant);
		lua_setglobal(L, c->name);
	}
}

// Treat handles as void* within the reflection sytsem.
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
#define REF_FUNCTION(F) \
	REF_Function g_##F##_REF_Function(#F, F)

// Expose a function to the reflection system.
// It will be automatically bound to Lua with a custom name.
#define REF_FUNCTION_EX(name, F) \
	REF_Function g_##name##_REF_Function(#name, F)

// Automatically bind a constant to Lua.
#define REF_CONSTANT(C) \
	REF_Constant g_##C##_REF_Constant(#C, C)

// Automatically bind an enum to Lua as a bunch of constants.
#define CF_ENUM(K, V) REF_CONSTANT(K);

// -------------------------------------------------------------------------------------------------
// App

REF_STRUCT(CF_Result,
	REF_MEMBER(code),
	REF_MEMBER(details)
);

CF_RESULT_DEFS
CF_MESSAGE_BOX_TYPE_DEFS

REF_FUNCTION(is_error);
REF_FUNCTION(message_box);

// -------------------------------------------------------------------------------------------------
// Math

REF_STRUCT(v2,
	REF_MEMBER(x),
	REF_MEMBER(y)
);

REF_STRUCT(M2x2,
	REF_MEMBER(x),
	REF_MEMBER(y)
);

REF_STRUCT(M3x2,
	REF_MEMBER(m),
	REF_MEMBER(p)
);

// -------------------------------------------------------------------------------------------------
// Graphics

CF_BACKEND_TYPE_DEFS
CF_PIXELFORMAT_DEFS
CF_PIXELFORMAT_OP_DEFS
CF_DEVICE_FEATURE_DEFS
CF_RESOURCE_LIMIT_DEFS
CF_USAGE_TYPE_DEFS
CF_FILTER_DEFS
CF_WRAP_MODE_DEFS
CF_VERTEX_FORMAT_DEFS
CF_ATTRIBUTE_STEP_DEFS
CF_CULL_MODE_DEFS
CF_COMPARE_FUNCTION_DEFS
CF_STENCIL_OP_DEFS
CF_BLEND_OP_DEFS
CF_BLEND_FACTOR_DEFS
CF_UNIFORM_TYPE_DEFS

REF_HANDLE_TYPE(CF_Texture);
REF_HANDLE_TYPE(CF_Canvas);
REF_HANDLE_TYPE(CF_Mesh);
REF_HANDLE_TYPE(CF_Material);
REF_HANDLE_TYPE(CF_Shader);

REF_STRUCT(CF_TextureParams,
	REF_MEMBER(pixel_format),
	REF_MEMBER(usage),
	REF_MEMBER(filter),
	REF_MEMBER(wrap_u),
	REF_MEMBER(wrap_v),
	REF_MEMBER(width),
	REF_MEMBER(height),
	REF_MEMBER(render_target),
	REF_MEMBER(initial_data_size),
	REF_MEMBER(initial_data),
);

REF_STRUCT(CF_CanvasParams,
	REF_MEMBER(target),
	REF_MEMBER(depth_stencil_target),
);

REF_STRUCT(CF_VertexAttribute,
	REF_MEMBER(name),
	REF_MEMBER(format),
	REF_MEMBER(offset),
	REF_MEMBER(step_type),
);

REF_STRUCT(CF_StencilFunction,
	REF_MEMBER(compare),
	REF_MEMBER(fail_op),
	REF_MEMBER(depth_fail_op),
	REF_MEMBER(pass_op),
);

REF_STRUCT(CF_StencilParams,
	REF_MEMBER(enabled),
	REF_MEMBER(read_mask),
	REF_MEMBER(write_mask),
	REF_MEMBER(reference),
	REF_MEMBER(front),
	REF_MEMBER(back),
);

REF_STRUCT(CF_BlendState,
	REF_MEMBER(enabled),
	REF_MEMBER(pixel_format),
	REF_MEMBER(write_R_enabled),
	REF_MEMBER(write_G_enabled),
	REF_MEMBER(write_B_enabled),
	REF_MEMBER(write_A_enabled),
	REF_MEMBER(rgb_op),
	REF_MEMBER(rgb_src_blend_factor),
	REF_MEMBER(rgb_dst_blend_factor),
	REF_MEMBER(alpha_op),
	REF_MEMBER(alpha_src_blend_factor),
	REF_MEMBER(alpha_dst_blend_factor),
);

REF_STRUCT(CF_RenderState,
	REF_MEMBER(cull_mode),
	REF_MEMBER(blend),
	REF_MEMBER(depth_compare),
	REF_MEMBER(depth_write_enabled),
	REF_MEMBER(stencil),
);

REF_FUNCTION(query_backend);
REF_FUNCTION(query_pixel_format);
REF_FUNCTION(query_device_feature);
REF_FUNCTION(query_resource_limit);
REF_FUNCTION(texture_defaults);
REF_FUNCTION(make_texture);
REF_FUNCTION(destroy_texture);
REF_FUNCTION(update_texture);
// @TODO
//REF_FUNCTION(make_shader);
REF_FUNCTION(destroy_shader);
REF_FUNCTION(canvas_defaults);
REF_FUNCTION(make_canvas);
REF_FUNCTION(destroy_canvas);
REF_FUNCTION(canvas_get_target);
REF_FUNCTION(canvas_get_depth_stencil_target);
REF_FUNCTION(canvas_get_backend_target_handle);
REF_FUNCTION(canvas_get_backend_depth_stencil_handle);
REF_FUNCTION(canvas_blit);
REF_FUNCTION(make_mesh);
REF_FUNCTION(destroy_mesh);
REF_FUNCTION(mesh_set_attributes);
REF_FUNCTION(mesh_update_vertex_data);
REF_FUNCTION(mesh_append_vertex_data);
REF_FUNCTION(mesh_will_overflow_vertex_data);
REF_FUNCTION(mesh_update_instance_data);
REF_FUNCTION(mesh_append_instance_data);
REF_FUNCTION(mesh_will_overflow_instance_data);
REF_FUNCTION(mesh_update_index_data);
REF_FUNCTION(mesh_append_index_data);
REF_FUNCTION(mesh_will_overflow_index_data);
REF_FUNCTION(render_state_defaults);
REF_FUNCTION(make_material);
REF_FUNCTION(destroy_material);
REF_FUNCTION(material_set_render_state);
REF_FUNCTION(material_set_texture_vs);
REF_FUNCTION(material_set_texture_fs);
REF_FUNCTION(material_clear_textures);
REF_FUNCTION(material_set_uniform_vs);
REF_FUNCTION(material_set_uniform_fs);
REF_FUNCTION(material_clear_uniforms);
REF_FUNCTION(apply_canvas);
REF_FUNCTION(apply_viewport);
REF_FUNCTION(apply_scissor);
REF_FUNCTION(apply_mesh);
REF_FUNCTION(apply_shader);
REF_FUNCTION(draw_elements);
REF_FUNCTION(unapply_canvas);
REF_FUNCTION_EX(clear_color, cf_clear_color);

// -------------------------------------------------------------------------------------------------
// App

CF_APP_OPTION_DEFS
CF_POWER_STATE_DEFS

REF_FUNCTION(make_app);
REF_FUNCTION(destroy_app);
REF_FUNCTION(app_is_running);
REF_FUNCTION(app_signal_shutdown);
REF_FUNCTION(app_update);
REF_FUNCTION(app_draw_onto_screen);
REF_FUNCTION(app_get_width);
REF_FUNCTION(app_get_height);
v2 wrap_app_get_position() { int x, y; app_get_position(&x, &y); return V2((float)x, (float)y); }
REF_FUNCTION_EX(app_get_position, wrap_app_get_position);
REF_FUNCTION(app_set_position);
v2 wrap_app_get_size() { int x, y; app_get_size(&x, &y); return V2((float)x, (float)y); }
REF_FUNCTION(app_set_size);
REF_FUNCTION_EX(app_get_size, wrap_app_get_size);
REF_FUNCTION(app_get_dpi_scale);
REF_FUNCTION(app_dpi_scaled_was_changed);
REF_FUNCTION(app_was_resized);
REF_FUNCTION(app_was_moved);
REF_FUNCTION(app_lost_focus);
REF_FUNCTION(app_gained_focus);
REF_FUNCTION(app_has_focus);
REF_FUNCTION(app_was_minimized);
REF_FUNCTION(app_was_maximized);
REF_FUNCTION(app_minimized);
REF_FUNCTION(app_maximized);
REF_FUNCTION(app_was_restored);
REF_FUNCTION(app_mouse_entered);
REF_FUNCTION(app_mouse_exited);
REF_FUNCTION(app_mouse_inside);
REF_FUNCTION(app_get_canvas_width);
REF_FUNCTION(app_get_canvas_height);
REF_FUNCTION(app_set_vsync);
REF_FUNCTION(app_get_vsync);
REF_FUNCTION(app_init_imgui);
REF_FUNCTION(app_get_canvas);
REF_FUNCTION(app_set_canvas_size);
CF_PowerState app_get_power_state() { CF_PowerInfo info = app_power_info(); return info.state; }
int app_get_power_seconds_left() { CF_PowerInfo info = app_power_info(); return info.seconds_left; }
int app_get_power_percentage_left() { CF_PowerInfo info = app_power_info(); return info.percentage_left; }
REF_FUNCTION(app_get_power_seconds_left);
REF_FUNCTION(app_get_power_percentage_left);

// -------------------------------------------------------------------------------------------------
// Audio

REF_HANDLE_TYPE(CF_Audio);
REF_FUNCTION(audio_load_ogg);
REF_FUNCTION(audio_load_wav);
REF_FUNCTION(audio_load_ogg_from_memory);
REF_FUNCTION(audio_load_wav_from_memory);
REF_FUNCTION(audio_destroy);
REF_FUNCTION(audio_sample_rate);
REF_FUNCTION(audio_sample_count);
REF_FUNCTION(audio_channel_count);
REF_FUNCTION(audio_set_pan);
REF_FUNCTION(audio_set_global_volume);
REF_FUNCTION(audio_set_sound_volume);
REF_FUNCTION(audio_set_pause);
REF_FUNCTION(music_play);
REF_FUNCTION(music_stop);
REF_FUNCTION(play_music);
REF_FUNCTION(stop_music);
REF_FUNCTION(music_set_volume);
REF_FUNCTION(music_set_loop);
REF_FUNCTION(music_set_pitch);
REF_FUNCTION(music_pause);
REF_FUNCTION(music_resume);
REF_FUNCTION(music_switch_to);
REF_FUNCTION(music_crossfade);
REF_FUNCTION(music_set_sample_index);
REF_FUNCTION(music_get_sample_index);

REF_STRUCT(SoundParams,
	REF_MEMBER(paused),
	REF_MEMBER(looped),
	REF_MEMBER(volume),
	REF_MEMBER(pan),
	REF_MEMBER(pitch),
);

REF_HANDLE_TYPE(Sound);

REF_FUNCTION(play_sound);
REF_FUNCTION(sound_is_active);
REF_FUNCTION(sound_get_is_paused);
REF_FUNCTION(sound_get_is_looped);
REF_FUNCTION(sound_get_volume);
REF_FUNCTION(sound_get_sample_index);
REF_FUNCTION(sound_set_is_paused);
REF_FUNCTION(sound_set_is_looped);
REF_FUNCTION(sound_set_volume);
REF_FUNCTION(sound_set_pitch);
REF_FUNCTION(sound_set_sample_index);
REF_FUNCTION(sound_stop);

// -------------------------------------------------------------------------------------------------
// Clipboard

REF_FUNCTION(clipboard_get);
REF_FUNCTION(clipboard_set);

// -------------------------------------------------------------------------------------------------
// Draw

REF_STRUCT(CF_Rect,
	REF_MEMBER(w),
	REF_MEMBER(h),
	REF_MEMBER(x),
	REF_MEMBER(y),
);

REF_STRUCT(CF_Color,
	REF_MEMBER(r),
	REF_MEMBER(g),
	REF_MEMBER(b),
	REF_MEMBER(a),
);

REF_STRUCT(CF_Pixel,
	REF_MEMBER(val),
);

// @TODO Sprite
void wrap_draw_quad(v2 min, v2 max, float thickness, float chubbiness) { draw_quad(make_aabb(min, max), thickness, chubbiness); }
REF_FUNCTION_EX(draw_quad, wrap_draw_quad);
void wrap_draw_quad_fill(v2 min, v2 max, float chubbiness) { draw_quad_fill(make_aabb(min, max), chubbiness); }
REF_FUNCTION_EX(draw_quad_fill, wrap_draw_quad_fill);
REF_FUNCTION_EX(draw_box, wrap_draw_quad);
REF_FUNCTION_EX(draw_box_fill, wrap_draw_quad_fill);
REF_FUNCTION_EX(draw_circle, cf_draw_circle2);
REF_FUNCTION_EX(draw_circle_fill, cf_draw_circle_fill2);
REF_FUNCTION_EX(draw_capsule, cf_draw_capsule2);
REF_FUNCTION_EX(draw_capsule_fill, cf_draw_capsule_fill2);
REF_FUNCTION(draw_tri);
REF_FUNCTION(draw_tri_fill);
REF_FUNCTION(draw_line);
// @TODO draw_polyine
REF_FUNCTION_EX(draw_bezier_line, cf_draw_bezier_line2);
REF_FUNCTION(draw_arrow);

REF_FUNCTION(draw_push_layer);
REF_FUNCTION(draw_pop_layer);
REF_FUNCTION(draw_peek_layer);
REF_FUNCTION(draw_push_color);
REF_FUNCTION(draw_pop_color);
REF_FUNCTION(draw_peek_color);
REF_FUNCTION(draw_push_tint);
REF_FUNCTION(draw_pop_tint);
REF_FUNCTION(draw_peek_tint);
REF_FUNCTION(draw_push_antialias);
REF_FUNCTION(draw_pop_antialias);
REF_FUNCTION(draw_peek_antialias);
REF_FUNCTION(draw_push_antialias_scale);
REF_FUNCTION(draw_pop_antialias_scale);
REF_FUNCTION(draw_peek_antialias_scale);
REF_FUNCTION_EX(draw_push_vertex_attributes, cf_draw_push_vertex_attributes2);
REF_FUNCTION(draw_pop_vertex_attributes);
REF_FUNCTION(draw_peek_vertex_attributes);

REF_FUNCTION(make_font);
REF_FUNCTION(make_font_from_memory);
REF_FUNCTION(destroy_font);
REF_FUNCTION(push_font);
REF_FUNCTION(pop_font);
REF_FUNCTION(peek_font);
REF_FUNCTION(push_font_size);
REF_FUNCTION(pop_font_size);
REF_FUNCTION(peek_font_size);
REF_FUNCTION(push_font_blur);
REF_FUNCTION(pop_font_blur);
REF_FUNCTION(peek_font_blur);
REF_FUNCTION(push_text_wrap_width);
REF_FUNCTION(pop_text_wrap_width);
REF_FUNCTION(peek_text_wrap_width);
// @TODO REF_FUNCTION(push_text_clip_box);
// @TODO REF_FUNCTION(pop_text_clip_box);
// @TODO REF_FUNCTION(peek_text_clip_box);
REF_FUNCTION(text_width);
REF_FUNCTION(text_height);
REF_FUNCTION(text_size);
REF_FUNCTION(draw_text);

// @TODO Text FX

REF_FUNCTION(render_settings_filter);
REF_FUNCTION(render_settings_push_viewport);
REF_FUNCTION(render_settings_pop_viewport);
REF_FUNCTION(render_settings_peek_viewport);
REF_FUNCTION(render_settings_push_scissor);
REF_FUNCTION(render_settings_pop_scissor);
REF_FUNCTION(render_settings_peek_scissor);
REF_FUNCTION(render_settings_push_render_state);
REF_FUNCTION(render_settings_pop_render_state);
REF_FUNCTION(render_settings_peek_render_state);
REF_FUNCTION(render_settings_push_shader);
REF_FUNCTION(render_settings_pop_shader);
REF_FUNCTION(render_settings_peek_shader);
REF_FUNCTION(render_settings_push_texture);

REF_FUNCTION(cf_render_settings_push_uniform);
REF_FUNCTION_EX(render_settings_push_uniform_int, cf_render_settings_push_uniform_int);
REF_FUNCTION_EX(render_settings_push_uniform_float, cf_render_settings_push_uniform_float);
REF_FUNCTION_EX(render_settings_push_uniform_v2, cf_render_settings_push_uniform_v2);
REF_FUNCTION_EX(render_settings_push_uniform_color, cf_render_settings_push_uniform_color);

REF_FUNCTION_EX(draw_scale, cf_draw_scale);
REF_FUNCTION_EX(draw_translate, cf_draw_translate);
REF_FUNCTION(draw_rotate);
REF_FUNCTION(draw_TRS);
REF_FUNCTION(draw_TRS_absolute);
REF_FUNCTION(draw_push);
REF_FUNCTION(draw_pop);
REF_FUNCTION(draw_peek);
REF_FUNCTION(draw_projection);
REF_FUNCTION(world_to_screen);
REF_FUNCTION(screen_to_world);

REF_FUNCTION(render_to);

// @TODO fetch_image

// -------------------------------------------------------------------------------------------------
// haptics

CF_HAPTIC_TYPE_DEFS
CF_HAPTIC_WAVE_TYPE_DEFS

REF_HANDLE_TYPE(CF_Haptic);
REF_HANDLE_TYPE(CF_Joypad);
REF_HANDLE_TYPE(CF_HapticEffect);

REF_STRUCT(CF_HapticEnvelope,
	REF_MEMBER(attack),
	REF_MEMBER(attack_milliseconds),
	REF_MEMBER(fade),
	REF_MEMBER(fade_milliseconds),
);

REF_STRUCT(CF_HapticLeftRight,
	REF_MEMBER(duration_milliseconds),
	REF_MEMBER(lo_motor_strength),
	REF_MEMBER(hi_motor_strength),
);

REF_STRUCT(CF_HapticPeriodic,
	REF_MEMBER(wave_type),
	REF_MEMBER(duration_milliseconds),
	REF_MEMBER(period_milliseconds),
	REF_MEMBER(magnitude),
	REF_MEMBER(envelope),
);

REF_STRUCT(CF_HapticRamp,
	REF_MEMBER(duration_milliseconds),
	REF_MEMBER(start),
	REF_MEMBER(end),
	REF_MEMBER(envelope),
);

REF_STRUCT(CF_HapticData,
	REF_MEMBER(type),
	REF_MEMBER(u.leftright),
	REF_MEMBER(u.periodic),
	REF_MEMBER(u.ramp),
);

REF_FUNCTION(haptic_open);
REF_FUNCTION(haptic_close);
REF_FUNCTION(haptic_supports);
REF_FUNCTION(haptic_set_gain);
REF_FUNCTION(haptic_create_effect);
REF_FUNCTION(haptic_run_effect);
REF_FUNCTION(haptic_update_effect);
REF_FUNCTION(haptic_stop_effect);
REF_FUNCTION(haptic_destroy_effect);
REF_FUNCTION(haptic_pause);
REF_FUNCTION(haptic_unpause);
REF_FUNCTION(haptic_stop_all);
REF_FUNCTION(haptic_rumble_supported);
REF_FUNCTION(haptic_rumble_play);
REF_FUNCTION(haptic_rumble_stop);

// -------------------------------------------------------------------------------------------------
// https

CF_HTTPS_RESULT_DEFS

REF_HANDLE_TYPE(CF_HttpsRequest);
REF_HANDLE_TYPE(CF_HttpsResponse);

REF_STRUCT(CF_HttpsHeader,
	REF_MEMBER(name),
	REF_MEMBER(value),
);

REF_FUNCTION(https_get);
REF_FUNCTION(https_post);
REF_FUNCTION(https_add_header);
REF_FUNCTION(https_destroy);
REF_FUNCTION(https_process);
REF_FUNCTION(https_response);
REF_FUNCTION(https_response_code);
REF_FUNCTION(https_response_content_length);
REF_FUNCTION(https_response_content);
REF_FUNCTION(https_response_find_header);
REF_FUNCTION(https_response_headers_count);
REF_FUNCTION(https_response_headers);

// -------------------------------------------------------------------------------------------------
// Input

CF_KEY_BUTTON_DEFS
CF_MOUSE_BUTTON_DEFS

REF_FUNCTION(key_down);
REF_FUNCTION(key_just_pressed);
REF_FUNCTION(key_just_released);
REF_FUNCTION(key_repeating);
REF_FUNCTION(key_ctrl);
REF_FUNCTION(key_shift);
REF_FUNCTION(key_alt);
REF_FUNCTION(key_gui);
REF_FUNCTION(clear_key_states);

REF_FUNCTION(mouse_x);
REF_FUNCTION(mouse_y);
REF_FUNCTION(mouse_down);
REF_FUNCTION(mouse_just_pressed);
REF_FUNCTION(mouse_just_released);
REF_FUNCTION(mouse_wheel_motion);
REF_FUNCTION(mouse_double_click_held);
REF_FUNCTION(mouse_double_clicked);
REF_FUNCTION(mouse_hide);
REF_FUNCTION(mouse_hidden);
REF_FUNCTION(mouse_lock_inside_window);

REF_FUNCTION(input_text_add_utf8);
REF_FUNCTION(input_text_pop_utf32);
REF_FUNCTION(input_text_has_data);
REF_FUNCTION(input_text_clear);

REF_FUNCTION(input_enable_ime);
REF_FUNCTION(input_disable_ime);
REF_FUNCTION(input_is_ime_enabled);
REF_FUNCTION(input_has_ime_keyboard_support);
REF_FUNCTION(input_is_ime_keyboard_shown);
REF_FUNCTION(input_set_ime_rect);
// @TODO input_get_ime_composition
// @TODO touch_get

// -------------------------------------------------------------------------------------------------
// joypad

CF_JOYPAD_POWER_LEVEL_DEFS
CF_JOYPAD_BUTTON_DEFS
CF_JOYPAD_AXIS_DEFS

REF_FUNCTION(joypad_add_mapping);
REF_FUNCTION(joypad_count);
REF_FUNCTION(joypad_open);
REF_FUNCTION(joypad_close);
REF_FUNCTION(joypad_is_connected);
REF_FUNCTION(joypad_power_level);
REF_FUNCTION(joypad_name);
REF_FUNCTION(joypad_button_down);
REF_FUNCTION(joypad_button_was_pressed);
REF_FUNCTION(joypad_button_was_released);
REF_FUNCTION(joypad_axis);

// -------------------------------------------------------------------------------------------------
// noise

// @TODO Consider? Not sure how to handle CF_Pixel

// -------------------------------------------------------------------------------------------------
// time

// @TODO Global variables
// @TODO How to deal with callbacks

// -------------------------------------------------------------------------------------------------
// version

REF_CONSTANT(CF_VERSION_STRING_COMPILED);
REF_FUNCTION(version_string_linked);

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
