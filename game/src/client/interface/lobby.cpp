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

#include "client/interface/lobby.hpp"
#include "client/game_state.hpp"
#include "unified_render/ui/ui.hpp"
#include "world.hpp"
#include "client/map.hpp"
#include "client/camera.hpp"
#include <filesystem>
#include "io_impl.hpp"

#include "unified_render/ui/button.hpp"
#include "unified_render/ui/label.hpp"
#include "unified_render/ui/group.hpp"

using namespace Interface;

// Start screen
LobbySelectView::LobbySelectView(GameState& _gs)
    : gs{ _gs },
    curr_selected_nation{ 0 }
{
    select_country_lab = new UI::Button(-(320 / 2), 8, 320, 24);
    select_country_lab->origin = UI::Origin::UPPER_MIDDLE_SCREEN;
    select_country_lab->text("Select a country");

    curr_country_btn = new UI::Button(-(320 / 2), -48, 320, 24);
    curr_country_btn->origin = UI::Origin::LOWER_MIDDLE_SCREEN;
    curr_country_btn->text("No selected");
    curr_country_btn->on_click = ([this](UI::Widget& w) {
        if(this->gs.curr_nation != nullptr) {
            // Didn't seem to be able to delete them in a callback so this will do
            this->next_country_btn->kill();
            this->next_country_btn = nullptr;
            this->prev_country_btn->kill();
            this->prev_country_btn = nullptr;
            this->select_country_lab->kill();
            this->select_country_lab = nullptr;
            this->curr_country_btn->kill();
            this->curr_country_btn = nullptr;
            this->gs.play_nation();
        }
    });

    next_country_btn = new UI::Button(0, -48, 128, 24);
    next_country_btn->origin = UI::Origin::LOWER_MIDDLE_SCREEN;
    next_country_btn->text("Next");
    next_country_btn->right_side_of(*curr_country_btn);
    next_country_btn->user_data = this;
    next_country_btn->on_click = ([this](UI::Widget& w) {
        this->change_nation(this->curr_selected_nation + 1);
    });

    prev_country_btn = new UI::Button(0, -48, 128, 24);
    prev_country_btn->origin = UI::Origin::LOWER_MIDDLE_SCREEN;
    prev_country_btn->text("Prev");
    prev_country_btn->left_side_of(*curr_country_btn);
    prev_country_btn->on_click = ([this](UI::Widget& w) {
        this->change_nation(this->curr_selected_nation - 1);
    });

    const std::string path = std::filesystem::current_path().string();
    auto* game_group = new UI::Group(0, 0, 128, gs.height);
    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        if(!entry.is_directory() && entry.path().extension() == ".scv") {
            LoadGameBtnData data{ gs };
            data.file = entry.path().lexically_relative(path).string();
            ldgame_data.push_back(data);
        }
    }

    size_t i = 0;
    for(const auto& ldgame : ldgame_data) {
        auto* ldgame_btn = new UI::Button(0, 24 * i, 128, 24, game_group);
        ldgame_btn->text(ldgame_data[i].file);
        auto data = &ldgame_data[i];
        ldgame_btn->on_click = ([data](UI::Widget& w){
            std::scoped_lock lock1(data->gs.world->world_mutex);

            if(data->gs.world != nullptr) {
                delete data->gs.world;
            }

            Archive ar = Archive();
            data->gs.world = new World();
            ::deserialize(ar, data->gs.world);
            data->gs.world->load_mod();

            data->gs.map->update_mapmode();
        });
        ++i;
    }
}

// Change nation in start screen
void LobbySelectView::change_nation(size_t id) {
    size_t old_id = curr_selected_nation;
    if(!g_world->nations.size()) {
        gs.ui_ctx->prompt("Error", "No nations to select");
        return;
    }

    if(id >= g_world->nations.size()) {
        id = 0;
    }

    gs.curr_nation = gs.world->nations[id];
    if(id > old_id) {
        while(gs.curr_nation->exists() == false) {
            id++;
            if(id >= gs.world->nations.size()) {
                id = 0;
            }
            gs.curr_nation = gs.world->nations[id];
        }
    }
    else {
        while(gs.curr_nation->exists() == false) {
            id--;
            if(id >= gs.world->nations.size()) {
                id = gs.world->nations.size() - 1;
            }
            gs.curr_nation = gs.world->nations[id];
        }
    }

    curr_selected_nation = id;
    curr_country_btn->text(gs.curr_nation->name.c_str());
    const Province* capital = gs.curr_nation->capital;
    if(capital != nullptr) {
        gs.map->camera->set_pos(capital->max_x, capital->max_y);
    }
}
