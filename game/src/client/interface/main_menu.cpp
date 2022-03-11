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

#include "unified_render/print.hpp"
#include "unified_render/log.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/ui/button.hpp"
#include "unified_render/ui/input.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/slider.hpp"
#include "unified_render/ui/checkbox.hpp"
#include "unified_render/ui/group.hpp"
#include "unified_render/ui/image.hpp"
#include "unified_render/ui/close_button.hpp"

#include "client/interface/main_menu.hpp"
#include "client/interface/settings.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "world.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"
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
    conn_btn->text("Connect");
    conn_btn->on_click = ([this](UI::Widget& w) {
        auto& gs = this->gs;
        UnifiedRender::Log::debug("ui", "Okey, connecting to [" + ip_addr_inp->buffer + "]");

        // TODO: Handle when mods differ (i.e checksum not equal to host)
        gs.host_mode = false;

        try {
            gs.client = new Client(gs, this->ip_addr_inp->buffer, 1836);
            gs.client->username = this->username_inp->buffer;
            gs.client->wait_for_snapshot();
            gs.in_game = true;
            return;
        }
        catch(UnifiedRender::Networking::SocketException& e) {
            gs.ui_ctx->prompt("Network layer error", e.what());
        }
        catch(ClientException& e) {
            gs.ui_ctx->prompt("Client error", e.what());
        }
        catch(ServerException& e) {
            gs.ui_ctx->prompt("Server error", e.what());
        }
        delete gs.world;
        delete gs.client;
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*conn_btn);
    close_btn->text("Cancel");
}


