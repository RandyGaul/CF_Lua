def = b2DefaultWorldDef()
world = b2CreateWorld(def)

shd = make_shader("flash")
spr = make_demo_sprite()
sprite_play(spr, "spin")
sprite_set_scale(spr, 4,4)

dbg_thickness = 0
fps = 1

function dbg_draw_polygon(verts, cr, cg, cb, ca)
	draw_push_color(cr, cg, cb, ca)
	draw_polyline(verts, dbg_thickness, true)
	draw_pop_color()
end

function dbg_draw_solid_polygon(verts, cr, cg, cb, ca)
	-- @TODO triangulate
	draw_push_color(cr, cg, cb, ca)
	draw_polyline(verts, dbg_thickness, true)
	draw_pop_color()
end

function dbg_draw_circle(x, y, r, cr, cg, cb, ca)
	draw_push_color(cr, cg, cb, ca)
	draw_circle(x, y, r, dbg_thickness)
	draw_pop_color()
end

function dbg_draw_solid_circle(x, y, r, cr, cg, cb, ca)
	draw_push_color(cr, cg, cb, ca)
	draw_circle_fill(x, y, r)
	draw_pop_color()
end

function dbg_draw_capsule(x0, y0, r0, x1, y1, r1, cr, cg, cb, ca)
	draw_push_color(cr, cg, cb, ca)
	draw_circle(x0, y0, r0, x1, y1, r1, dbg_thickness)
	draw_pop_color()
end

function dbg_draw_solid_capsule(x0, y0, r0, x1, y1, r1, cr, cg, cb, ca)
	draw_push_color(cr, cg, cb, ca)
	draw_circle_fill(x0, y0, r0, x1, y1, r1)
	draw_pop_color()
end

function dbg_draw_transform()
	-- @TODO
end

function dbg_draw_segment(x0, y0, x1, y1, cr, cg, cb, ca)
	draw_push_color(cr, cg, cb, ca)
	draw_line(x0, y0, x1, y1, dbg_thickness)
	draw_pop_color()
end

function dbg_draw_point(x, y, r, cr, cg, cb, ca)
	draw_push_color(cr, cg, cb, ca)
	draw_circle_fill(x, y, r*0.5)
	draw_pop_color()
end

function dbg_draw_string(x, y, s)
	draw_text(s, x, y, -1)
end

debug_draw = {
	draw_polygon = "dbg_draw_polygon",
	draw_solid_polygon = "dbg_draw_solid_polygon",
	draw_circle = "dbg_draw_circle",
	draw_solid_circle = "dbg_draw_solid_circle",
	draw_capsule = "dbg_draw_capsule",
	draw_solid_capsule = "dbg_draw_solid_capsule",
	draw_segment = "dbg_draw_segment",
	draw_transform = "dbg_draw_transform",
	draw_point = "dbg_draw_point",
	draw_string = "dbg_draw_string",
	drawingBounds = nil,
	useDrawingBounds = false,
	drawText = false,
	drawShapes = true,
	drawJoints = true,
	drawJointExtras = true,
	drawAABBs = false,
	drawMass = true,
	drawContacts = true,
	drawGraphColors = true,
	drawContactNormals = true,
	drawContactImpulses = true,
	drawFrictionImpulses = true,
}

function b2CreateBox(x, y, w, h, body_type)
	w = w * 0.5
	h = h * 0.5
	body_type = body_type or b2_staticBody
	local def = b2DefaultBodyDef()
	def.type = body_type
	local body = b2CreateBody(world, def)
	local verts = { x-w,y-h, x+w,y-h, x+w,y+h, x-w,y+h, }
	local shape_def = b2DefaultShapeDef()
	local poly = b2MakePolygon(verts, 0)
	return b2CreatePolygonShape(body, shape_def, poly)
end

b2CreateBox(0, 0, 300, 20, b2_staticBody) -- floor
for i=1,100 do
	b2CreateBox(0, 20+i*10.5, 10, 10, b2_dynamicBody)
end

def = b2DefaultChainDef()
def.points = { -25,-25, 50,-50, 100,-50, 150,-25, 200,0, 250,50 }
def.isLoop = true
b2CreateChain(b2CreateBody(world, b2DefaultBodyDef()), def)

function on_update()
	REF_SyncGlobals()
		
	if key_just_pressed(KEY_SPACE) then
		message_box(MESSAGE_BOX_TYPE_ERROR, "PEN15", "TEXT OF THE ERROR MESSAGE")
	end
	
	b2World_Step(world, CF_DELTA_TIME_FIXED, 4)
end

function blue_text(fx)
	if fx.on_begin then
		fx.color = { 1, 0, 0, 1 }
	else
		fx.color = { 0, 0, 1, 1 }
	end
	return true, fx
end

function lerp(x,a,b)
	return a+(b-a)*x
end

function main()
	set_fixed_timestep(60)
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
		
		draw_polyline({ -300,200, 0,250, 300,200 }, 5, false)
		
		b2World_Draw(world, debug_draw)
		
		if CF_DELTA_TIME > 0 then
			fps = lerp(1.0 / 10.0, fps, 1.0 / CF_DELTA_TIME)
		end
		draw_text(tostring(fps), -300,-200,-1)
		
		app_draw_onto_screen(true)
	end

	destroy_sprite(spr)
	destroy_app()
end
