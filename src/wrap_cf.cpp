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

// -------------------------------------------------------------------------------------------------
// Math

CF_SHAPE_TYPE_DEFS

REF_FLAT_FLOATS(v2);
REF_FLAT_FLOATS(CF_M2x2);
REF_FLAT_FLOATS(CF_M3x2);
REF_FLAT_FLOATS(CF_Aabb);
REF_FLAT_INTS(CF_Rect);
REF_FLAT_FLOATS(CF_Color);
REF_FLAT_FLOATS(CF_Circle);
REF_FLAT_FLOATS(CF_Capsule);
REF_FLAT_FLOATS(CF_Raycast);
REF_FLAT_FLOATS(CF_Ray);
REF_FLAT_FLOATS(CF_Halfspace);
REF_FLAT_FLOATS(CF_SinCos);
REF_FLAT_FLOATS(CF_Transform);
REF_FLAT_FLOATS(CF_ToiResult);

REF_STRUCT(CF_Poly,
	REF_MEMBER_ARRAY(verts, count),
	REF_MEMBER_ARRAY(norms, count),
);

REF_STRUCT(CF_SliceOutput,
	REF_MEMBER(front),
	REF_MEMBER(back),
);

CF_Poly wrap_make_poly(CF_V2* pts, int count)
{
	CF_Poly poly;
	poly.count = min(CF_POLY_MAX_VERTS, count);
	CF_MEMCPY(poly.verts, pts, sizeof(CF_V2) * poly.count);
	make_poly(&poly);
	return poly;
}
REF_FUNCTION_EX(make_poly, wrap_make_poly, {0,1});

REF_FUNCTION(ray_to_halfspace);
REF_FUNCTION(center_of_mass);
REF_FUNCTION(calc_area);
REF_FUNCTION(slice);
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

REF_FUNCTION(ortho_2d);

// @TODO Move these into CF?
CF_Raycast cast_ray_to_circle(CF_Ray A, CF_Circle B) { CF_Raycast cast; cast_ray(A, &B, NULL, SHAPE_TYPE_CIRCLE, &cast); return cast; }
CF_Raycast cast_ray_to_aabb(CF_Ray A, CF_Aabb B) { CF_Raycast cast; cast_ray(A, &B, NULL, SHAPE_TYPE_AABB, &cast); return cast; }
CF_Raycast cast_ray_to_capsule(CF_Ray A, CF_Capsule B) { CF_Raycast cast; cast_ray(A, &B, NULL, SHAPE_TYPE_CAPSULE, &cast); return cast; }
CF_Raycast cast_ray_to_poly(CF_Ray A, CF_Poly B) { CF_Raycast cast; cast_ray(A, &B, NULL, SHAPE_TYPE_POLY, &cast); return cast; }
REF_FUNCTION(cast_ray_to_circle);
REF_FUNCTION(cast_ray_to_aabb);
REF_FUNCTION(cast_ray_to_capsule);
REF_FUNCTION(cast_ray_to_poly);
REF_FUNCTION(impact);
REF_FUNCTION(endpoint);

// @TODO Move these into CF?
CF_ToiResult cast_circle_to_circle(CF_Circle A, CF_V2 vA, CF_Circle B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_CIRCLE, NULL, vA, &B, SHAPE_TYPE_CIRCLE, NULL, vB, (int)use_radius); }
CF_ToiResult cast_circle_to_aabb(CF_Circle A, CF_V2 vA, CF_Aabb B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_CIRCLE, NULL, vA, &B, SHAPE_TYPE_AABB, NULL, vB, (int)use_radius); }
CF_ToiResult cast_circle_to_capsule(CF_Circle A, CF_V2 vA, CF_Capsule B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_CIRCLE, NULL, vA, &B, SHAPE_TYPE_CAPSULE, NULL, vB, (int)use_radius); }
CF_ToiResult cast_circle_to_poly(CF_Circle A, CF_V2 vA, CF_Poly B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_CIRCLE, NULL, vA, &B, SHAPE_TYPE_POLY, NULL, vB, (int)use_radius); }
CF_ToiResult cast_aabb_to_aabb(CF_Aabb A, CF_V2 vA, CF_Aabb B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_AABB, NULL, vA, &B, SHAPE_TYPE_AABB, NULL, vB, (int)use_radius); }
CF_ToiResult cast_aabb_to_capsule(CF_Aabb A, CF_V2 vA, CF_Capsule B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_AABB, NULL, vA, &B, SHAPE_TYPE_CAPSULE, NULL, vB, (int)use_radius); }
CF_ToiResult cast_aabb_to_poly(CF_Aabb A, CF_V2 vA, CF_Poly B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_AABB, NULL, vA, &B, SHAPE_TYPE_POLY, NULL, vB, (int)use_radius); }
CF_ToiResult cast_capsule_to_capsule(CF_Capsule A, CF_V2 vA, CF_Capsule B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_CAPSULE, NULL, vA, &B, SHAPE_TYPE_CAPSULE, NULL, vB, (int)use_radius); }
CF_ToiResult cast_capsule_to_poly(CF_Capsule A, CF_V2 vA, CF_Poly B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_CAPSULE, NULL, vA, &B, SHAPE_TYPE_POLY, NULL, vB, (int)use_radius); }
CF_ToiResult cast_poly_to_poly(CF_Poly A, CF_V2 vA, CF_Poly B, CF_V2 vB, bool use_radius) { return toi(&A, SHAPE_TYPE_POLY, NULL, vA, &B, SHAPE_TYPE_POLY, NULL, vB, (int)use_radius); }
REF_FUNCTION(cast_circle_to_circle);
REF_FUNCTION(cast_circle_to_aabb);
REF_FUNCTION(cast_circle_to_capsule);
REF_FUNCTION(cast_circle_to_poly);
REF_FUNCTION(cast_aabb_to_aabb);
REF_FUNCTION(cast_aabb_to_capsule);
REF_FUNCTION(cast_aabb_to_poly);
REF_FUNCTION(cast_capsule_to_capsule);
REF_FUNCTION(cast_capsule_to_poly);
REF_FUNCTION(cast_poly_to_poly);

