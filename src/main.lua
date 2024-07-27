dump_lua_api()
make_app("CF in Lua", 0, 0, 0, 640, 480, APP_OPTIONS_WINDOW_POS_CENTERED + APP_OPTIONS_RESIZABLE, argv0)
mount_directory_as("/content", "/")
app_set_icon("CF_Logo_Pixel.png")
load_shaders()
shd = make_shader("gray")

function main()
    local girl = make_demo_sprite()
    sprite_play(girl, "spin")
    
    local params = canvas_defaults(640/4,480/4)
    local canvas = make_canvas(params)
	local shadow = make_canvas(params)
	clear_color(0,0,0,0)

    while app_is_running() do
        app_update()
        REF_SyncGlobals()
		
		if key_just_pressed(KEY_SPACE) then
			app_set_size(640*2,480*2)
		end
        
        sprite_update(girl)
        draw_scale(4,4)
        draw_sprite(girl)
        draw_push_antialias(false)
        draw_circle(50,0,10,5)
        draw_pop_antialias()
        draw_pop()
        render_to(canvas, true)
		
		render_settings_push_shader(shd)
		render_settings_push_texture("tex", canvas_get_target(canvas))
		draw_push_antialias(false);
		draw_box_fill(-320,-240,320,240);
		render_to(shadow, true)
        draw_pop_antialias()
		render_settings_pop_shader()
		
		render_to(app_get_canvas(), true) -- clear the canvas
		local dx=2/(640/4)
		local dy=2/(480/4)
        canvas_blit(shadow, dx,dy, 1,1, app_get_canvas(), 0,0, 1-dx,1-dy)
        canvas_blit(canvas, 0,0, 1,1, app_get_canvas(), 0,0, 1,1)

        app_draw_onto_screen(false)
    end

    destroy_sprite(girl)
    destroy_app()
end
