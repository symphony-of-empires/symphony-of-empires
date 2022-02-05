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
#include "client/interface/settings.hpp"
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
        o.settings_window = new Interface::Settings(o.gs);
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