// -------------------------------------------------------------------------------------------------
// Graphics

CF_BACKEND_TYPE_DEFS
CF_PIXEL_FORMAT_DEFS
CF_PIXELFORMAT_OP_DEFS
CF_FILTER_DEFS
CF_WRAP_MODE_DEFS
CF_VERTEX_FORMAT_DEFS
CF_CULL_MODE_DEFS
CF_COMPARE_FUNCTION_DEFS
CF_STENCIL_OP_DEFS
CF_BLEND_OP_DEFS
CF_BLENDFACTOR_DEFS
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
	REF_MEMBER(stream),
);

REF_STRUCT(CF_CanvasParams,
	REF_MEMBER(target),
	REF_MEMBER(depth_stencil_enable),
	REF_MEMBER(depth_stencil_target),
);

REF_STRUCT(CF_VertexAttribute,
	REF_MEMBER(name),
	REF_MEMBER(format),
	REF_MEMBER(offset),
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
REF_FUNCTION(texture_defaults);
REF_FUNCTION(make_texture);
REF_FUNCTION(destroy_texture);
REF_FUNCTION(texture_update);
REF_FUNCTION(make_shader);
REF_FUNCTION(shader_directory);
REF_FUNCTION(make_shader_from_source);
void wrap_shader_on_changed_callback(const char* path, void* udata)
{
	const char* lua_fn_name = (const char*)udata;
	REF_CallLuaFunction(L, lua_fn_name, { }, path);
}
void wrap_shader_on_changed(const char* lua_fn_name)
{
	shader_on_changed(wrap_shader_on_changed_callback, (void*)lua_fn_name);
}
REF_FUNCTION_EX(shader_on_changed, wrap_shader_on_changed);
// @TODO compile_shader_to_bytecode
// @TODO make_shader_from_bytecode
REF_FUNCTION(destroy_shader);
REF_FUNCTION(canvas_defaults);
REF_FUNCTION(make_canvas);
REF_FUNCTION(destroy_canvas);
REF_FUNCTION(canvas_get_target);
REF_FUNCTION(canvas_get_depth_stencil_target);
REF_FUNCTION(make_mesh, {1, 2});
REF_FUNCTION(destroy_mesh);
// @TODO These need manual serializers.
//REF_FUNCTION(mesh_update_vertex_data);
//REF_FUNCTION(mesh_update_instance_data);
//REF_FUNCTION(mesh_update_index_data);
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
REF_FUNCTION(commit);
REF_FUNCTION_EX(clear_color, cf_clear_color);

// -------------------------------------------------------------------------------------------------
// App

CF_APP_OPTION_DEFS
CF_POWER_STATE_DEFS
CF_DISPLAY_ORIENTATION_DEFS

REF_FUNCTION(default_display);
REF_FUNCTION(display_count);
int wrap_get_display_list(lua_State* L)
{
	int count = display_count();
	CF_DisplayID* ids = get_display_list();
	REF_LuaSetArray(L, ids, count);
	free_display_list(ids);
	return 1;
}
REF_WRAP_MANUAL(wrap_get_display_list);
REF_FUNCTION(display_x);
REF_FUNCTION(display_y);
REF_FUNCTION(display_width);
REF_FUNCTION(display_height);
REF_FUNCTION(display_refresh_rate);
REF_FUNCTION(display_bounds);
REF_FUNCTION(display_name);
REF_FUNCTION(display_orientation);

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
REF_FUNCTION(app_set_size);
REF_FUNCTION(app_show_window);
v2 wrap_app_get_size() { int x, y; app_get_size(&x, &y); return V2((float)x, (float)y); }
REF_FUNCTION_EX(app_get_size, wrap_app_get_size);
REF_FUNCTION(app_get_dpi_scale);
REF_FUNCTION(app_dpi_scaled_was_changed);
REF_FUNCTION(app_center_window);
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
REF_FUNCTION(app_set_windowed_mode);
REF_FUNCTION(app_set_borderless_fullscreen_mode);
REF_FUNCTION(app_set_fullscreen_mode);
REF_FUNCTION(app_set_title);
REF_FUNCTION(app_set_icon);
CF_PowerState app_get_power_state() { CF_PowerInfo info = app_power_info(); return info.state; }
int app_get_power_seconds_left() { CF_PowerInfo info = app_power_info(); return info.seconds_left; }
int app_get_power_percentage_left() { CF_PowerInfo info = app_power_info(); return info.percentage_left; }
REF_FUNCTION(app_get_power_seconds_left);
REF_FUNCTION(app_get_power_percentage_left);

// -------------------------------------------------------------------------------------------------
// Audio

REF_HANDLE_TYPE(CF_Audio);
REF_FUNCTION_EX(audio_cull_duplicates, cf_audio_cull_duplicates);
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
	REF_MEMBER(sample_index),
);

