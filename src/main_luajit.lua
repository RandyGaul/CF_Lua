function update() end

function main()
    make_app("cute imgui image test", default_display(), 0, 0, 640, 480, bit.bor(APP_OPTIONS_WINDOW_POS_CENTERED_BIT, APP_OPTIONS_RESIZABLE_BIT), argv0)
    app_init_imgui()
    mount_directory_as("../content", "/")

    sprite = make_demo_sprite()
    assert(sprite)
    while app_is_running() do
        app_update("update")
        draw_sprite(sprite)

        draw_text("Press space", 0, 200, -1)

        imgui_begin("Room Editor", true)
        imgui_button("click me")
        check = imgui_checkbox("yo", check)
        imgui_sprite_button("cf", sprite)
        imgui_end()

        app_draw_onto_screen(true)
    end
end
