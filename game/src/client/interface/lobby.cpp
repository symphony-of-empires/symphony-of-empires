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
#include "client/game_state.hpp"
#include "world.hpp"
#include "client/map.hpp"
#include "io_impl.hpp"

using namespace Interface;

// Start screen
LobbySelectView::LobbySelectView(GameState& _gs)
    : gs{ _gs },
    curr_selected_nation{ 0 }
{
    this->ctrl_window = new UI::Window(-576, 0, 576, 128);
    this->ctrl_window->origin = UI::Origin::UPPER_RIGHT_SCREEN;

    // Flag with shadow
    this->curr_country_flag_img = new UI::Image(0, 0, 38, 28, gs.tex_man.get_white(), ctrl_window);
    auto* drop_shadow_img = new UI::Image(0, 0, 38, 28, "gfx/drop_shadow.png", ctrl_window);

    // Country button for selection
    this->curr_country_btn = new UI::Button(0, 0, 320, 28, ctrl_window);
    curr_country_btn->right_side_of(*this->curr_country_flag_img);
    curr_country_btn->text("Select a country");
    curr_country_btn->set_on_click([this](UI::Widget&) {
        if(this->gs.curr_nation != nullptr) {
            // Didn't seem to be able to delete them in a callback so this will do
            this->ctrl_window->kill();
            this->ctrl_window = nullptr;
            this->gs.play_nation();
        }
    });

    auto* back_btn = new UI::Button(0, 0, 128, 24, ctrl_window);
    back_btn->text("Back");
    back_btn->below_of(*curr_country_btn);
    back_btn->set_on_click([this](UI::Widget&) {
        this->ctrl_window->kill();
        new Interface::MainMenu(gs);
    });

    auto* next_country_btn = new UI::Button(0, 0, 128, 24, ctrl_window);
    next_country_btn->text("Next");
    next_country_btn->below_of(*curr_country_btn);
    next_country_btn->right_side_of(*back_btn);
    next_country_btn->set_on_click([this](UI::Widget&) {
        this->change_nation(this->curr_selected_nation + 1);
    });

    auto* prev_country_btn = new UI::Button(0, 0, 128, 24, ctrl_window);
    prev_country_btn->text("Previous");
    prev_country_btn->below_of(*curr_country_btn);
    prev_country_btn->right_side_of(*next_country_btn);
    prev_country_btn->set_on_click([this](UI::Widget&) {
        this->change_nation(this->curr_selected_nation - 1);
    });

    const std::string path = std::filesystem::current_path().string();
    auto* game_group = new UI::Group(0, 0, 128, gs.height);
    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        if(!entry.is_directory() && entry.path().extension() == ".scv") {
            LoadGameBtnData data{ gs, entry.path().lexically_relative(path).string() };
            ldgame_data.push_back(data);
        }
    }

    size_t i = 0;
    for(const auto& ldgame : ldgame_data) {
        auto* ldgame_btn = new UI::Button(0, 24 * i, 128, 24, game_group);
        ldgame_btn->text(ldgame_data[i].file);
        ldgame_btn->set_on_click([this, ldgame](UI::Widget&) {
            if(ldgame.gs.world != nullptr) delete ldgame.gs.world;
            Archive ar = Archive();
            ::deserialize(ar, &ldgame.gs.world);
            ldgame.gs.map->update_mapmode();
        });
        ++i;
    }
}

// Change nation in start screen
void LobbySelectView::change_nation(size_t id) {
    size_t old_id = curr_selected_nation;
    if(!g_world.nations.size()) {
        gs.ui_ctx.prompt("Error", "No nations to select");
        return;
    }

    if(id >= g_world.nations.size())
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
    this->curr_country_flag_img->current_texture = gs.map->nation_flags[id];
    this->curr_country_btn->text(gs.curr_nation->name.c_str());
    const Province& capital = gs.world->provinces[gs.curr_nation->capital_id];
    gs.map->camera->set_pos(capital.box_area.right, capital.box_area.bottom);
}
