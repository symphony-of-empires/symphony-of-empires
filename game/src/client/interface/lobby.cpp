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
//      client/interface/lobby.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <filesystem>
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/group.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/window.hpp"
#include "eng3d/camera.hpp"

#include "client/interface/lobby.hpp"
#include "client/interface/main_menu.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "client/game_state.hpp"
#include "world.hpp"
#include "client/map.hpp"
#include "client/lua_save_util.hpp"

using namespace Interface;

// Start screen
LobbySelectView::LobbySelectView(GameState& _gs)
    : UI::Window(-450, 0, 450, 128),
    gs{ _gs },
    curr_selected_nation{ 0 }
{
    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;

    // Country button for selection
    this->curr_country_btn = new UI::Button(0, 0, 250, 24, this);
    this->curr_country_btn->set_text(translate("No country selected"));
    this->curr_country_btn->set_tooltip(translate("Select a country by clicking on the map or pressing the buttons below"));
    this->curr_country_btn->set_on_click([this](UI::Widget&) {
        if(this->gs.curr_nation != nullptr) {
            this->gs.ui_ctx.clear();
            if(!this->gs.singleplayer) {
                if(this->gs.host_mode) {
                    this->gs.server = std::make_unique<Server>(gs, 1836);
                    this->gs.client = std::make_unique<Client>(gs, "127.0.0.1", 1836);
                    this->gs.client->username = "Host";
                }
            }
            this->gs.in_game = true;
            this->gs.play_nation();
        }
    });

    // Flag with shadow
    this->curr_country_flag_img = new UI::Image(250, 0, 128, 72, gs.tex_man.get_white(), this);
#ifndef E3D_HANDHELD
    this->make_widget<UI::Image>(250, 0, this->curr_country_flag_img->width, this->curr_country_flag_img->height, "gfx/drop_shadow.png");
#endif

    auto& back_btn = this->make_widget<UI::Button>(0, 0, 128, 24);
    back_btn.set_text("Back");
    back_btn.below_of(*curr_country_btn);
    back_btn.set_on_click([this](UI::Widget&) {
        this->kill();
        new Interface::MainMenu(gs);
    });

    auto& prev_country_btn = this->make_widget<UI::Image>(0, 0, 24, 24, "gfx/arrow_left.png");
    prev_country_btn.below_of(*curr_country_btn);
    prev_country_btn.right_side_of(back_btn);
    prev_country_btn.set_on_click([this](UI::Widget&) {
        this->change_nation(this->curr_selected_nation - 1);
    });

    auto& next_country_ibtn = this->make_widget<UI::Image>(0, 0, 24, 24, "gfx/arrow_right.png");
    next_country_ibtn.below_of(*curr_country_btn);
    next_country_ibtn.right_side_of(prev_country_btn);
    next_country_ibtn.set_on_click([this](UI::Widget&) {
        this->change_nation(this->curr_selected_nation + 1);
    });

    auto& random_country_ibtn = this->make_widget<UI::Image>(0, 0, 24, 24, "gfx/noicon.png");
    random_country_ibtn.set_tooltip(translate("Select a random country"));
    random_country_ibtn.below_of(*curr_country_btn);
    random_country_ibtn.right_side_of(next_country_ibtn);
    random_country_ibtn.set_on_click([this](UI::Widget&) {
        this->change_nation(rand() % this->gs.world->nations.size());
    });

    const auto path = std::filesystem::current_path().string();
    auto& savefiles_grp = this->make_widget<UI::Group>(0, 0, 128, gs.height);
    savefiles_grp.below_of(random_country_ibtn);
    savefiles_grp.flex = UI::Flex::COLUMN;
    savefiles_grp.is_scroll = true;
    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        if(!entry.is_directory() && entry.path().extension() == ".sc4") {
            auto savefile_path = entry.path().lexically_relative(path).string();
            auto& savefile_btn = savefiles_grp.make_widget<UI::Button>(0, 0, 128, 24);
            savefile_btn.set_text(savefile_path);
            savefile_btn.set_on_click([this, &savefile_path](UI::Widget&) {
                LUA_util::load(this->gs, savefile_path);
            });
        }
    }
}

// Change nation in start screen
void LobbySelectView::change_nation(size_t id) {
    size_t old_id = curr_selected_nation;
    if(gs.world->nations.empty()) {
        gs.ui_ctx.prompt("Error", "No nations to select");
        return;
    }

    if(id >= gs.world->nations.size())
        id = 0;

    gs.curr_nation = &gs.world->nations[id];
    if(id > old_id) {
        while(gs.curr_nation->exists() == false) {
            id++;
            if(id >= gs.world->nations.size())
                id = 0;
            gs.curr_nation = &gs.world->nations[id];
        }
    } else {
        while(gs.curr_nation->exists() == false) {
            id--;
            if(id >= gs.world->nations.size())
                id = gs.world->nations.size() - 1;
            gs.curr_nation = &gs.world->nations[id];
        }
    }

    curr_selected_nation = id;
    this->curr_country_flag_img->current_texture = this->gs.get_nation_flag(*this->gs.curr_nation);
    this->curr_country_btn->set_text(gs.curr_nation->name.data());
    const auto& capital = gs.world->provinces[gs.curr_nation->capital_id];
    gs.map->camera->set_pos(capital.box_area.right, capital.box_area.bottom);
}
