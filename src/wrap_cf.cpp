#include <bind.h>

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

REF_OPAQUE_PTR_TYPE(ImGuiContext);
REF_OPAQUE_PTR_TYPE(CF_Sprite);
REF_OPAQUE_PTR_TYPE(CF_File);
REF_OPAQUE_PTR_TYPE(CF_Joypad);
REF_OPAQUE_PTR_TYPE(CF_Haptic);

// -------------------------------------------------------------------------------------------------
// Math

CF_SHAPE_TYPE_DEFS

REF_FLAT_FLOATS(v2);
REF_FLAT_FLOATS(CF_M2x2);
REF_FLAT_FLOATS(CF_M3x2);
REF_FLAT_FLOATS(CF_Aabb);
REF_FLAT_FLOATS(CF_Rect);
REF_FLAT_FLOATS(CF_Color);
REF_FLAT_FLOATS(CF_Circle);
REF_FLAT_FLOATS(CF_Capsule);
REF_FLAT_FLOATS(CF_Raycast);
REF_FLAT_FLOATS(CF_Ray);
REF_FLAT_FLOATS(CF_Halfspace);
REF_FLAT_FLOATS(CF_SinCos);
REF_FLAT_FLOATS(CF_Transform);

REF_STRUCT(CF_Poly,
	REF_MEMBER_ARRAY(verts, count),
	REF_MEMBER_ARRAY(norms, count),
);

REF_STRUCT(CF_SliceOutput,
	REF_MEMBER(front),
	REF_MEMBER(back),
);

REF_FUNCTION(ray_to_halfspace);
REF_FUNCTION(center_of_mass);
REF_FUNCTION(calc_area);
REF_FUNCTION(slice);
REF_FUNCTION(make_poly, {0,1});
REF_FUNCTION(circle_to_circle);
REF_FUNCTION(circle_to_aabb);
REF_FUNCTION(circle_to_capsule);
REF_FUNCTION(aabb_to_aabb);
REF_FUNCTION(aabb_to_capsule);
REF_FUNCTION(capsule_to_capsule);

bool wrap_circle_to_poly(CF_Circle A, CF_Poly B) { return circle_to_poly(A, &B, NULL); }
REF_FUNCTION_EX(circle_to_poly, wrap_circle_to_poly);
bool wrap_aabb_to_poly(CF_Aabb A, CF_Poly B) { return aabb_to_poly(A, &B, NULL); }
REF_FUNCTION_EX(aabb_to_poly, wrap_aabb_to_poly);
bool wrap_capsule_to_poly(CF_Capsule A, CF_Poly B) { return capsule_to_poly(A, &B, NULL); }
REF_FUNCTION_EX(capsule_to_poly, wrap_capsule_to_poly);
bool wrap_poly_to_poly(CF_Poly A, CF_Poly B) { return poly_to_poly(&A, NULL, &B, NULL); }
REF_FUNCTION_EX(poly_to_poly, wrap_poly_to_poly);

REF_FUNCTION(ray_to_circle);
REF_FUNCTION(ray_to_capsule);
REF_FUNCTION(ray_to_aabb);
REF_FUNCTION(ray_to_poly);

REF_STRUCT(CF_Manifold,
	REF_MEMBER(count),
	REF_MEMBER_ARRAY(depths, count),
	REF_MEMBER_ARRAY(contact_points, count),
	REF_MEMBER(n),
);

REF_FUNCTION(circle_to_circle_manifold);
REF_FUNCTION(circle_to_aabb_manifold);
REF_FUNCTION(circle_to_capsule_manifold);
REF_FUNCTION(aabb_to_aabb_manifold);
REF_FUNCTION(aabb_to_capsule_manifold);
REF_FUNCTION(capsule_to_capsule_manifold);
REF_FUNCTION(circle_to_poly_manifold);
REF_FUNCTION(aabb_to_poly_manifold);
REF_FUNCTION(capsule_to_poly_manifold);
REF_FUNCTION(poly_to_poly_manifold);

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
// make_shader manually bindings below.
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
// app_update -- Wrapped explicitly below.
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

REF_FLAT_INTS(CF_Pixel);

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

