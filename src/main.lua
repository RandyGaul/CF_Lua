w = 480
h = 270
sx = 3
sy = 3
make_app(nil, 0, 0, 0, 0, 0, APP_OPTIONS_WINDOW_POS_CENTERED_BIT, argv0)

function rectangle(x, y, w, h, rs, color, line_width)
  if color then
    draw_push_color(color.r, color.g, color.b, color.a)
  end
  local w, h = w - 2*(rs or 0), h - 2*(rs or 0)
  if not line_width then
    draw_box_fill(x - w/2, y - h/2, x + w/2, y + h/2, rs or 0)
  else
    draw_box(x - w/2, y - h/2, x + w/2, y + h/2, line_width, rs or 0)
  end
  if color then draw_pop_color() end
end

-- "Simplest" fix, but rather innefficient.
function draw_image_inefficient(canvas, image, x, y, r, sx, sy, color)
  if color then
    render_to(canvas, false)
    render_settings_push_shader(shader)
    draw_push_vertex_attributes(color.r, color.g, color.b, color.a)
  end
  sprite_set_offset_x(image, x)
  sprite_set_offset_y(image, y)
  push(x, y, r, sx, sy)
    draw_sprite(image)
  pop()
  if color then
    render_to(canvas, false)
    draw_pop_vertex_attributes()
    render_settings_pop_shader()
  end
end

-- Avoid swapping shaders, more efficient.
function draw_image_suggested(canvas, image, x, y, r, sx, sy, color)
  if color then
    draw_push_vertex_attributes(color.r, color.g, color.b, color.a)
  end
  sprite_set_offset_x(image, x)
  sprite_set_offset_y(image, y)
  push(x, y, r, sx, sy)
    draw_sprite(image)
  pop()
  if color then
    draw_pop_vertex_attributes()
  end
end

draw_image = draw_image_suggested

function push(x, y, r, sx, sy)
  draw_push()
  draw_translate(x or 0, y or 0)
  draw_scale(sx or 1, sy or sx or 1)
  draw_rotate(-(r or 0))
  draw_translate(-(x or 0), -(y or 0))
end

function pop()
  draw_pop()
end

function main()
  mount_directory_as('/content', '/')
	print(fs_file_exists("/player.ase"))
	local s = fs_read_entire_file_to_memory("/player.ase")
	print(s)
  make_sprite("player.ase")
  app_set_title('Test')
  app_set_size(w*sx, h*sy)
  app_set_canvas_size(w*sx, h*sy)
  display_width, display_height = display_width(default_display()), display_height(default_display())
  app_center_window()

  hit_effect_2 = make_easy_sprite('hit_effect_2.png')
  shader = make_draw_shader_from_source([[
    vec4 shader(vec4 color, vec2 pos, vec2 atlas_us, vec2 screen_uv, vec4 params) {
      return params.a == 0 ? color : vec4(params.rgb, color.a);
    }
  ]])

  blue = {r = 0, g = 0, b = 1, a = 1}
  red = {r = 1, g = 0, b = 0, a = 1}
  white = {r = 1, g = 1, b = 1, a = 1}

  set_fixed_timestep(144)
  set_fixed_timestep_max_updates(10)
  set_target_framerate(display_refresh_rate(default_display()))

  local params = canvas_defaults(480, 270)
  canvas = make_canvas(params)
  screen_canvas = make_canvas(params)
  clear_color(1, 1, 1, 1)
  draw_projection(ortho_2d(0, 0, 480, 270))

  while app_is_running() do
    app_update()
    REF_SyncGlobals()
    draw_push_antialias(false)
    draw_push()
    draw_translate(-480/2, 270/2)
    draw_scale(1, -1)
    draw_push_vertex_attributes(0,0,0,0)
    render_settings_push_shader(shader)

    rectangle(w/2, h/2, 300, 200, 0, white)
    rectangle(w/2 - 80, h/2, 40, 10, 0, red)
    rectangle(w/2 + 80, h/2, 40, 10, 0, red)
    if key_down(KEY_K) then
      draw_image(canvas, hit_effect_2, w/2, h/2, 0, 1, 1, blue)
    end

	if key_just_pressed(KEY_SPACE) then
		if draw_image == draw_image_suggested then
			print("Using draw_image_inefficient.")
			draw_image = draw_image_inefficient
		else
			print("Using draw_image_suggested.")
			draw_image = draw_image_suggested
		end
	end

    draw_pop_antialias()
    draw_pop()

    canvas_blit(canvas, 0, 0, 1, 1, screen_canvas, 0, 0, 1, 1)
    canvas_blit(screen_canvas, 0, 0, 1, 1, app_get_canvas(), 0, 0, 1, 1)
    local draw_call_count = app_draw_onto_screen(false)
	print(draw_call_count)
  end
  destroy_app()
end
