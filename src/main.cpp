#include <cute.h>
using namespace Cute;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <bind.h>

// -------------------------------------------------------------------------------------------------
// Utilities.

// For debugging.
void PrintLuaStack(lua_State *L)
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


int LuaCall(lua_State* L, int nargs, int nresults)
{
	int base = lua_gettop(L) - nargs;
	lua_pushcfunction(L, [](lua_State* L) {
		const char* msg = lua_tostring(L, 1);
		if (msg) {
			fprintf(stdout, "Lua error: %s\n", msg);
		} else {
			fprintf(stdout, "Unknown Lua error occurred.\n");
		}
		return 1;
	});
	lua_insert(L, base);
	int status = lua_pcall(L, nargs, nresults, base);
	lua_remove(L, base);
	return status;
}

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

// Explicitly register v2 as two flattened floats. This is a lot more efficient
// than handling vectors as tables in Lua.
struct v2_Type : public REF_Type
{
	virtual const char* name() const { return "v2"; }
	virtual int size() const { return sizeof(v2); }
	virtual double to_number(void* v) const { return 0; }
	virtual String to_string(void* v) const { return String(); }

	virtual void cast(void* to, void* from, const REF_Type* from_type) const { assert(from_type == REF_GetType<v2>()); *(v2*)to = *(v2*)from; }
	virtual void cleanup(void* v) const { }
	virtual void lua_set(lua_State* L, void* v) const { lua_pushnumber(L, ((v2*)v)->x); lua_pushnumber(L, ((v2*)v)->y); }
	virtual void lua_get(lua_State* L, int index, void* v) const { ((v2*)v)->x = (float)lua_tonumber(L, index); ((v2*)v)->y = (float)lua_tonumber(L, index + 1); }
	virtual int lua_flatten_count() const { return 2; }
} g_v2_Type;
template <> struct REF_TypeGetter<v2> { static const REF_Type* get() { return &g_v2_Type; } };

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
// Runtime wrapper, name -> vtable map
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

REF_FUNCTION_EX(draw_sprite, cf_draw_sprite);
REF_FUNCTION_EX(sprite_update, cf_sprite_update);
REF_FUNCTION_EX(sprite_reset, cf_sprite_reset);
REF_FUNCTION_EX(sprite_play, cf_sprite_play);
REF_FUNCTION_EX(sprite_is_playing, cf_sprite_is_playing);
REF_FUNCTION_EX(sprite_pause, cf_sprite_pause);
REF_FUNCTION_EX(sprite_unpause, cf_sprite_unpause);
REF_FUNCTION_EX(sprite_toggle_pause, cf_sprite_toggle_pause);
REF_FUNCTION_EX(sprite_flip_x, cf_sprite_flip_x);
REF_FUNCTION_EX(sprite_flip_y, cf_sprite_flip_y);
REF_FUNCTION_EX(sprite_frame_count, cf_sprite_frame_count);
REF_FUNCTION_EX(sprite_current_frame, cf_sprite_current_frame);
REF_FUNCTION_EX(sprite_frame_delay, cf_sprite_frame_delay);
REF_FUNCTION_EX(sprite_animation_delay, cf_sprite_animation_delay);
REF_FUNCTION_EX(sprite_animation_interpolant, cf_sprite_animation_interpolant);
REF_FUNCTION_EX(sprite_will_finish, cf_sprite_will_finish);
REF_FUNCTION_EX(sprite_on_loop, cf_sprite_on_loop);
REF_FUNCTION_EX(sprite_width, cf_sprite_width);
REF_FUNCTION_EX(sprite_height, cf_sprite_height);
REF_FUNCTION_EX(sprite_get_scale_x, cf_sprite_get_scale_x);
REF_FUNCTION_EX(sprite_get_scale_y, cf_sprite_get_scale_y);
REF_FUNCTION_EX(sprite_set_scale, cf_sprite_set_scale);
REF_FUNCTION_EX(sprite_set_scale_x, cf_sprite_set_scale_x);
REF_FUNCTION_EX(sprite_set_scale_y, cf_sprite_set_scale_y);
REF_FUNCTION_EX(sprite_get_offset_x, cf_sprite_get_offset_x);
REF_FUNCTION_EX(sprite_get_offset_y, cf_sprite_get_offset_y);
REF_FUNCTION_EX(sprite_set_offset_x, cf_sprite_set_offset_x);
REF_FUNCTION_EX(sprite_set_offset_y, cf_sprite_set_offset_y);
REF_FUNCTION_EX(sprite_get_opacity, cf_sprite_get_opacity);
REF_FUNCTION_EX(sprite_set_opacity, cf_sprite_set_opacity);
REF_FUNCTION_EX(sprite_get_play_speed_multiplier, cf_sprite_get_play_speed_multiplier);
REF_FUNCTION_EX(sprite_set_play_speed_multiplier, cf_sprite_set_play_speed_multiplier);
REF_FUNCTION_EX(sprite_get_loop_count, cf_sprite_get_loop_count);
// The rest of sprite stuff bound manually below.

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
// draw_polyine - Manually bound below.
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
// Joypad

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
// Noise