REF_HANDLE_TYPE(CF_Sound);
REF_HANDLE_TYPE(Sound);

SoundParams sound_params_defaults() { return SoundParams(); }
REF_FUNCTION(sound_params_defaults);
REF_FUNCTION(play_sound);
REF_FUNCTION(sound_is_active);
REF_FUNCTION(sound_get_is_paused);
REF_FUNCTION(sound_get_is_looped);
REF_FUNCTION(sound_get_volume);
REF_FUNCTION(sound_get_pitch);
REF_FUNCTION(sound_get_sample_index);
REF_FUNCTION(sound_set_is_paused);
REF_FUNCTION(sound_set_is_looped);
REF_FUNCTION(sound_set_volume);
REF_FUNCTION(sound_set_pitch);
REF_FUNCTION(sound_set_sample_index);
REF_FUNCTION(sound_stop);

void wrap_on_sound_finish(CF_Sound snd, void* udata)
{
	const char* lua_fn_name = (const char*)udata;
	REF_CallLuaFunction(L, lua_fn_name, { }, snd);
}
void wrap_sound_set_on_finish(const char* lua_fn_name)
{
	cf_sound_set_on_finish_callback(wrap_on_sound_finish, (void*)lua_fn_name, true);
}
REF_FUNCTION_EX(sound_set_on_finish, wrap_sound_set_on_finish);

void wrap_on_music_finish(void* udata)
{
	const char* lua_fn_name = (const char*)udata;
	REF_CallLuaFunction(L, lua_fn_name);
}
void wrap_music_set_on_finish(const char* lua_fn_name)
{
	cf_music_set_on_finish_callback(wrap_on_music_finish, (void*)lua_fn_name, true);
}
REF_FUNCTION_EX(music_set_on_finish, wrap_music_set_on_finish);

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
REF_FUNCTION_EX(sprite_set_frame, cf_sprite_set_frame);
REF_FUNCTION_EX(sprite_frame_delay, cf_sprite_frame_delay);
REF_FUNCTION_EX(sprite_animation_delay, cf_sprite_animation_delay);
REF_FUNCTION_EX(sprite_animation_interpolant, cf_sprite_animation_interpolant);
REF_FUNCTION_EX(sprite_will_finish, cf_sprite_will_finish);
REF_FUNCTION_EX(sprite_on_loop, cf_sprite_on_loop);
REF_FUNCTION_EX(sprite_width, cf_sprite_width);
REF_FUNCTION_EX(sprite_height, cf_sprite_height);
REF_FUNCTION_EX(sprite_get_width, cf_sprite_width);
REF_FUNCTION_EX(sprite_get_height, cf_sprite_height);
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
REF_FUNCTION_EX(sprite_get_local_offset, cf_sprite_get_local_offset);
// The rest of sprite stuff bound manually below.

REF_FUNCTION_EX(draw_quad, cf_draw_quad);
REF_FUNCTION_EX(draw_quad_fill, cf_draw_quad_fill);
REF_FUNCTION_EX(draw_box, cf_draw_quad);
REF_FUNCTION_EX(draw_box_fill, cf_draw_quad_fill);
REF_FUNCTION(draw_box_rounded);
REF_FUNCTION(draw_box_rounded_fill);
REF_FUNCTION_EX(draw_circle, cf_draw_circle);
REF_FUNCTION_EX(draw_circle_fill, cf_draw_circle_fill);
REF_FUNCTION_EX(draw_capsule, cf_draw_capsule);
REF_FUNCTION_EX(draw_capsule_fill, cf_draw_capsule_fill);
REF_FUNCTION(draw_tri);
REF_FUNCTION(draw_tri_fill);
REF_FUNCTION(draw_line);
REF_FUNCTION(draw_polyline, {0,1});
REF_FUNCTION(draw_polygon_fill, {0,1});
REF_FUNCTION(draw_polygon_fill_simple, {0,1});
REF_FUNCTION_EX(draw_bezier_line, cf_draw_bezier_line2);
REF_FUNCTION(draw_arrow);

REF_FUNCTION(draw_push_layer);
REF_FUNCTION(draw_pop_layer);
REF_FUNCTION(draw_peek_layer);
REF_FUNCTION(draw_push_color);
REF_FUNCTION(draw_pop_color);
REF_FUNCTION(draw_peek_color);
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

REF_FUNCTION(draw_push_viewport);
REF_FUNCTION(draw_pop_viewport);
REF_FUNCTION(draw_peek_viewport);
REF_FUNCTION(draw_push_scissor);
REF_FUNCTION(draw_pop_scissor);
REF_FUNCTION(draw_peek_scissor);
REF_FUNCTION(draw_push_render_state);
REF_FUNCTION(draw_pop_render_state);
REF_FUNCTION(draw_peek_render_state);
REF_FUNCTION(draw_set_atlas_dimensions);
REF_FUNCTION(make_draw_shader);
REF_FUNCTION(make_draw_shader_from_source);
REF_FUNCTION(draw_push_shader);
REF_FUNCTION(draw_pop_shader);
REF_FUNCTION(draw_peek_shader);
REF_FUNCTION(draw_push_alpha_discard);
REF_FUNCTION(draw_pop_alpha_discard);
REF_FUNCTION(draw_peek_alpha_discard);
REF_FUNCTION(draw_set_texture);

