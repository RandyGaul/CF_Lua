shd = make_shader("flash")
spr = make_demo_sprite()
sprite_play(spr, "spin")
sprite_set_scale(spr, 4,4)

-- Setup basic box2D world
world = b2CreateWorld(b2DefaultWorldDef())

function b2CreateBox(x, y, w, h, body_type)
	w = w * 0.5
	h = h * 0.5
	body_type = body_type or b2_dynamicBody
	local def = b2DefaultBodyDef()
	def.type = body_type
	local body = b2CreateBody(world, def)
	local verts = { x-w,y-h, x+w,y-h, x+w,y+h, x-w,y+h, }
	local shape_def = b2DefaultShapeDef()
	local poly = b2MakePolygon(verts, 0)
	return b2CreatePolygonShape(body, shape_def, poly)
end

b0 = b2CreateBox(0, 0, 100, 20, b2_staticBody)

function draw_polygon(verts, color)
end

debug_draw_settings = {
}

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
		
		local text = "<blue>Hello</blue>"
		draw_text(text, 150,0, #text)
		draw_circle(-100,100, 50, 10)
		draw_quad(-100,-100, 100,100, 5, 5)
		
		app_draw_onto_screen(true)
	end

	destroy_sprite(spr)
	destroy_app()
end
