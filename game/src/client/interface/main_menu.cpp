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
//      client/interface/main_menu.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/main_menu.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
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
// Main menu connect server
//
MainMenuConnectServer::MainMenuConnectServer(GameState& _gs)
    : UI::Window(0, 0, 512, 128, nullptr),
    gs{ _gs }
{
    this->is_scroll = false;
    this->text("Internet multiplayer");

    ip_addr_inp = new UI::Input(0, 0, 128, 24, this);
    ip_addr_inp->buffer = "127.0.0.1";
    ip_addr_inp->text(ip_addr_inp->buffer);
    ip_addr_inp->tooltip = new UI::Tooltip(ip_addr_inp, 512, 24);
    ip_addr_inp->tooltip->text("IP Address of the server");

    username_inp = new UI::Input(0, 24, 512, 24, this);
    username_inp->buffer = "Player";
    username_inp->text(username_inp->buffer);
    username_inp->tooltip = new UI::Tooltip(username_inp, 512, 24);
    username_inp->tooltip->text("Your publicly visible username");

    conn_btn = new UI::Button(0, 48, 128, 24, this);
    conn_btn->user_data = this;
    conn_btn->text("Connect");
    conn_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuConnectServer&>(*w.parent);
        print_info("Okey, connecting to [%s]", o.ip_addr_inp->buffer.c_str());

        // TODO: Handle when mods differ (i.e checksum not equal to host)
        o.gs.host_mode = false;

        try {
            o.gs.client = new Client(o.gs, o.ip_addr_inp->buffer, 1836);
            o.gs.client->username = o.username_inp->buffer;
            o.gs.client->wait_for_snapshot();
        } catch(UnifiedRender::Networking::SocketException& e) {
            o.gs.ui_ctx->prompt("Network layer error", e.what());
            goto failure_cleanup;
        } catch(ClientException& e) {
            o.gs.ui_ctx->prompt("Client error", e.what());
            goto failure_cleanup;
        } catch(ServerException& e) {
            o.gs.ui_ctx->prompt("Server error", e.what());
            goto failure_cleanup;
        }
        o.gs.in_game = true;
    failure_cleanup:
        delete o.gs.world;
        delete o.gs.client;
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*conn_btn);
    close_btn->text("Cancel");
}