REF_FUNCTION_EX(render_settings_set_uniform_int,   cf_draw_set_uniform_int);
REF_FUNCTION_EX(render_settings_set_uniform_float, cf_draw_set_uniform_float);
REF_FUNCTION_EX(render_settings_set_uniform_v2,    cf_draw_set_uniform_v2);
REF_FUNCTION_EX(render_settings_set_uniform_color, cf_draw_set_uniform_color);

REF_FUNCTION(draw_mul);
REF_FUNCTION_EX(draw_scale, cf_draw_scale);
REF_FUNCTION_EX(draw_translate, cf_draw_translate);
REF_FUNCTION(draw_rotate);
REF_FUNCTION(draw_TSR);
REF_FUNCTION(draw_TSR_absolute);
REF_FUNCTION(draw_push);
REF_FUNCTION(draw_pop);
REF_FUNCTION(draw_peek);
REF_FUNCTION(draw_projection);
REF_FUNCTION(world_to_screen);
REF_FUNCTION(screen_to_world);
REF_FUNCTION(screen_bounds_to_world);
REF_FUNCTION_EX(draw_canvas, cf_draw_canvas);

REF_FUNCTION(render_to);

REF_STRUCT(CF_TemporaryImage,
	REF_MEMBER(tex),
	REF_MEMBER(w),
	REF_MEMBER(h),
	REF_MEMBER(u),
	REF_MEMBER(v),
);

REF_FUNCTION_EX(fetch_image, cf_fetch_image);

REF_STRUCT(CF_AtlasSubImage,
	REF_MEMBER(image_id),
	REF_MEMBER(w),
	REF_MEMBER(h),
	REF_MEMBER(minx),
	REF_MEMBER(miny),
	REF_MEMBER(maxx),
	REF_MEMBER(maxy),
);

REF_FUNCTION(register_premade_atlas, {2,1});

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

REF_FUNCTION_EX(fs_init, cf_fs_init);
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

int wrap_fs_read_entire_file(lua_State* L)
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
REF_WRAP_MANUAL(wrap_fs_read_entire_file);

REF_FUNCTION(fs_get_backend_specific_error_message);
REF_FUNCTION(fs_get_actual_path);
const char* normalize_path(const char* path) { Path p = path; p.normalize(); return sintern(p.c_str()); } // @JANK "leaks" the intern'd string, oh well.
REF_FUNCTION(normalize_path);

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
REF_FUNCTION(joypad_is_connected);
REF_FUNCTION(joypad_power_level);
REF_FUNCTION(joypad_name);
REF_FUNCTION(joypad_type);
REF_FUNCTION(joypad_vendor);
REF_FUNCTION(joypad_product_id);
REF_FUNCTION(joypad_serial_number);
REF_FUNCTION(joypad_firmware_version);
REF_FUNCTION(joypad_product_version);
REF_FUNCTION(joypad_button_down);
REF_FUNCTION(joypad_button_just_pressed);
REF_FUNCTION(joypad_button_just_released);
REF_FUNCTION(joypad_axis);
REF_FUNCTION(joypad_axis_prev);
REF_FUNCTION(joypad_rumble);

// -------------------------------------------------------------------------------------------------
// Noise

REF_HANDLE_TYPE(CF_Noise);

REF_FUNCTION(make_noise);
REF_FUNCTION(make_noise_fbm);
REF_FUNCTION(destroy_noise);
REF_FUNCTION_EX(noise2, cf_noise2);
REF_FUNCTION_EX(noise3, cf_noise3);
REF_FUNCTION_EX(noise4, cf_noise4);

int wrap_noise_pixels(lua_State* L)
{
	int w = (int)lua_tointeger(L, -4);
	int h = (int)lua_tointeger(L, -3);
	uint64_t seed = lua_tointeger(L, -2);
	float scale = (float)lua_tonumber(L, -1);
	lua_pop(L, 4);
	CF_Pixel* pixels = noise_pixels(w, h, seed, scale);
	int* pix = (int*)pixels;
	lua_newtable(L);
	REF_LuaSetArray(L, pix, w * h);
	cf_free(pix);
	return 1;
}
REF_WRAP_MANUAL(wrap_noise_pixels);

int wrap_noise_pixels_wrapped(lua_State* L)
{
	int w = (int)lua_tointeger(L, -6);
	int h = (int)lua_tointeger(L, -5);
	uint64_t seed = lua_tointeger(L, -4);
	float scale = (float)lua_tonumber(L, -3);
	float time = (float)lua_tonumber(L, -2);
	float amplitude = (float)lua_tonumber(L, -1);
	lua_pop(L, 6);
	CF_Pixel* pixels = noise_pixels_wrapped(w, h, seed, scale, time, amplitude);
	int* pix = (int*)pixels;
	lua_newtable(L);
	REF_LuaSetArray(L, pix, w * h);
	cf_free(pix);
	return 1;
}
REF_WRAP_MANUAL(wrap_noise_pixels_wrapped);

