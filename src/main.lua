w = 480
h = 270
scale = 2

make_app("CF in Lua", 0, 0, 0, w*scale, h*scale, APP_OPTIONS_WINDOW_POS_CENTERED, argv0)
load_shaders()

function main()
	soft_circles = make_canvas(canvas_defaults(w, h))
	meta = make_canvas(canvas_defaults(w, h))
	shader = make_shader('metaball')
	clear_color(0,0,0,1)
	local t = 0

	while app_is_running() do
		app_update()
		REF_SyncGlobals()
		draw_scale(scale, scale)
		t = t + DELTA_TIME

		draw_push()
		draw_scale(1,-1)

		rnd = rnd_seed(0)
		for i=1,100 do
			local o = rnd_range_float(rnd, -10,10)
			local x = rnd_range_float(rnd, -w/scale, w/scale) + math.cos(t+o) * 10
			local y = rnd_range_float(rnd, -h/scale, h/scale) + math.sin(t+o) * 10
			local r = rnd_range_float(rnd, 10,60)
			draw_push_antialias_scale(r)
			draw_circle_fill(x, y, 0)
			draw_pop_antialias_scale()
		end
		render_to(soft_circles, true)

		render_settings_push_shader(shader)
		render_settings_push_texture("tex", canvas_get_target(soft_circles))
		draw_box_fill(-w/scale, -h/scale, w/scale, h/scale)
		render_to(meta, true)
		render_settings_pop_shader()

		canvas_blit(meta, 0, 0, 1, 1, app_get_canvas(), 0, 0, 1, 1)

		draw_text("hello world", 0,0, -1)

		app_draw_onto_screen(false)
	end
	destroy_app()
end
