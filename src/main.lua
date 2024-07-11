while app_is_running() do
	app_update(0)
	
	if key_just_pressed(KEY_SPACE) then
		message_box(MESSAGE_BOX_TYPE_ERROR, "PEN15", "TEXT OF THE ERROR MESSAGE")
	end
	
	local text = "Hello"
	draw_text(text, 0,0, #text)
	draw_circle(-100,100, 50, 10)
	draw_quad(-100,-100, 100,100, 5, 5)
	
	local line = { -10,10, 100,-100, -50,-150, -200,50 }
	draw_polyline(line, 3, false)
	
	app_draw_onto_screen(true)
end

destroy_app()