REF_FUNCTION_EX(draw_quad, cf_draw_quad);
REF_FUNCTION_EX(draw_quad_fill, cf_draw_quad_fill);
REF_FUNCTION_EX(draw_box, cf_draw_quad);
REF_FUNCTION_EX(draw_box_fill, cf_draw_quad_fill);
REF_FUNCTION_EX(draw_circle, cf_draw_circle);
REF_FUNCTION_EX(draw_circle_fill, cf_draw_circle_fill);
REF_FUNCTION_EX(draw_capsule, cf_draw_capsule);
REF_FUNCTION_EX(draw_capsule_fill, cf_draw_capsule_fill);
REF_FUNCTION(draw_tri);
REF_FUNCTION(draw_tri_fill);
REF_FUNCTION(draw_line);
REF_FUNCTION(draw_polyline, {0,1});
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

REF_FUNCTION(push_text_clip_box);
REF_FUNCTION(pop_text_clip_box);
REF_FUNCTION(peek_text_clip_box);

REF_FUNCTION(text_width);
REF_FUNCTION(text_height);
REF_FUNCTION(text_size);
REF_FUNCTION(draw_text);

REF_STRUCT(CF_TextEffect,
	REF_MEMBER(effect_name),
	REF_MEMBER(on_begin),
	REF_MEMBER(on_end),
	REF_MEMBER(character),
	REF_MEMBER(index_into_string),
	REF_MEMBER(index_into_effect),
	REF_MEMBER(glyph_count),
	REF_MEMBER(elapsed),
	REF_MEMBER(center),
	REF_MEMBER(q0),
	REF_MEMBER(q1),
	REF_MEMBER(w),
	REF_MEMBER(h),
	REF_MEMBER(color),
	REF_MEMBER(opacity),
	REF_MEMBER(xadvance),
	REF_MEMBER(visible),
	REF_MEMBER(font_size),
);

// Wrap custom text fx in Lua by mapping fx names to lua function names.
Map<const char*, const char*> g_fx_name_to_lua_fn_name;
bool wrap_text_fx_fn(TextEffect* fx)
{
	CF_TextEffect effect = *(CF_TextEffect*)fx;
	const char* lua_fn_name = g_fx_name_to_lua_fn_name.find(sintern(effect.effect_name));
	assert(lua_fn_name);
	bool keep_going;
	REF_CallLuaFunction(L, lua_fn_name, { keep_going, effect }, effect);

	// Keep old pointer around, as Lua overwrote it with a dynamic string.
	const char* old_name_ptr = fx->effect_name;
	*(CF_TextEffect*)fx = effect;
	fx->effect_name = old_name_ptr;
	sfree(effect.effect_name);

	return keep_going;
}
int wrap_text_effect_register(lua_State* L)
{
	const char* fx_name = sintern(lua_tostring(L, -2));
	const char* lua_fn_name = sintern(lua_tostring(L, -1));
	g_fx_name_to_lua_fn_name.add(fx_name, lua_fn_name);
	text_effect_register(fx_name, wrap_text_fx_fn);
	lua_pop(L, 2);
	return 0;
}
REF_WRAP_MANUAL(wrap_text_effect_register);

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

REF_STRUCT(CF_TemporaryImage,
	REF_MEMBER(tex),
	REF_MEMBER(w),
	REF_MEMBER(h),
	REF_MEMBER(u),
	REF_MEMBER(v),
);

REF_FUNCTION_EX(fetch_image, cf_fetch_image);

// -------------------------------------------------------------------------------------------------
// File I/O

CF_FILE_TYPE_DEFS

REF_STRUCT(CF_Stat,
	REF_MEMBER(type),
	REF_MEMBER(is_read_only),
	REF_MEMBER(size),
	REF_MEMBER(last_modified_time),
	REF_MEMBER(created_time),
	REF_MEMBER(last_accessed_time),
);

REF_FUNCTION(fs_get_base_directory);
REF_FUNCTION(fs_set_write_directory);
REF_FUNCTION(fs_get_user_directory);
REF_FUNCTION(fs_mount);
REF_FUNCTION(fs_dismount);

