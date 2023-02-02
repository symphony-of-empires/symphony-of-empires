// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/main_menu.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ttf.hpp"
#include "eng3d/log.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/input.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/slider.hpp"
#include "eng3d/ui/checkbox.hpp"
#include "eng3d/ui/group.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/close_button.hpp"

#include "client/interface/main_menu.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "world.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"
#include "client/interface/lobby.hpp"

//
// Main menu connect server
//
Interface::MainMenuConnectServer::MainMenuConnectServer(GameState& _gs)
    : UI::Window(0, 0, 512, 128),
    gs{ _gs }
{
    this->is_scroll = false;
    this->set_text("Internet multiplayer");
    this->set_close_btn_function([this](Widget&) {
        this->kill();
    });

    ip_addr_inp = new UI::Input(0, 0, 128, 24, this);
    ip_addr_inp->set_buffer("127.0.0.1");
    ip_addr_inp->set_tooltip("IP Address of the server\n"
                             "127.x.x.x: Local host\n"
                             "192.x.x.x: LAN IP range");

    username_inp = new UI::Input(0, 24, 512, 24, this);
    username_inp->set_buffer("PlayerUsername");
    username_inp->set_tooltip("Your publicly visible username");

    conn_btn = new UI::Button(0, 48, 128, 24, this);
    conn_btn->set_text("Connect");
    conn_btn->set_on_click([this](UI::Widget&) {
        Eng3D::Log::debug("ui", translate_format("Connecting to server on IP %s", this->ip_addr_inp->get_buffer().data()));
        /// @todo Handle when mods differ (i.e checksum not equal to host)
        this->gs.host_mode = false;
        this->gs.editor = false;
        try {
            this->gs.ui_ctx.clear();
            this->gs.current_mode = MapMode::COUNTRY_SELECT;
            this->gs.select_nation = new Interface::LobbySelectView(gs);
            this->gs.client = std::make_unique<Client>(this->gs, this->ip_addr_inp->get_buffer(), 1836);
            this->gs.client->username = this->username_inp->get_buffer();
            this->gs.client->wait_for_snapshot();
            return;
        } catch(Eng3D::Networking::SocketException& e) {
            this->gs.ui_ctx.prompt("Network layer error", e.what());
        } catch(ClientException& e) {
            this->gs.ui_ctx.prompt("Client error", e.what());
        } catch(ServerException& e) {
            this->gs.ui_ctx.prompt("Server error", e.what());
        }
        this->gs.client.reset();
    });
}

