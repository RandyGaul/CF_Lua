dump_lua_api()
make_app("CF in Lua", 0, 0, 0, 640, 480, APP_OPTIONS_WINDOW_POS_CENTERED + APP_OPTIONS_RESIZABLE, argv0)
mount_directory_as("/content", "/")
app_set_icon("CF_Logo_Pixel.png")
load_shaders()
shd = make_shader("gray")

dbg_thickness = 0

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

world = b2CreateWorld(b2DefaultWorldDef())

b2CreateBox(0, 0, 300, 20, b2_staticBody) -- floor
for i=1,100 do
	b2CreateBox(0, 20+i*10.5, 10, 10, b2_dynamicBody)
end

def = b2DefaultChainDef()
def.points = { -25,-25, 50,-50, 100,-50, 150,-25, 200,0, 250,50 }
def.isLoop = true
b2CreateChain(b2CreateBody(world, b2DefaultBodyDef()), def)

function lerp(x,a,b)
	return a+(b-a)*x
end

function main()
	while app_is_running() do
		app_update()
		REF_SyncGlobals()
		b2World_Step(world, DELTA_TIME, 4)
		
		b2World_Draw(world, debug_draw)
		
		local e = b2World_GetContactEvents(world)
		for k,v in pairs(e.beginEvents) do
			print("begin event #"..k, v.shapeIdA, v.shapeIdB)
		end
		for k,v in pairs(e.endEvents) do
			print("end event #"..k, v.shapeIdA, v.shapeIdB)
		end
		for k,v in pairs(e.hitEvents) do
			print("hit event #"..k, v.shapeIdA, v.shapeIdB, v.point, v.normal, v.approachSpeed)
		end
		print("---")
		
		app_draw_onto_screen(true)
	end

	destroy_app()
end