int wrap_fs_stat(lua_State* L)
{
	const char* path = lua_tostring(L, -1);
	CF_Stat stat;
	bool ok = !is_error(fs_stat(path, &stat));
	lua_pop(L, 1);
	if (ok) {
		REF_GetType<CF_Stat>()->lua_set(L, &stat);
		return 1;
	} else {
		return 0;
	}
}
REF_WRAP_MANUAL(wrap_fs_stat);

REF_FUNCTION(fs_create_file);
REF_FUNCTION(fs_open_file_for_write);
REF_FUNCTION(fs_open_file_for_append);
REF_FUNCTION(fs_open_file_for_read);
REF_FUNCTION(fs_close);
REF_FUNCTION(fs_remove);
REF_FUNCTION(fs_create_directory);

int wrap_fs_enumerate_directory(lua_State* L)
{
	const char* path = lua_tostring(L, -1);
	const char** list = cf_fs_enumerate_directory(path);
	lua_pop(L, 1);
	if (!list) return 0;
	lua_newtable(L);
	int idx = 1;
	for (const char** i = list; *i; ++i) {
		lua_pushstring(L, *i);
		lua_rawseti(L, -2, idx++);
	}
	cf_fs_free_enumerated_directory(list);
	return 1;
}
REF_WRAP_MANUAL(wrap_fs_enumerate_directory);

REF_FUNCTION(fs_file_exists);

int wrap_fs_read(lua_State* L)
{
	CF_File* file = (CF_File*)lua_touserdata(L, -2);
	size_t size = lua_tointeger(L, -1);
	char* buffer = (char*)cf_alloc(size + 1);
	buffer[size] = 0;
	size_t read = fs_read(file, buffer, size);
	lua_pop(L, 2);
	lua_pushstring(L, buffer);
	lua_pushinteger(L, read);
	cf_free(buffer);
	return 2;
}
REF_WRAP_MANUAL(wrap_fs_read);

int wrap_fs_write(lua_State* L)
{
	CF_File* file = (CF_File*)lua_touserdata(L, -3);
	const char* data = lua_tostring(L, -2);
	size_t size = lua_tointeger(L, -1);
	size_t written = fs_write(file, data, size);
	lua_pop(L, 3);
	lua_pushinteger(L, written);
	return 1;
}
REF_WRAP_MANUAL(wrap_fs_write);

REF_FUNCTION(fs_eof);
REF_FUNCTION(fs_tell);
REF_FUNCTION(fs_size);

int wrap_fs_read_entire_file_to_memory(lua_State* L)
{
	const char* path = lua_tostring(L, -1);
	lua_pop(L, 1);
	size_t sz = 0;
	char* data = (char*)fs_read_entire_file_to_memory_and_nul_terminate(path, &sz);
	if (!data) return 0;
	lua_pushstring(L, data);
	lua_pushinteger(L, sz);
	return 2;
}
REF_WRAP_MANUAL(wrap_fs_read_entire_file_to_memory);

REF_FUNCTION(fs_get_backend_specific_error_message);
REF_FUNCTION(fs_get_actual_path);

// -------------------------------------------------------------------------------------------------
// haptics

CF_HAPTIC_TYPE_DEFS
CF_HAPTIC_WAVE_TYPE_DEFS

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

REF_STRUCT(CF_ImeComposition,
	REF_MEMBER(composition),
	REF_MEMBER(cursor),
	REF_MEMBER(selection_len),
);

REF_FUNCTION(input_get_ime_composition);

REF_STRUCT(CF_Touch,
	REF_MEMBER(id),
	REF_MEMBER(x),
	REF_MEMBER(y),
	REF_MEMBER(pressure),
);

int wrap_touch_get(lua_State* L)
{
	uint64_t id = (uint64_t)lua_tointeger(L, -1);
	lua_pop(L, 1);
	CF_Touch touch;
	bool ok = touch_get(id, &touch);
	lua_pushboolean(L, ok);
	if (ok) {
		REF_LuaSet(L, &touch);
		return 2;
	} else {
		return 1;
	}
}
REF_WRAP_MANUAL(wrap_touch_get);