// @TODO Consider? Not sure how to handle CF_Pixel
// -> Flatten the pixel to ints

REF_HANDLE_TYPE(CF_Noise);

REF_FUNCTION(make_noise);

// -------------------------------------------------------------------------------------------------
// Time

// @TODO Global variables
// @TODO How to deal with callbacks

// -------------------------------------------------------------------------------------------------
// Version

REF_CONSTANT(CF_VERSION_STRING_COMPILED);
REF_FUNCTION(version_string_linked);

// -------------------------------------------------------------------------------------------------
// Manually bind certain, difficult to automate, functions.

int wrap_draw_polyine(lua_State* L)
{
	v2* pts;
	int count;
	float thickness;
	bool loop;
	REF_LuaGetArray(L, 1, &pts, &count);
	REF_GetType<float>()->lua_get(L, 2, &thickness);
	REF_GetType<bool>()->lua_get(L, 3, &loop);
	lua_pop(L, 3);
	draw_polyline(pts, count, thickness, loop);
	cf_free(pts);
	return 0;
}

CF_MemoryPool* g_sprite_pool;
int wrap_make_demo_sprite(lua_State* L)
{
	if (!g_sprite_pool) g_sprite_pool = make_memory_pool(sizeof(CF_Sprite), 1024 * 1024, 8);
	CF_Sprite* s = (CF_Sprite*)memory_pool_alloc(g_sprite_pool);
	*s = cf_make_demo_sprite();
	REF_GetType<CF_Sprite*>()->lua_set(L, &s);
	return 1;
}

int wrap_make_sprite(lua_State* L)
{
	if (!g_sprite_pool) g_sprite_pool = make_memory_pool(sizeof(CF_Sprite), 1024 * 1024, 8);
	char* path;
	REF_GetType<char*>()->lua_get(L, -1, &path);
	lua_pop(L, 1);
	CF_Sprite* s = (CF_Sprite*)memory_pool_alloc(g_sprite_pool);
	*s = make_sprite(path);
	REF_GetType<CF_Sprite*>()->lua_set(L, &s);
	return 1;
}

int wrap_destroy_sprite(lua_State* L)
{
	CF_Sprite* s;
	REF_GetType<CF_Sprite*>()->lua_get(L, -1, &s);
	lua_pop(L, 1);
	memory_pool_free(g_sprite_pool, s);
	return 0;
}

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
	PrintLuaStack(L);
	REF_GetType<char*>()->lua_get(L, -1, &name_ptr);
	lua_pop(L, 1);
	String name = String(name_ptr) + "_shader";
	CF_Shader shd = g_shaders.find(sintern(name));
	assert(shd.id);
	REF_GetType<CF_Shader>()->lua_set(L, &shd);
	return 1;
}

// -------------------------------------------------------------------------------------------------
// Main

void ManuallyBindFunctions(lua_State* L)
{
	lua_pushcfunction(L, wrap_draw_polyine);
	lua_setglobal(L, "draw_polyline");
	lua_pushcfunction(L, wrap_make_shader);
	lua_setglobal(L, "make_shader");
	lua_pushcfunction(L, wrap_make_demo_sprite);
	lua_setglobal(L, "make_demo_sprite");
	lua_pushcfunction(L, wrap_make_sprite);
	lua_setglobal(L, "make_sprite");
	lua_pushcfunction(L, wrap_destroy_sprite);
	lua_setglobal(L, "destroy_sprite");
}

int main(int argc, char* argv[])
{
	make_app("Fancy Window Title", 0, 0, 640, 480, APP_OPTIONS_WINDOW_POS_CENTERED, argv[0]);
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	REF_BindLua(L);
	ManuallyBindFunctions(L);
	LoadShaders();

	if (luaL_loadfile(L, "../../src/main.lua")) {
		fprintf(stderr, lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}
	LuaCall(L, 0, 0);
	lua_close(L);

	return 0;
}
