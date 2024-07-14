shd = make_shader("flash")
spr = make_demo_sprite()
sprite_play(spr, "spin")
sprite_set_scale(spr, 4,4)

function on_update()
	REF_SyncGlobals()
	
	if key_just_pressed(KEY_SPACE) then
		message_box(MESSAGE_BOX_TYPE_ERROR, "PEN15", "TEXT OF THE ERROR MESSAGE")
	end
end

function blue_text(fx)
	if fx.on_begin then
		fx.color = { 1, 0, 0, 1 }
	else
		fx.color = { 0, 0, 1, 1 }
	end
	return true, fx
end

function main()
	set_fixed_timestep(30)
	text_effect_register("blue", "blue_text")

	while app_is_running() do
		app_update("on_update")
		draw_push_layer(1)
		sprite_update(spr)
		draw_sprite(spr)
		draw_pop_layer()
		
		--local text = "<blue>Hello</blue>"
		--draw_text(text, 100,0, #text)
		--draw_circle(-100,100, 50, 10)
		--draw_quad(-100,-100, 100,100, 5, 5)
		
		app_draw_onto_screen(true)
	end

	destroy_sprite(spr)
	destroy_app()
end
