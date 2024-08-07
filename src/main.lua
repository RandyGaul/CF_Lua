make_app("CF in Lua", 0, 0, 0, 480*2, 270*2, APP_OPTIONS_WINDOW_POS_CENTERED, argv0)
load_shaders()

function main()
	offscreen = make_canvas(canvas_defaults(480, 270))
	shader = make_shader('metaball')

	while app_is_running() do
		app_update()
		draw_scale(2, 2)
		render_settings_push_shader(shader)
		draw_box_fill(-240, -135, 480, 270)
		render_to(offscreen, true)
		render_settings_pop_shader()
		canvas_blit(offscreen, 0, 0, 1, 1, app_get_canvas(), 0, 0, 1, 1)
		app_draw_onto_screen(false)
	end
	destroy_app()
end
