shd = make_shader("flash")
spr = make_demo_sprite()
sprite_play(spr, "spin")
sprite_set_scale(spr, 4,4)

function main()
	while app_is_running() do
		app_update(0)
		REF_SyncGlobals()
		
		print(CF_DELTA_TIME)
		
		if key_just_pressed(KEY_SPACE) then
			message_box(MESSAGE_BOX_TYPE_ERROR, "PEN15", "TEXT OF THE ERROR MESSAGE")
		end
		
		draw_push_layer(1)
		sprite_update(spr)
		draw_sprite(spr)
		draw_pop_layer()
		
		local text = "Hello"
		draw_text(text, 0,0, #text)
		draw_circle(-100,100, 50, 10)
		draw_quad(-100,-100, 100,100, 5, 5)
		
		local line = { -10,10, 100,-100, -50,-150, -200,50 }
		draw_polyline(line, 3, false)
		
		app_draw_onto_screen(true)
	end

	destroy_sprite(spr)
	destroy_app()
end
