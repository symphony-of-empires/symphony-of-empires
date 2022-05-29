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
//      eng3d/ui/battle_widget.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/image.hpp"
#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/camera.hpp"
#include "eng3d/color.hpp"

#include "client/interface/battle_widget.hpp"
#include "client/map.hpp"
#include "client/game_state.hpp"
#include "nation.hpp"
#include "diplomacy.hpp"
#include "unit.hpp"
#include "nation.hpp"

using namespace Interface;

BattleWidget::BattleWidget(const War* _war, size_t _idx, Map* _map, UI::Widget* parent)
    : UI::Div(0, 0, 188, 30, parent), war{ _war }, idx{ _idx }, map{ _map }
{
    this->background_color = Eng3D::Color(1.f, 1.f, 1.f, 1.f);
    /// @todo On click display information about the battle

    auto nation_flag = map->nation_flags[0];

    this->left_flag_img = new UI::Image(1, 1, 38, 28, nation_flag, this);
    this->left_size_label = new UI::Div(41, 1, 48, 28, this);
    this->left_size_label->text_align_x = UI::Align::END;
    this->left_size_label->background_color = Eng3D::Color(0.5f, 0.f, 0.f, 1.f);
    this->left_size_label->on_each_tick = ([this](UI::Widget&) {
        if(this->idx >= this->war->battles.size()) {
            return;
        }

        const Battle& battle = this->war->battles[this->idx];
        auto unit_size = 0;
        for(const auto& unit : battle.attackers) {
            unit_size += (int)unit->size;
        }
        this->left_size_label->text(std::to_string(unit_size));
    });

    this->right_flag_img = new UI::Image(139, 1, 38, 28, nation_flag, this);
    this->right_size_label = new UI::Div(90, 1, 48, 28, this);
    this->right_size_label->text_align_x = UI::Align::END;
    this->right_size_label->background_color = Eng3D::Color(0.f, 0.f, 0.5f, 1.f);
    this->right_size_label->on_each_tick = ([this](UI::Widget&) {
        if(this->idx >= this->war->battles.size()) {
            return;
        }

        const Battle& battle = this->war->battles[this->idx];
        auto unit_size = 0;
        for(const auto& unit : battle.defenders) {
            unit_size += (int)unit->size;
        }
        this->right_size_label->text(std::to_string(unit_size));
    });

    this->set_battle(this->war, this->idx);
}

void BattleWidget::set_battle(const War* _war, size_t _idx) {
    this->war = _war;
    this->idx = _idx;
    const Battle& battle = this->war->battles[this->idx];

    const Eng3D::Camera& camera = *map->camera;
    const auto battle_pos = battle.province->get_pos();
    const auto screen_pos = camera.get_tile_screen_pos(glm::vec2(battle_pos.first, battle_pos.second));

    this->x = screen_pos.x - this->width / 2;
    this->y = screen_pos.y - this->height / 2;

    if(!battle.attackers.empty()) {
        auto left_nation_flag = map->nation_flags[battle.attackers[0]->owner->cached_id];
        this->left_flag_img->current_texture = left_nation_flag;
        this->left_size_label->on_each_tick(*this->left_size_label);
    }

    if(!battle.defenders.empty()) {
        auto right_nation_flag = map->nation_flags[battle.defenders[0]->owner->cached_id];
        this->right_flag_img->current_texture = right_nation_flag;
        this->right_size_label->on_each_tick(*this->right_size_label);
    }
}

BattleWidget::~BattleWidget() {

}