int wrap_noise_fbm_pixels(lua_State* L)
{
	int w = (int)lua_tointeger(L, -7);
	int h = (int)lua_tointeger(L, -6);
	uint64_t seed = lua_tointeger(L, -5);
	float scale = (float)lua_tonumber(L, -4);
	float lacunarity = (float)lua_tonumber(L, -3);
	int octaves = (int)lua_tointeger(L, -2);
	float falloff = (float)lua_tonumber(L, -1);
	lua_pop(L, 7);
	CF_Pixel* pixels = noise_fbm_pixels(w, h, seed, scale, lacunarity, octaves, falloff);
	int* pix = (int*)pixels;
	lua_newtable(L);
	REF_LuaSetArray(L, pix, w * h);
	cf_free(pix);
	return 1;
}
REF_WRAP_MANUAL(wrap_noise_fbm_pixels);

int wrap_noise_fbm_pixels_wrapped(lua_State* L)
{
	int w = (int)lua_tointeger(L, -9);
	int h = (int)lua_tointeger(L, -8);
	uint64_t seed = lua_tointeger(L, -7);
	float scale = (float)lua_tonumber(L, -6);
	float lacunarity = (float)lua_tonumber(L, -5);
	int octaves = (int)lua_tointeger(L, -4);
	float falloff = (float)lua_tonumber(L, -3);
	float time = (float)lua_tonumber(L, -2);
	float time_amplitude = (float)lua_tonumber(L, -1);
	lua_pop(L, 9);
	CF_Pixel* pixels = noise_fbm_pixels_wrapped(w, h, seed, scale, lacunarity, octaves, falloff, time, time_amplitude);
	int* pix = (int*)pixels;
	lua_newtable(L);
	REF_LuaSetArray(L, pix, w * h);
	cf_free(pix);
	return 1;
}
REF_WRAP_MANUAL(wrap_noise_fbm_pixels_wrapped);

// -------------------------------------------------------------------------------------------------
// Rnd

int wrap_rnd_seed(lua_State* L)
{
	uint64_t seed = lua_tointeger(L, -1);
	lua_pop(L, 1);
	CF_RndState* s = (CF_RndState*)lua_newuserdata(L, sizeof(CF_RndState));
	*s = rnd_seed(seed);
	return 1;
}
REF_WRAP_MANUAL(wrap_rnd_seed);

int wrap_rnd(lua_State* L)
{
	CF_RndState* s = (CF_RndState*)lua_touserdata(L, -1);
	lua_pop(L, 1);
	lua_pushinteger(L, rnd(s));
	return 1;
}
REF_WRAP_MANUAL(wrap_rnd);

int wrap_rnd_float(lua_State* L)
{
	CF_RndState* s = (CF_RndState*)lua_touserdata(L, -1);
	lua_pop(L, 1);
	lua_pushnumber(L, rnd_double(s));
	return 1;
}
REF_WRAP_MANUAL(wrap_rnd_float);

int wrap_rnd_range(lua_State* L)
{
	CF_RndState* s = (CF_RndState*)lua_touserdata(L, -3);
	uint64_t lo = lua_tointeger(L, -2);
	uint64_t hi = lua_tointeger(L, -1);
	lua_pop(L, 3);
	lua_pushinteger(L, cf_rnd_range_uint64(s, lo, hi));
	return 1;
}
REF_WRAP_MANUAL(wrap_rnd_range);

int wrap_rnd_range_float(lua_State* L)
{
	CF_RndState* s = (CF_RndState*)lua_touserdata(L, -3);
	double lo = lua_tonumber(L, -2);
	double hi = lua_tonumber(L, -1);
	lua_pop(L, 3);
	lua_pushnumber(L, cf_rnd_range_double(s, lo, hi));
	return 1;
}
REF_WRAP_MANUAL(wrap_rnd_range_float);

// -------------------------------------------------------------------------------------------------
// Time

REF_GLOBAL_EX(CF_DELTA_TIME, DELTA_TIME);
REF_GLOBAL_EX(CF_DELTA_TIME_FIXED, DELTA_TIME_FIXED);
REF_GLOBAL_EX(CF_DELTA_TIME_INTERPOLANT, DELTA_TIME_INTERPOLANT);
REF_GLOBAL_EX(CF_TICKS, TICKS);
REF_GLOBAL_EX(CF_PREV_TICKS, PREV_TICKS);
REF_GLOBAL_EX(CF_SECONDS, SECONDS);
REF_GLOBAL_EX(CF_PREV_SECONDS, PREV_SECONDS);
REF_GLOBAL_EX(CF_PAUSE_TIME_LEFT, PAUSE_TIME_LEFT);

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

REF_CONSTANT_EX(CF_VERSION_STRING_COMPILED, VERSION_STRING_COMPILED);
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
	const char* path = lua_tostring(L, -1);
	CF_Sprite* s = (CF_Sprite*)memory_pool_alloc(g_sprite_pool);
	*s = make_sprite(path);
	lua_pop(L, 1);
	REF_LuaSet(L, &s);
	return 1;
}
REF_WRAP_MANUAL(wrap_make_sprite);

int wrap_destroy_sprite(lua_State* L)
{
	CF_Sprite* s;
	REF_LuaGet(L, -1, &s);
	assert(s);
	lua_pop(L, 1);
	memory_pool_free(g_sprite_pool, s);
	return 0;
}
REF_WRAP_MANUAL(wrap_destroy_sprite);

int wrap_make_easy_sprite(lua_State* L)
{
	if (!g_sprite_pool) g_sprite_pool = make_memory_pool(sizeof(CF_Sprite), 1024 * 1024, 8);
	const char* path = lua_tostring(L, -1);
	CF_Sprite* s = (CF_Sprite*)memory_pool_alloc(g_sprite_pool);
	*s = cf_make_easy_sprite_from_png(path, NULL);
	REF_LuaSet(L, &s);
	return 1;
}
REF_WRAP_MANUAL(wrap_make_easy_sprite);