//
// Main menu
//
Interface::MainMenu::MainMenu(GameState& _gs)
    : UI::Div(250, -270, 300, 540),
    gs{ _gs }
{
    this->origin = UI::Origin::MIDDLE_LEFT_SCREEN;
    this->is_pinned = true;
    this->is_scroll = false;
    
    auto menu_font = gs.ttf_man.load(gs.package_man.get_unique("fonts/neon_euler/euler.ttf"));
    auto menu_text_color = Eng3D::Color(1.f, 1.f, 1.f);

    this->current_texture = gs.tex_man.load(gs.package_man.get_unique("gfx/ui/bg/main_menu.png"));
    auto main_menu_border = gs.tex_man.load(gs.package_man.get_unique("gfx/ui/bg/main_menu_border.png"));
    this->border = UI::Border(main_menu_border, glm::ivec2(16), glm::ivec2(16));
    this->make_widget<UI::Image>(0, 0, 300, 120, "gfx/ui/image/logo.png");

    auto button_image = gs.tex_man.load(gs.package_man.get_unique("gfx/ui/button/button.png"));
    auto button_border_image = gs.tex_man.load(gs.package_man.get_unique("gfx/ui/button/button_border.png"));
    glm::ivec2 size(3, 3);
    glm::ivec2 texture_size(3, 3);
    auto button_border = UI::Border(button_border_image, size, texture_size);

    auto& button_list = this->make_widget<UI::Div>(0, 200, 300, 320);
    button_list.flex = UI::Flex::COLUMN;
    button_list.flex_align = UI::Align::CENTER;
    button_list.flex_gap = 8;

    int b_width = 225;
    int b_height = 33;

    auto& demo_btn = button_list.make_widget<UI::Button>(0, 0, b_width, b_height);
    demo_btn.border = button_border;
    demo_btn.current_texture = button_image;
    demo_btn.font = menu_font;
    demo_btn.text_color = menu_text_color;
    demo_btn.text_align_x = UI::Align::CENTER;
    demo_btn.text_align_y = UI::Align::CENTER;
    demo_btn.set_text("Pre-alpha demo");
    demo_btn.set_on_click([this](UI::Widget&) {
        this->gs.ui_ctx.clear();
        this->gs.current_mode = MapMode::COUNTRY_SELECT;
        this->gs.host_mode = true;
        this->gs.editor = false;
        this->gs.singleplayer = true;

        for(auto& nation : this->gs.world->nations)
            if(nation.ref_name.get_string() == "netherlands")
                this->gs.curr_nation = &nation;
        
        if(this->gs.curr_nation == nullptr)
            return;

        this->gs.ui_ctx.clear();
        this->gs.in_game = true;
        this->gs.play_nation();
    });

    auto& single_btn = button_list.make_widget<UI::Button>(0, 0, b_width, b_height);
    single_btn.border = button_border;
    single_btn.current_texture = button_image;
    single_btn.font = menu_font;
    single_btn.text_color = menu_text_color;
    single_btn.text_align_x = single_btn.text_align_y = UI::Align::CENTER;
    single_btn.set_text("Singleplayer");
    single_btn.set_on_click([this](UI::Widget&) {
        this->gs.ui_ctx.clear();
        gs.current_mode = MapMode::COUNTRY_SELECT;
        gs.select_nation = new Interface::LobbySelectView(gs);
        gs.host_mode = true;
        gs.editor = false;
        this->gs.singleplayer = true;
    });

    auto& mp_btn = button_list.make_widget<UI::Button>(0, 0, b_width, b_height);
    mp_btn.border = button_border;
    mp_btn.current_texture = button_image;
    mp_btn.font = menu_font;
    mp_btn.text_color = menu_text_color;
    mp_btn.text_align_x = mp_btn.text_align_y = UI::Align::CENTER;
    mp_btn.set_text("Multiplayer");
    mp_btn.set_on_click([this](UI::Widget&) {
        this->gs.singleplayer = false;
        this->connect_window = new Interface::MainMenuConnectServer(this->gs);
    });

    auto& host_btn = button_list.make_widget<UI::Button>(0, 0, b_width, b_height);
    host_btn.border = button_border;
    host_btn.current_texture = button_image;
    host_btn.font = menu_font;
    host_btn.text_color = menu_text_color;
    host_btn.text_align_x = host_btn.text_align_y = UI::Align::CENTER;
    host_btn.set_text("Host");
    host_btn.set_on_click([this](UI::Widget&) {
        this->gs.ui_ctx.clear();
        this->gs.current_mode = MapMode::COUNTRY_SELECT;
        this->gs.select_nation = new Interface::LobbySelectView(this->gs);
        this->gs.host_mode = true;
        this->gs.editor = false;
        this->gs.singleplayer = false;
    });

    auto& edit_btn = button_list.make_widget<UI::Button>(0, 0, b_width, b_height);
    edit_btn.border = button_border;
    edit_btn.current_texture = button_image;
    edit_btn.font = menu_font;
    edit_btn.text_color = menu_text_color;
    edit_btn.text_align_x = edit_btn.text_align_y = UI::Align::CENTER;
    edit_btn.set_text("Editor");
    edit_btn.set_on_click([this](UI::Widget&) {
        this->gs.current_mode = MapMode::NORMAL;
        // Create a local server in editor mode
        this->gs.host_mode = true;
        this->gs.editor = true;
        this->gs.singleplayer = true;
        this->gs.server.reset(new Server(gs, 1836));
        this->gs.client.reset(new Client(gs, "127.0.0.1", 1836));
        this->gs.ui_ctx.clear();
        this->gs.curr_nation = &gs.world->nations[1];
        this->gs.play_nation();
    });

    auto& cfg_btn = button_list.make_widget<UI::Button>(0, 0, b_width, b_height);
    cfg_btn.border = button_border;
    cfg_btn.current_texture = button_image;
    cfg_btn.font = menu_font;
    cfg_btn.text_color = menu_text_color;
    cfg_btn.text_align_x = cfg_btn.text_align_y = UI::Align::CENTER;
    cfg_btn.set_text("Settings");
    cfg_btn.set_on_click([this](UI::Widget&) {
        this->gs.world->lua.invoke_registered_callback("settings_window_invoke");
    });

    auto& exit_btn = button_list.make_widget<UI::Button>(0, 0, b_width, b_height);
    exit_btn.border = button_border;
    exit_btn.current_texture = button_image;
    exit_btn.font = menu_font;
    exit_btn.text_color = menu_text_color;
    exit_btn.text_align_x = exit_btn.text_align_y = UI::Align::CENTER;
    exit_btn.set_text("Exit");
    exit_btn.set_on_click([this](UI::Widget&) {
        this->gs.paused = true;
        this->gs.run = false;
    });
}

Interface::MainMenu::~MainMenu() {
    if(connect_window != nullptr)
        connect_window->kill();
}
