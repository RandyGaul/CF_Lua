make_app("CF in Lua", 0, 0, 0, 480*2, 270*2, APP_OPTIONS_WINDOW_POS_CENTERED, argv0)
load_shaders()

function main()
	x = 480
	y = 270
	offscreen = make_canvas(canvas_defaults(480, 270))
	shader = make_shader('metaball')

	while app_is_running() do
		app_update()
		draw_scale(2, 2)

		draw_push_antialias_scale(10)
		draw_circle(0,0,0)
		draw_pop_antialias_scale()

		render_settings_push_shader(shader)
		draw_box_fill(-x/2, -y/2, x/2, y/2)
		render_to(offscreen, true)
		render_settings_pop_shader()

		canvas_blit(offscreen, 0, 0, 1, 1, app_get_canvas(), 0, 0, 1, 1)
		app_draw_onto_screen(false)
	end
	destroy_app()
end
