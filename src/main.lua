function V2(x, y)
	local v = {x=x, y=y}
	return v
end


while app_is_running() do
	app_update(0)
	
	if key_just_pressed(KEY_SPACE) then
		message_box(MESSAGE_BOX_TYPE_ERROR, "PEN15", "TEXT OF THE ERROR MESSAGE")
	end
	
	local text = "Hello"
	draw_text(text, V2(0,0), #text)
	draw_circle(V2(-100,100), 50, 10)
	draw_quad(V2(-100,-100), V2(100,100),5,5)
	
	app_draw_onto_screen(true)
end

destroy_app()
