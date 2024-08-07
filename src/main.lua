w = 480
h = 270
scale = 2

make_app("CF in Lua", 0, 0, 0, w*scale, h*scale, APP_OPTIONS_WINDOW_POS_CENTERED, argv0)
load_shaders()

function main()
	clear_color(0,0,0,1)
	local t = 0

	while app_is_running() do
		app_update()
		REF_SyncGlobals()
		draw_scale(scale, scale)
		t = t + DELTA_TIME

		draw_push_antialias(true)
		draw_box_rounded_fill(-20,-20, 20,20, 20)

		app_draw_onto_screen(false)
	end
	destroy_app()
end
