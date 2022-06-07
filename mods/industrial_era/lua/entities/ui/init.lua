require('classes/ui')

-- AI Control Window
UI_RegisterCallback("ai_settings_window_invoke", function()
    local ai_sett_win = UI_Window:new(0, 0, 512, 256)
    ai_sett_win:set_scroll(false)
    ai_sett_win:text(_("AI Control"))
    ai_sett_win:set_close_btn_func(function(widget)
        UI_CallBuiltin("gs.ai_control.form_packet")
    end)

    local cmd_chk = UI_Checkbox:new(0, 0, 128, 24, ai_sett_win.id)
    cmd_chk:text(_("Command troops"))
    cmd_chk:set_value(UI_CallBuiltin("gs.ai_do_cmd_troops.get"))
    cmd_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.ai_do_cmd_troops.set", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)
    cmd_chk:set_tooltip(_("Units will be managed by the AI"))
end)

-- Unit type button
ui_unit_types = {}
function unit_type_button(x, y, unit_type_ref_name, parent_id)
    local unit_type = UnitType:get(unit_type_ref_name)
    local width = UI_Widget:get(parent_id).width
    local unit_type_btn = UI_Group:new(x, y, width, 24, parent_id)
    unit_type_btn:set_scroll(false)

    local icon_img = UI_Image:new(0, 0, 32, 24, unit_type_btn.id)
    icon_img:set_image("gfx/unittype/" .. unit_type.ref_name .. ".png")

    local name_btn = UI_Button:new(32, 0, width - 32, 24)
    name_btn:text(_(unit_type.name))

    -- Save on global table
    ui_unit_types[unit_type_btn.id] = unit_type
    return unit_type_btn
end

-- Province button
ui_provinces = {}
function province_button(x, y, province_ref_name, parent_id)
    local province = Province:get(province_ref_name)
    local width = UI_Widget:get(parent_id).width

    local province_btn = UI_Button:new(x, y, width, 24, parent_id)
    province_btn:text(_(province.name))

    -- Save on global table
    ui_provinces[province_btn.id] = province
    return province_btn
end

-- Map dev view
UI_RegisterCallback("map_dev_view_invoke", function()
    local map_dev_win = UI_Window:new(0, 0, 512, 128)
    map_dev_win:set_close_btn_func(function(widget)
        -- ...
    end)
    local reload_shader_btn = UI_Button:new(10, 10, 256, 24, map_dev_win.id)
    reload_shader_btn:text(_("Reload shaders"))
    reload_shader_btn:set_on_click(function(widget)
        UI_CallBuiltin("gs.map.reload_shaders")
    end)
end)

-- Map dev view
UI_RegisterCallback("settings_window_invoke", function()
    local settings_win = UI_Window:new(0, 0, 512, 512)
    settings_win:text(_("Settings"))
    settings_win:set_close_btn_func(function(widget)
        -- ...
    end)

    local graphics_flex = UI_Div:new(0, 0, 512, 512, settings_win.id)
    graphics_flex:set_flex(UI_FlexColumn)

    local sdf_detail_chk = UI_Checkbox:new(0, 0, 256, 24, graphics_flex.id)
    sdf_detail_chk:text(_("Multipass SDF"))
    sdf_detail_chk:set_value(UI_CallBuiltin("gs.shader_opt.get", "SDF"))
    sdf_detail_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.shader_opt.set", "SDF", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)

    local noise_chk = UI_Checkbox:new(0, 0, 256, 24, graphics_flex.id)
    noise_chk:text(_("Noise"))
    noise_chk:set_value(UI_CallBuiltin("gs.shader_opt.get", "NOISE"))
    noise_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.shader_opt.set", "NOISE", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)
    
    local lighting_chk = UI_Checkbox:new(0, 0, 256, 24, graphics_flex.id)
    lighting_chk:text(_("Light and Reflections"))
    lighting_chk:set_value(UI_CallBuiltin("gs.shader_opt.get", "LIGHTING"))
    lighting_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.shader_opt.set", "LIGHTING", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)

    local parallax_chk = UI_Checkbox:new(0, 0, 256, 24, graphics_flex.id)
    parallax_chk:text(_("3D Parallax"))
    parallax_chk:set_value(UI_CallBuiltin("gs.shader_opt.get", "PARALLAX"))
    parallax_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.shader_opt.set", "PARALLAX", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)

    local rivers_chk = UI_Checkbox:new(0, 0, 256, 24, graphics_flex.id)
    rivers_chk:text(_("Advanced Rivers"))
    rivers_chk:set_value(UI_CallBuiltin("gs.shader_opt.get", "RIVERS"))
    rivers_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.shader_opt.set", "RIVERS", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)

    local water_chk = UI_Checkbox:new(0, 0, 256, 24, graphics_flex.id)
    water_chk:text(_("Advanced Water"))
    water_chk:set_value(UI_CallBuiltin("gs.shader_opt.get", "WATER"))
    water_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.shader_opt.set", "WATER", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)

    local grid_chk = UI_Checkbox:new(0, 0, 256, 24, graphics_flex.id)
    grid_chk:text(_("Square grid"))
    grid_chk:set_value(UI_CallBuiltin("gs.shader_opt.get", "GRID"))
    grid_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.shader_opt.set", "GRID", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)

    -- TODO: Motion blur get
    local motion_blur_chk = UI_Checkbox:new(0, 0, 256, 24, graphics_flex.id)
    motion_blur_chk:text(_("Motion blur"))
    motion_blur_chk:set_value(UI_CallBuiltin("gs.motion_blur.get"))
    motion_blur_chk:set_on_click(function(widget)
        UI_CallBuiltin("gs.motion_blur.set", UI_ReinterpretAs(UI_Checkbox, widget):get_value())
    end)

    local music_volume_sld = UI_Slider:new(0, 0, 256, 24, graphics_flex.id)
    music_volume_sld:text(_("Music volume"))
    music_volume_sld:set_value(UI_CallBuiltin("gs.music_volume.get"))
    music_volume_sld:set_on_click(function(widget)
        local volume = UI_ReinterpretAs(UI_Slider, widget):get_value()
        print("Music volume: ", volume)
        UI_CallBuiltin("gs.music_volume.set", volume)
    end)

    local sound_volume_sld = UI_Slider:new(0, 0, 256, 24, graphics_flex.id)
    sound_volume_sld:text(_("Sound volume"))
    sound_volume_sld:set_value(UI_CallBuiltin("gs.sound_volume.get"))
    sound_volume_sld:set_on_click(function(widget)
        local volume = UI_ReinterpretAs(UI_Slider, widget):get_value()
        print("Sound volume: ", volume)
        UI_CallBuiltin("gs.sound_volume.set", volume)
    end)
end)
