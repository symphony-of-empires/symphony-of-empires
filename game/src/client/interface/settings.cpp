// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/settings.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/settings.hpp"
#include "world.hpp"
#include "unified_render/print.hpp"
#include "unified_render/path.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/ui/button.hpp"
#include "unified_render/ui/input.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/slider.hpp"
#include "unified_render/ui/checkbox.hpp"
#include "unified_render/ui/close_button.hpp"
#include "client/interface/lobby.hpp"

using namespace Interface;

//
// Main menu settings
//
Settings::Settings(GameState& _gs)
    : UI::Window(0, 0, 512, 512, nullptr),
    gs{ _gs }
{
    this->is_scroll = false;
    this->text("Settings");

    auto* check_controller_btn = new UI::Button(0, 0, 128, 24, this);
    check_controller_btn->text("Check controller");
    check_controller_btn->set_on_click([this](UI::Widget& w) {
        if(this->gs.joy != nullptr) {
            SDL_JoystickClose(this->gs.joy);
        }
        
        if(SDL_NumJoysticks() >= 1) {
            this->gs.joy = SDL_JoystickOpen(0);
        } else {
            this->gs.ui_ctx->prompt("Gamepad", "No present joysticks");
        }
    });
    check_controller_btn->set_tooltip("Checks for any new gamepads - click this if you've connected a gamepad and experience issues");

    auto* sensivity_sld = new UI::Slider(0, 0, 128, 24, -8000.f, 8000.f, this);
    sensivity_sld->text("Controller Sensivity");
    sensivity_sld->below_of(*check_controller_btn);
    sensivity_sld->set_on_click([this](UI::Widget& w) {
        this->gs.joy_sensivity = ((UI::Slider&)w).value;
    });
    sensivity_sld->set_tooltip("Sensivity of the controller (negative/positive inverts controls)");

    auto* sdf_detail_chk = new UI::Checkbox(this);
    sdf_detail_chk->below_of(*sensivity_sld);
    sdf_detail_chk->set_value(gs.map->map_render->options.sdf.used);
    sdf_detail_chk->text("SDF detail");
    sdf_detail_chk->set_on_click([this](UI::Widget& w) {
        this->gs.map->map_render->options.sdf.used = ((UI::Checkbox&)w).get_value();
        this->gs.map->reload_shaders();
    });
    sdf_detail_chk->set_tooltip("Enables/Disables SDF detail on the map. High performance impact");

    auto* noise_chk = new UI::Checkbox(this);
    noise_chk->below_of(*sdf_detail_chk);
    noise_chk->set_value(gs.map->map_render->options.noise.used);
    noise_chk->text("Noise");
    noise_chk->set_on_click([this](UI::Widget& w) {
        this->gs.map->map_render->options.noise.used = ((UI::Checkbox&)w).get_value();
        this->gs.map->reload_shaders();
    });
    noise_chk->set_tooltip("Adds noise to the map, giving it more \"natural\" feel. Low performance impact");

    auto* raytracing_chk = new UI::Checkbox(this);
    raytracing_chk->below_of(*noise_chk);
    raytracing_chk->set_value(gs.map->map_render->options.lighting.used);
    raytracing_chk->text("Simple Raytracing");
    raytracing_chk->set_on_click([this](UI::Widget& w) {
        this->gs.map->map_render->options.lighting.used = ((UI::Checkbox&)w).get_value();
        this->gs.map->reload_shaders();
    });
    raytracing_chk->set_tooltip("Creates vibrant lighting through the map. Low performance impact");

    auto* parallax_chk = new UI::Checkbox(this);
    parallax_chk->below_of(*raytracing_chk);
    parallax_chk->set_value(gs.map->map_render->options.parallax.used);
    parallax_chk->text("Parallax");
    parallax_chk->set_on_click([this](UI::Widget& w) {
        this->gs.map->map_render->options.parallax.used = ((UI::Checkbox&)w).get_value();
        this->gs.map->reload_shaders();
    });
    parallax_chk->set_tooltip("Enables/Disables a parallax map. High performance impact");

    auto* rivers_chk = new UI::Checkbox(this);
    rivers_chk->below_of(*parallax_chk);
    rivers_chk->set_value(gs.map->map_render->options.rivers.used);
    rivers_chk->text("Rivers");
    rivers_chk->set_on_click([this](UI::Widget& w) {
        this->gs.map->map_render->options.rivers.used = ((UI::Checkbox&)w).get_value();
        this->gs.map->reload_shaders();
    });
    rivers_chk->set_tooltip("Enables/Disables rivers. Low performance impact");

    auto* grid_chk = new UI::Checkbox(this);
    grid_chk->below_of(*rivers_chk);
    grid_chk->set_value(gs.map->map_render->options.grid.used);
    grid_chk->text("Gridlines");
    grid_chk->set_on_click([this](UI::Widget& w) {
        this->gs.map->map_render->options.grid.used = ((UI::Checkbox&)w).get_value();
        this->gs.map->reload_shaders();
    });
    grid_chk->set_tooltip("Enables/Disables the grid lines on the water. Low performance impact");

    auto* water_chk = new UI::Checkbox(this);
    water_chk->below_of(*grid_chk);
    water_chk->set_value(gs.map->map_render->options.water.used);
    water_chk->text("Water texture");
    water_chk->set_on_click([this](UI::Widget& w) {
        this->gs.map->map_render->options.water.used = ((UI::Checkbox&)w).get_value();
        this->gs.map->reload_shaders();
    });
    water_chk->set_tooltip("Enables/Disables the water texture (for terrain). Low performance impact");

    auto* motionblur_chk = new UI::Checkbox(this);
    motionblur_chk->below_of(*water_chk);
    motionblur_chk->set_value(gs.motion_blur);
    motionblur_chk->text("Motion blur");
    motionblur_chk->set_on_click([this](UI::Widget& w) {
        this->gs.motion_blur = ((UI::Checkbox&)w).get_value();
    });
    motionblur_chk->set_tooltip("Control if motion blur should be enabled");

    auto* music_volume_sld = new UI::Slider(0, 0, 128, 24, 0.f, 100.f, this);
    music_volume_sld->text("Music volume");
    music_volume_sld->below_of(*motionblur_chk);
    music_volume_sld->value = gs.music_volume;
    music_volume_sld->set_on_click([this](UI::Widget& w) {
        this->gs.music_volume = static_cast<int>(((UI::Slider&)w).value);
    });
    music_volume_sld->set_tooltip("Controls the volume of the music");

    auto* sound_volume_sld = new UI::Slider(0, 0, 128, 24, 0.f, 100.f, this);
    sound_volume_sld->text("Sound volume");
    sound_volume_sld->below_of(*music_volume_sld);
    sound_volume_sld->value = gs.sound_volume;
    sound_volume_sld->set_on_click([this](UI::Widget& w) {
        this->gs.sound_volume = static_cast<int>(((UI::Slider&)w).value);
    });
    sound_volume_sld->set_tooltip("Controls the volume of the sounds");

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*sound_volume_sld);
    close_btn->text("Cancel");
}