int wrap_touch_get_all(lua_State* L)
{
	CF_Touch* touches = NULL;
	int touch_count = cf_touch_get_all(&touches);
	lua_newtable(L);
	for (int i = 0; i < touch_count; ++i) {
		REF_LuaSet(L, touches + i);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}
REF_WRAP_MANUAL(wrap_touch_get_all);

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

REF_HANDLE_TYPE(CF_Noise);

REF_FUNCTION(make_noise);
REF_FUNCTION(make_noise_fbm);
REF_FUNCTION(destroy_noise);
REF_FUNCTION_EX(noise2, cf_noise2);
REF_FUNCTION_EX(noise3, cf_noise3);
REF_FUNCTION_EX(noise4, cf_noise4);

// @TODO Image helpers.

// -------------------------------------------------------------------------------------------------
// Time

REF_GLOBAL(CF_DELTA_TIME);
REF_GLOBAL(CF_DELTA_TIME_FIXED);
REF_GLOBAL(CF_DELTA_TIME_INTERPOLANT);
REF_GLOBAL(CF_TICKS);
REF_GLOBAL(CF_PREV_TICKS);
REF_GLOBAL(CF_SECONDS);
REF_GLOBAL(CF_PREV_SECONDS);
REF_GLOBAL(CF_PAUSE_TIME_LEFT);

REF_FUNCTION(set_fixed_timestep);
REF_FUNCTION(set_fixed_timestep_max_updates);
REF_FUNCTION(set_target_framerate);

String g_update_name_in_lua;
static void wrap_app_update_fn(void* udata)
{
	REF_CallLuaFunction(L, g_update_name_in_lua);
}
int wrap_app_update(lua_State* L)
{
	// Update with a callback.
	if (lua_isstring(L, -1)) {
		const char* fn_name = lua_tostring(L, -1);
		lua_pop(L, 1);
		g_update_name_in_lua = fn_name;
		app_update(wrap_app_update_fn);
	} else {
		app_update(NULL);
	}
	return 0;
}
REF_WRAP_MANUAL(wrap_app_update);

REF_FUNCTION(pause_for);
REF_FUNCTION(pause_for_ticks);
REF_FUNCTION(on_interval);
REF_FUNCTION(between_interval);
REF_FUNCTION(on_timestamp);
REF_FUNCTION(is_paused);
REF_FUNCTION(get_ticks);
REF_FUNCTION(get_tick_frequency);
REF_FUNCTION(sleep);

// -------------------------------------------------------------------------------------------------
// Version

REF_CONSTANT(CF_VERSION_STRING_COMPILED);
REF_FUNCTION(version_string_linked);

// -------------------------------------------------------------------------------------------------
// Manually bind certain, difficult to automate, functions.

CF_MemoryPool* g_sprite_pool;
int wrap_make_demo_sprite(lua_State* L)
{
	if (!g_sprite_pool) g_sprite_pool = make_memory_pool(sizeof(CF_Sprite), 1024 * 1024, 8);
	CF_Sprite* s = (CF_Sprite*)memory_pool_alloc(g_sprite_pool);
	*s = cf_make_demo_sprite();
	REF_LuaSet(L, &s);
	return 1;
}
REF_WRAP_MANUAL(wrap_make_demo_sprite);

int wrap_make_sprite(lua_State* L)
{
	if (!g_sprite_pool) g_sprite_pool = make_memory_pool(sizeof(CF_Sprite), 1024 * 1024, 8);
	char* path;
	REF_LuaGet(L, -1, &path);
	lua_pop(L, 1);
	CF_Sprite* s = (CF_Sprite*)memory_pool_alloc(g_sprite_pool);
	*s = make_sprite(path);
	REF_LuaSet(L, &s);
	return 1;
}
REF_WRAP_MANUAL(wrap_make_sprite);

int wrap_destroy_sprite(lua_State* L)
{
	CF_Sprite* s;
	REF_LuaGet(L, -1, &s);
	lua_pop(L, 1);
	memory_pool_free(g_sprite_pool, s);
	return 0;
}
REF_WRAP_MANUAL(wrap_destroy_sprite);