//
// Main menu settings
//
MainMenuSettings::MainMenuSettings(GameState& _gs)
    : UI::Window(0, 0, 512, 512, nullptr),
    gs{ _gs }
{
    this->is_scroll = false;
    this->text("Settings");

    auto* check_controller_btn = new UI::Button(0, 0, 128, 24, this);
    check_controller_btn->text("Check controller");
    check_controller_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        if(o.gs.joy != nullptr) {
            SDL_JoystickClose(o.gs.joy);
        }
        
        if(SDL_NumJoysticks() >= 1) {
            o.gs.joy = SDL_JoystickOpen(0);
        } else {
            o.gs.ui_ctx->prompt("Gamepad", "No present joysticks");
        }
    });
    check_controller_btn->tooltip = new UI::Tooltip(check_controller_btn, 512, 24);
    check_controller_btn->tooltip->text("Checks for any new gamepads - click this if you've connected a gamepad and experience issues");

    auto* sensivity_sld = new UI::Slider(0, 0, 128, 24, -8000.f, 8000.f, this);
    sensivity_sld->text("Controller Sensivity");
    sensivity_sld->below_of(*check_controller_btn);
    sensivity_sld->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        o.gs.joy_sensivity = ((UI::Slider&)w).value;
    });
    sensivity_sld->tooltip = new UI::Tooltip(sensivity_sld, 512, 24);
    sensivity_sld->tooltip->text("Sensivity of the controller (negative/positive inverts controls)");

    auto* sdf_detail_chk = new UI::Checkbox(0, 0, 128, 24, this);
    sdf_detail_chk->below_of(*sensivity_sld);
    sdf_detail_chk->value = gs.map->map_render->options.sdf.used;
    sdf_detail_chk->text("SDF detail");
    sdf_detail_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.sdf.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    sdf_detail_chk->tooltip = new UI::Tooltip(sdf_detail_chk, 512, 24);
    sdf_detail_chk->tooltip->text("Enables/Disables SDF detail on the map. High performance impact");

    auto* noise_chk = new UI::Checkbox(0, 0, 128, 24, this);
    noise_chk->below_of(*sdf_detail_chk);
    noise_chk->value = gs.map->map_render->options.noise.used;
    noise_chk->text("Noise");
    noise_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.noise.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    noise_chk->tooltip = new UI::Tooltip(noise_chk, 512, 24);
    noise_chk->tooltip->text("Adds noise to the map, giving it more \"natural\" feel. Low performance impact");

    auto* raytracing_chk = new UI::Checkbox(0, 0, 128, 24, this);
    raytracing_chk->below_of(*noise_chk);
    raytracing_chk->value = gs.map->map_render->options.lighting.used;
    raytracing_chk->text("Simple Raytracing");
    raytracing_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.lighting.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    raytracing_chk->tooltip = new UI::Tooltip(raytracing_chk, 512, 24);
    raytracing_chk->tooltip->text("Creates vibrant lighting through the map. Low performance impact");

    auto* diag_borders_chk = new UI::Checkbox(0, 0, 128, 24, this);
    diag_borders_chk->below_of(*raytracing_chk);
    diag_borders_chk->value = gs.map->map_render->options.diag_borders.used;
    diag_borders_chk->text("Diagonal borders");
    diag_borders_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.diag_borders.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    diag_borders_chk->tooltip = new UI::Tooltip(diag_borders_chk, 512, 24);
    diag_borders_chk->tooltip->text("Enables/Disables replacing the blocky borders with diagonal ones. Low performance impact");

    auto* parallax_chk = new UI::Checkbox(0, 0, 128, 24, this);
    parallax_chk->below_of(*diag_borders_chk);
    parallax_chk->value = gs.map->map_render->options.parallax.used;
    parallax_chk->text("Parallax");
    parallax_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.parallax.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    parallax_chk->tooltip = new UI::Tooltip(parallax_chk, 512, 24);
    parallax_chk->tooltip->text("Enables/Disables a parallax map. High performance impact");

    auto* rivers_chk = new UI::Checkbox(0, 0, 128, 24, this);
    rivers_chk->below_of(*parallax_chk);
    rivers_chk->value = gs.map->map_render->options.rivers.used;
    rivers_chk->text("Rivers");
    rivers_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.rivers.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    rivers_chk->tooltip = new UI::Tooltip(rivers_chk, 512, 24);
    rivers_chk->tooltip->text("Enables/Disables rivers. Low performance impact");

    auto* landscape_chk = new UI::Checkbox(0, 0, 128, 24, this);
    landscape_chk->below_of(*rivers_chk);
    landscape_chk->value = gs.map->map_render->options.landscape.used;
    landscape_chk->text("Landscape");
    landscape_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.landscape.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    landscape_chk->tooltip = new UI::Tooltip(landscape_chk, 512, 24);
    landscape_chk->tooltip->text("Enables/Disables the landscape image (for terrain). Low performance impact");

    auto* grid_chk = new UI::Checkbox(0, 0, 128, 24, this);
    grid_chk->below_of(*landscape_chk);
    grid_chk->value = gs.map->map_render->options.grid.used;
    grid_chk->text("Gridlines");
    grid_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.grid.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    grid_chk->tooltip = new UI::Tooltip(grid_chk, 512, 24);
    grid_chk->tooltip->text("Enables/Disables the grid lines on the water. Low performance impact");

    auto* water_chk = new UI::Checkbox(0, 0, 128, 24, this);
    water_chk->below_of(*grid_chk);
    water_chk->value = gs.map->map_render->options.water.used;
    water_chk->text("Water texture");
    water_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.map->map_render->options.water.used = ((UI::Checkbox&)w).value;
        o.gs.map->reload_shaders();
    });
    water_chk->tooltip = new UI::Tooltip(water_chk, 512, 24);
    water_chk->tooltip->text("Enables/Disables the water texture (for terrain). Low performance impact");

    auto* motionblur_chk = new UI::Checkbox(0, 0, 128, 24, this);
    motionblur_chk->below_of(*water_chk);
    motionblur_chk->value = gs.motion_blur;
    motionblur_chk->text("Motion blur");
    motionblur_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        ((UI::Checkbox&)w).value = !((UI::Checkbox&)w).value;
        o.gs.motion_blur = ((UI::Checkbox&)w).value;
    });
    motionblur_chk->tooltip = new UI::Tooltip(motionblur_chk, 512, 24);
    motionblur_chk->tooltip->text("Control if motion blur should be enabled");

    auto* music_volume_sld = new UI::Slider(0, 0, 128, 24, -8000.f, 8000.f, this);
    music_volume_sld->text("Music volume");
    music_volume_sld->below_of(*motionblur_chk);
    music_volume_sld->value = gs.music_volume;
    music_volume_sld->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        o.gs.music_volume = ((UI::Slider&)w).value;
    });
    music_volume_sld->tooltip = new UI::Tooltip(music_volume_sld, 512, 24);
    music_volume_sld->tooltip->text("Controls the volume of the music");

    auto* sound_volume_sld = new UI::Slider(0, 0, 128, 24, -8000.f, 8000.f, this);
    sound_volume_sld->text("Sound volume");
    sound_volume_sld->below_of(*music_volume_sld);
    sound_volume_sld->value = gs.sound_volume;
    sound_volume_sld->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenuSettings&>(*w.parent);
        o.gs.sound_volume = ((UI::Slider&)w).value;
    });
    sound_volume_sld->tooltip = new UI::Tooltip(sound_volume_sld, 512, 24);
    sound_volume_sld->tooltip->text("Controls the volume of the sounds");

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*sound_volume_sld);
    close_btn->text("Cancel");
}