int wrap_make_premade_sprite(lua_State* L)
{
	if (!g_sprite_pool) g_sprite_pool = make_memory_pool(sizeof(CF_Sprite), 1024 * 1024, 8);
	uint64_t image_id = lua_tointeger(L, -1);
	CF_Sprite* s = (CF_Sprite*)memory_pool_alloc(g_sprite_pool);
	*s = make_premade_sprite(image_id);
	REF_LuaSet(L, &s);
	return 1;
}
REF_WRAP_MANUAL(wrap_make_premade_sprite);

int wrap_get_png_wh(lua_State* L)
{
	const char* path = lua_tostring(L, -1);
	lua_pop(L, 1);

	size_t sz = 0;
	void* data = fs_read_entire_file_to_memory(path, &sz);
	CF_ASSERT(data);
	int x, y;
	image_load_png_wh(data, (int)sz, &x, &y);
	cf_free(data);

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	return 2;
}
REF_WRAP_MANUAL(wrap_get_png_wh);

// -------------------------------------------------------------------------------------------------
// Dear ImGui bindings on an as-needed basis.

#include <imgui.h>

REF_CONSTANT(ImGuiWindowFlags_None);
REF_CONSTANT(ImGuiWindowFlags_NoTitleBar);
REF_CONSTANT(ImGuiWindowFlags_NoResize);
REF_CONSTANT(ImGuiWindowFlags_NoMove);
REF_CONSTANT(ImGuiWindowFlags_NoScrollbar);
REF_CONSTANT(ImGuiWindowFlags_NoScrollWithMouse);
REF_CONSTANT(ImGuiWindowFlags_NoCollapse);
REF_CONSTANT(ImGuiWindowFlags_AlwaysAutoResize);
REF_CONSTANT(ImGuiWindowFlags_NoBackground);
REF_CONSTANT(ImGuiWindowFlags_NoSavedSettings);
REF_CONSTANT(ImGuiWindowFlags_NoMouseInputs);
REF_CONSTANT(ImGuiWindowFlags_MenuBar);
REF_CONSTANT(ImGuiWindowFlags_HorizontalScrollbar);
REF_CONSTANT(ImGuiWindowFlags_NoFocusOnAppearing);
REF_CONSTANT(ImGuiWindowFlags_NoBringToFrontOnFocus);
REF_CONSTANT(ImGuiWindowFlags_AlwaysVerticalScrollbar);
REF_CONSTANT(ImGuiWindowFlags_AlwaysHorizontalScrollbar);
REF_CONSTANT(ImGuiWindowFlags_NoNavInputs);
REF_CONSTANT(ImGuiWindowFlags_NoNavFocus);
REF_CONSTANT(ImGuiWindowFlags_UnsavedDocument);
REF_CONSTANT(ImGuiWindowFlags_NoDocking);
REF_CONSTANT(ImGuiWindowFlags_NoNav);
REF_CONSTANT(ImGuiWindowFlags_NoDecoration);
REF_CONSTANT(ImGuiWindowFlags_NoInputs);

REF_CONSTANT(ImGuiCond_None);
REF_CONSTANT(ImGuiCond_Always);
REF_CONSTANT(ImGuiCond_Once);
REF_CONSTANT(ImGuiCond_FirstUseEver);
REF_CONSTANT(ImGuiCond_Appearing);

REF_CONSTANT(ImGuiTreeNodeFlags_None);
REF_CONSTANT(ImGuiTreeNodeFlags_Selected);
REF_CONSTANT(ImGuiTreeNodeFlags_Framed);
REF_CONSTANT(ImGuiTreeNodeFlags_AllowOverlap);
REF_CONSTANT(ImGuiTreeNodeFlags_NoTreePushOnOpen);
REF_CONSTANT(ImGuiTreeNodeFlags_NoAutoOpenOnLog);
REF_CONSTANT(ImGuiTreeNodeFlags_DefaultOpen);
REF_CONSTANT(ImGuiTreeNodeFlags_OpenOnDoubleClick);
REF_CONSTANT(ImGuiTreeNodeFlags_OpenOnArrow);
REF_CONSTANT(ImGuiTreeNodeFlags_Leaf);
REF_CONSTANT(ImGuiTreeNodeFlags_Bullet);
REF_CONSTANT(ImGuiTreeNodeFlags_FramePadding);
REF_CONSTANT(ImGuiTreeNodeFlags_SpanAvailWidth);
REF_CONSTANT(ImGuiTreeNodeFlags_SpanFullWidth);
REF_CONSTANT(ImGuiTreeNodeFlags_SpanTextWidth);
REF_CONSTANT(ImGuiTreeNodeFlags_SpanAllColumns);
REF_CONSTANT(ImGuiTreeNodeFlags_NavLeftJumpsBackHere);
REF_CONSTANT(ImGuiTreeNodeFlags_CollapsingHeader);

REF_CONSTANT(ImGuiMouseButton_Left);
REF_CONSTANT(ImGuiMouseButton_Right);
REF_CONSTANT(ImGuiMouseButton_Middle);