//
// Main menu
//
MainMenu::MainMenu(GameState& _gs)
    : UI::Div(250, -270, 300, 540),
    gs{ _gs }
{
    this->origin = UI::Origin::MIDDLE_LEFT_SCREEN;
    this->is_pinned = true;
    this->is_scroll = false;
    // this->text("Symphony of Empires");
    auto tex_man = UnifiedRender::State::get_instance().tex_man;
    UnifiedRender::TextureOptions mipmap_options;
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    mipmap_options.wrap_s = GL_CLAMP_TO_EDGE;
    mipmap_options.wrap_t = GL_CLAMP_TO_EDGE;

    auto font = TTF_OpenFont(Path::get("fonts/neon_euler/euler.ttf").c_str(), 20);
    auto text_color = UnifiedRender::Color(1., 1., 1.);

    this->current_texture = &tex_man->load(Path::get("gfx/ui/bg/main_menu.png"), mipmap_options);
    auto main_menu_border = &tex_man->load(Path::get("gfx/ui/bg/main_menu_border.png"));
    this->border = UI::Border(main_menu_border, glm::ivec2(16), glm::ivec2(16));

    auto logo = &tex_man->load(Path::get("gfx/ui/image/logo.png"), mipmap_options);
    new UI::Image(0, 0, 300, 120, logo, this);

    auto button_image = &tex_man->load(Path::get("gfx/ui/button/button.png"), mipmap_options);
    auto button_border_image = &tex_man->load(Path::get("gfx/ui/button/button_border.png"));
    glm::ivec2 size(3, 3);
    glm::ivec2 texture_size(3, 3);
    auto button_border = UI::Border(button_border_image, size, texture_size);

    auto* button_list = new UI::Div(0, 200, 300, 320, this);
    button_list->flex = UI::Flex::COLUMN;
    button_list->flex_align = UI::Align::CENTER;
    button_list->flex_gap = 8;


    int b_width = 225;
    int b_height = 33;
    auto* single_btn = new UI::Button(0, 0, b_width, b_height, button_list);
    single_btn->border = button_border;
    single_btn->current_texture = button_image;
    single_btn->font = font;
    single_btn->text_color = text_color;
    single_btn->text_align_x = UI::Align::CENTER;
    single_btn->text_align_y = UI::Align::CENTER;
    single_btn->text("Singleplayer");
    single_btn->on_click = ([this](UI::Widget& w) {
        auto& gs = this->gs;

        gs.current_mode = MapMode::COUNTRY_SELECT;
        gs.select_nation = new Interface::LobbySelectView(gs);

        gs.host_mode = true;
        gs.server = new Server(gs, 1836);
        gs.client = new Client(gs, "127.0.0.1", 1836);
        gs.client->username = "Player";
        gs.in_game = true;
        gs.editor = false;

        this->kill();
    });

    auto* mp_btn = new UI::Button(0, 0, b_width, b_height, button_list);
    mp_btn->border = button_border;
    mp_btn->current_texture = button_image;
    mp_btn->font = font;
    mp_btn->text_color = text_color;
    mp_btn->text_align_x = UI::Align::CENTER;
    mp_btn->text_align_y = UI::Align::CENTER;
    mp_btn->text("Join LAN");
    mp_btn->on_click = ([this](UI::Widget& w) {
        this->connect_window = new MainMenuConnectServer(this->gs);
    });

    auto* host_btn = new UI::Button(0, 0, b_width, b_height, button_list);
    host_btn->border = button_border;
    host_btn->current_texture = button_image;
    host_btn->font = font;
    host_btn->text_color = text_color;
    host_btn->text_align_x = UI::Align::CENTER;
    host_btn->text_align_y = UI::Align::CENTER;
    host_btn->text("Host");
    host_btn->on_click = ([this](UI::Widget& w) {
        auto& gs = this->gs;

        gs.current_mode = MapMode::COUNTRY_SELECT;
        gs.select_nation = new Interface::LobbySelectView(gs);

        gs.host_mode = true;
        gs.server = new Server(gs, 1836);
        gs.client = new Client(gs, "127.0.0.1", 1836);
        gs.client->username = "Host";
        gs.in_game = true;

        this->kill();
    });

    auto* edit_btn = new UI::Button(0, 0, b_width, b_height, button_list);
    edit_btn->border = button_border;
    edit_btn->current_texture = button_image;
    edit_btn->font = font;
    edit_btn->text_color = text_color;
    edit_btn->text_align_x = UI::Align::CENTER;
    edit_btn->text_align_y = UI::Align::CENTER;
    edit_btn->text("Editor");
    edit_btn->on_click = ([this](UI::Widget& w) {
        auto& gs = this->gs;

        gs.current_mode = MapMode::COUNTRY_SELECT;
        gs.select_nation = new Interface::LobbySelectView(gs);

        gs.host_mode = true;
        gs.server = new Server(gs, 1836);
        gs.client = new Client(gs, "127.0.0.1", 1836);
        gs.client->username = "Player";
        gs.in_game = true;
        gs.editor = true;

        this->kill();
    });

    auto* cfg_btn = new UI::Button(0, 0, b_width, b_height, button_list);
    cfg_btn->border = button_border;
    cfg_btn->current_texture = button_image;
    cfg_btn->font = font;
    cfg_btn->text_color = text_color;
    cfg_btn->text_align_x = UI::Align::CENTER;
    cfg_btn->text_align_y = UI::Align::CENTER;
    cfg_btn->text("Settings");
    cfg_btn->on_click = ([this](UI::Widget& w) {
        this->settings_window = new Interface::Settings(this->gs);
    });

    auto* exit_btn = new UI::Button(-75, -60, 150, b_height, this);
    exit_btn->border = button_border;
    exit_btn->current_texture = button_image;
    exit_btn->font = font;
    exit_btn->text_align_x = UI::Align::CENTER;
    exit_btn->text_align_y = UI::Align::CENTER;
    exit_btn->text_color = text_color;
    exit_btn->origin = UI::Origin::LOWER_MIDDLE;
    exit_btn->text("Exit");
    exit_btn->on_click = ([this](UI::Widget& w) {
        this->gs.run = false;
    });
}

MainMenu::~MainMenu() {
    if(settings_window != nullptr) {
        settings_window->kill();
    }

    if(connect_window != nullptr) {
        connect_window->kill();
    }
}