//
// Main menu
//
MainMenu::MainMenu(GameState& _gs)
    : UI::Window(0, -128, 400, 128),
    gs{ _gs }
{
    this->origin = UI::Origin::LOWER_LEFT_SCREEN;
    this->is_pinned = true;
    this->is_scroll = false;
    this->text("Symphony of Empires");
    
    auto* single_btn = new UI::Button(0, 0, 128, 24, this);
    single_btn->text("Singleplayer");
    single_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenu&>(*w.parent);

        o.gs.current_mode = MapMode::COUNTRY_SELECT;
        o.gs.select_nation = new Interface::LobbySelectView(o.gs);

        o.gs.host_mode = true;
        o.gs.server = new Server(o.gs, 1836);
        o.gs.client = new Client(o.gs, "127.0.0.1", 1836);
        o.gs.client->username = "Player";
        o.gs.in_game = true;
        o.gs.editor = false;

        o.kill();
    });

    auto* mp_btn = new UI::Button(0, 0, 128, 24, this);
    mp_btn->text("Join LAN");
    mp_btn->right_side_of(*single_btn);
    mp_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenu&>(*w.parent);
        o.connect_window = new MainMenuConnectServer(o.gs);
    });

    auto* host_btn = new UI::Button(0, 0, 128, 24, this);
    host_btn->text("Host");
    host_btn->right_side_of(*mp_btn);
    host_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenu&>(*w.parent);

        o.gs.current_mode = MapMode::COUNTRY_SELECT;
        o.gs.select_nation = new Interface::LobbySelectView(o.gs);

        o.gs.host_mode = true;
        o.gs.server = new Server(o.gs, 1836);
        o.gs.client = new Client(o.gs, "127.0.0.1", 1836);
        o.gs.client->username = "Host";
        o.gs.in_game = true;

        o.kill();
    });

    auto* edit_btn = new UI::Button(0, 0, 128, 24, this);
    edit_btn->below_of(*host_btn);
    edit_btn->text("Editor");
    edit_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenu&>(*w.parent);

        o.gs.current_mode = MapMode::COUNTRY_SELECT;
        o.gs.select_nation = new Interface::LobbySelectView(o.gs);

        o.gs.host_mode = true;
        o.gs.server = new Server(o.gs, 1836);
        o.gs.client = new Client(o.gs, "127.0.0.1", 1836);
        o.gs.client->username = "Player";
        o.gs.in_game = true;
        o.gs.editor = true;

        o.kill();
    });

    auto* cfg_btn = new UI::Button(0, 0, 128, 24, this);
    cfg_btn->text("Settings");
    cfg_btn->below_of(*host_btn);
    cfg_btn->right_side_of(*edit_btn);
    cfg_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenu&>(*w.parent);
        o.settings_window = new MainMenuSettings(o.gs);
    });

    auto* exit_btn = new UI::Button(0, 0, 128, 24, this);
    exit_btn->below_of(*host_btn);
    exit_btn->right_side_of(*cfg_btn);
    exit_btn->text("Exit");
}

MainMenu::~MainMenu() {
    if(settings_window != nullptr) {
        settings_window->kill();
    }

    if(connect_window != nullptr) {
        connect_window->kill();
    }
}