REF_CONSTANT(ImGuiComboFlags_None);
REF_CONSTANT(ImGuiComboFlags_PopupAlignLeft);
REF_CONSTANT(ImGuiComboFlags_HeightSmall);
REF_CONSTANT(ImGuiComboFlags_HeightRegular);
REF_CONSTANT(ImGuiComboFlags_HeightLarge);
REF_CONSTANT(ImGuiComboFlags_HeightLargest);
REF_CONSTANT(ImGuiComboFlags_NoArrowButton);
REF_CONSTANT(ImGuiComboFlags_NoPreview);
REF_CONSTANT(ImGuiComboFlags_WidthFitPreview);

void imgui_begin(const char* name) { ImGui::Begin(name); }
REF_FUNCTION(imgui_begin);
bool imgui_begin_ex(const char* name, bool opened, int flags) { ImGui::Begin(name, &opened, flags); return opened; }
REF_FUNCTION(imgui_begin_ex);
void imgui_end() { ImGui::End(); }
REF_FUNCTION(imgui_end);
void imgui_set_next_window_pos(CF_V2 position) { ImGui::SetNextWindowPos(position); }
REF_FUNCTION(imgui_set_next_window_pos);
void imgui_set_next_window_size(CF_V2 position) { ImGui::SetNextWindowSize(position); }
REF_FUNCTION(imgui_set_next_window_size);
bool imgui_button(const char* name) { return ImGui::Button(name); }
REF_FUNCTION(imgui_button);
bool imgui_button_ex(const char* name, CF_V2 size) { return ImGui::Button(name, size); }
REF_FUNCTION(imgui_button_ex);
float imgui_get_frame_height() { return ImGui::GetFrameHeight(); }
REF_FUNCTION(imgui_get_frame_height);
void imgui_separator() { ImGui::Separator(); }
REF_FUNCTION(imgui_separator);
void imgui_separator_text(const char* text) { ImGui::SeparatorText(text); }
REF_FUNCTION(imgui_separator_text);
int wrap_imgui_sprite_button(lua_State* L)
{
	const char* name = lua_tostring(L, -2);
	CF_Sprite* s;
	REF_LuaGet(L, -1, &s);
	lua_pop(L, 2);
	CF_TemporaryImage image = cf_fetch_image(s);
	ImTextureID id = (ImTextureID)cf_texture_handle(image.tex);
	ImVec2 size = { (float)image.w * s->scale.x, (float)image.h * s->scale.y };
	// y is flipped
	ImVec2 uv0 = { image.u.x, image.v.y };
	ImVec2 uv1 = { image.v.x, image.u.y };
	bool result = ImGui::ImageButton(name, id, size, uv0, uv1);
	lua_pushboolean(L, result);
	return 1;
}
REF_WRAP_MANUAL(wrap_imgui_sprite_button);
int wrap_imgui_sprite(lua_State* L)
{
	CF_Sprite* s;
	REF_LuaGet(L, -1, &s);
	lua_pop(L, 1);
	CF_TemporaryImage image = cf_fetch_image(s);
	ImTextureID id = (ImTextureID)cf_texture_handle(image.tex);
	ImVec2 size = { (float)image.w * s->scale.x, (float)image.h * s->scale.y };
	// y is flipped
	ImVec2 uv0 = { image.u.x, image.v.y };
	ImVec2 uv1 = { image.v.x, image.u.y };
	ImGui::Image(id, size, uv0, uv1);
	bool result = ImGui::IsItemClicked();
	lua_pushboolean(L, result);
	return 1;
}
REF_WRAP_MANUAL(wrap_imgui_sprite);
bool imgui_color(const char* name, CF_Color color) { return ImGui::ColorPicker4(name, &color.r); }
REF_FUNCTION(imgui_color);
bool imgui_checkbox(const char* name, bool opened) { ImGui::Checkbox(name, &opened); return opened; }
REF_FUNCTION(imgui_checkbox);
CF_V2 imgui_v2(const char* name, CF_V2 v, const char* fmt) { ImGui::InputFloat2(name, &v.x, fmt); return v; }
REF_FUNCTION(imgui_v2);
REF_FUNCTION_EX(imgui_float2, imgui_v2);
int imgui_int(const char* name, int i) { ImGui::InputInt(name, &i); return i; }
REF_FUNCTION(imgui_int);
float imgui_float(const char* name, float f) { ImGui::InputFloat(name, &f); return f; }
REF_FUNCTION(imgui_float);
void imgui_demo_window() { ImGui::ShowDemoWindow(); }
REF_FUNCTION(imgui_demo_window);
void imgui_dockspace_over_viewport() { ImGui::DockSpaceOverViewport(0, ImGui::GetWindowViewport(), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar); }
REF_FUNCTION(imgui_dockspace_over_viewport);
void imgui_begin_main_menu_bar() { ImGui::BeginMainMenuBar(); }
REF_FUNCTION(imgui_begin_main_menu_bar);
void imgui_end_main_menu_bar() { ImGui::EndMainMenuBar(); }
REF_FUNCTION(imgui_end_main_menu_bar);
bool imgui_is_window_docked() { return ImGui::IsWindowDocked(); }
REF_FUNCTION(imgui_is_window_docked);
void imgui_begin_menu(const char* name) { ImGui::BeginMenu(name); }
REF_FUNCTION(imgui_begin_menu);
void imgui_end_menu() { ImGui::EndMenu(); }
REF_FUNCTION(imgui_end_menu);
bool imgui_menu_item(const char* name, const char* shortcut, bool opened) {
	if (ImGui::MenuItem(name, shortcut, opened)) {
		opened = !opened;
	}
	return opened;
}
REF_FUNCTION(imgui_menu_item);
bool imgui_collapsing_header(const char* name) { return ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_None); }
REF_FUNCTION(imgui_collapsing_header);
bool imgui_collapsing_header_ex(const char* name, int flags) { return ImGui::CollapsingHeader(name, flags); }
REF_FUNCTION(imgui_collapsing_header_ex);
void imgui_same_line() { ImGui::SameLine(); }
REF_FUNCTION(imgui_same_line);
void imgui_sameline() { ImGui::SameLine(); }
REF_FUNCTION(imgui_sameline);
bool imgui_begin_combo(const char* name, const char* preview) { return ImGui::BeginCombo(name, preview); }
REF_FUNCTION(imgui_begin_combo);
bool imgui_begin_combo_ex(const char* name, const char* preview, int flags) { return ImGui::BeginCombo(name, preview, flags); }
REF_FUNCTION(imgui_begin_combo_ex);
void imgui_end_combo() { ImGui::EndCombo(); }
REF_FUNCTION(imgui_end_combo);
bool imgui_selectable(const char* name, bool is_selected) { return ImGui::Selectable(name, is_selected); }
REF_FUNCTION(imgui_selectable);
bool imgui_is_item_hovered() { return ImGui::IsItemHovered(); }
REF_FUNCTION(imgui_is_item_hovered);
void imgui_set_item_default_focus() { ImGui::SetItemDefaultFocus(); }
REF_FUNCTION(imgui_set_item_default_focus);
void imgui_set_frame_rounding(float rounding) { ImGui::GetStyle().FrameRounding = rounding; }
REF_FUNCTION(imgui_set_frame_rounding);
void imgui_push_id(int id) { ImGui::PushID(id); }
REF_FUNCTION(imgui_push_id);
void imgui_pop_id() { ImGui::PopID(); }
REF_FUNCTION(imgui_pop_id);
void imgui_dummy(float x, float y) { ImGui::Dummy(ImVec2(x, y)); }
REF_FUNCTION(imgui_dummy);
CF_V2 imgui_get_cursor_screen_pos() { return ImGui::GetCursorScreenPos(); }
REF_FUNCTION(imgui_get_cursor_screen_pos);
void imgui_set_cursor_screen_pos(CF_V2 p) { ImGui::SetCursorScreenPos(p); }
REF_FUNCTION(imgui_set_cursor_screen_pos);
bool imgui_is_item_clicked() { return ImGui::IsItemClicked(); }
REF_FUNCTION(imgui_is_item_clicked);
void imgui_add_rect(CF_V2 min, CF_V2 max, CF_Color col, float rounding, float thickness) {
	ImDrawList* draw = ImGui::GetWindowDrawList();
	CF_Pixel p = to_pixel(col);
	draw->AddRect(min, max, IM_COL32(255,0,0,255), rounding, ImDrawFlags_None, thickness);
}
REF_FUNCTION(imgui_add_rect);
void imgui_columns(int columns) { ImGui::Columns(columns); }
REF_FUNCTION(imgui_columns);
void imgui_next_column() { ImGui::NextColumn(); }
REF_FUNCTION(imgui_next_column);
bool imgui_is_mouse_clicked_on_world(int button) { return !ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseClicked(button); }
REF_FUNCTION(imgui_is_mouse_clicked_on_world);
bool imgui_want_mouse_capture() { return ImGui::GetIO().WantCaptureMouse; }
REF_FUNCTION(imgui_want_mouse_capture);
bool imgui_want_keyboard_capture() { return ImGui::GetIO().WantCaptureKeyboard; }
REF_FUNCTION(imgui_want_keyboard_capture);
bool imgui_is_any_window_hovered() { return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow); }
REF_FUNCTION(imgui_is_any_window_hovered);
void imgui_text(const char* text) { CF_ASSERT(text); ImGui::Text(text); }
REF_FUNCTION(imgui_text);
char* imgui_input_text(const char* label, const char* text) { CF_ASSERT(text); static char buffer[1024]; sprintf(buffer, "%s", text); ImGui::InputText(label, buffer, sizeof(buffer)); return buffer; }
REF_FUNCTION(imgui_input_text);
void imgui_set_next_item_width(float width) { ImGui::SetNextItemWidth(width); }
REF_FUNCTION(imgui_set_next_item_width);
void imgui_set_next_item_open(bool open) { ImGui::SetNextItemOpen(open); }
REF_FUNCTION(imgui_set_next_item_open);
void imgui_text_colored(float r, float g, float b, float a, const char* text) { ImGui::TextColored(ImVec4(r, g, b, a), text); }
REF_FUNCTION(imgui_text_colored);
void imgui_set_scroll_here_y(float y) { ImGui::SetScrollHereY(y); }
REF_FUNCTION(imgui_set_scroll_here_y);
float imgui_slider_float(const char* label, float v, float lo, float hi) { ImGui::SliderFloat(label, &v, lo, hi); return v; }
REF_FUNCTION(imgui_slider_float);
CF_V2 imgui_slider_float2(const char* label, CF_V2 v, float lo, float hi) { ImGui::SliderFloat2(label, &v.x, lo, hi); return v; }
REF_FUNCTION(imgui_slider_float2);