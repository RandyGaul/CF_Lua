while app_is_running() do
	app_update(0)
	
	print("APP_OPTIONS_D3D11_CONTEXT : "..APP_OPTIONS_D3D11_CONTEXT)
	print(test)
	
	local text = "Hello"
	draw_text(text, 0, 0, #text)
	
	draw_quad(-100,-100,100,100,5,5)
	
	app_draw_onto_screen(true)
end

destroy_app()
