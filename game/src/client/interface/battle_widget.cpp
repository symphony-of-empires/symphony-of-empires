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
#include "world.hpp"

using namespace Interface;

BattleWidget::BattleWidget(Map& _map, UI::Widget* _parent)
    : UI::Div(0, 0, 188, 30, _parent),
    map{ _map }
{
    this->background_color = Eng3D::Color(1.f, 1.f, 1.f, 1.f);

    /// @todo On click display information about the battle
    auto& gs = static_cast<GameState&>(Eng3D::State::get_instance());

    this->left_flag_img = new UI::Image(1, 1, 38, 28, gs.tex_man.get_white(), this);
    this->left_size_label = new UI::Div(41, 1, 48, 28, this);
    this->left_size_label->text_align_x = UI::Align::END;
    this->left_size_label->background_color = Eng3D::Color(0.8f, 0.f, 0.f, 1.f);
    this->left_size_label->set_on_each_tick([this](UI::Widget&) {
        if(this->province == nullptr) return;
        const auto& battle = this->province->battle;
        if(!battle.active) return;

        auto unit_size = 0.f;
        const auto attacker_unit_ids = battle.get_attacker_unit_ids();
        for(const auto unit_id : attacker_unit_ids) {
            const auto& unit = g_world.unit_manager.units[unit_id];
            unit_size += unit.size;
        }
        this->left_size_label->set_text(Eng3D::string_format("%.0f", unit_size));
    });

    this->right_flag_img = new UI::Image(139, 1, 38, 28, gs.tex_man.get_white(), this);
    this->right_size_label = new UI::Div(90, 1, 48, 28, this);
    this->right_size_label->text_align_x = UI::Align::END;
    this->right_size_label->background_color = Eng3D::Color(0.f, 0.f, 0.8f, 1.f);
    this->right_size_label->set_on_each_tick([this](UI::Widget&) {
        if(this->province == nullptr) return;
        const auto& battle = this->province->battle;
        if(!battle.active) return;

        auto unit_size = 0.f;
        const auto defender_unit_ids = battle.get_defender_unit_ids();
        for(const auto unit_id : defender_unit_ids) {
            const auto& unit = g_world.unit_manager.units[unit_id];
            unit_size += unit.size;
        }
        this->right_size_label->set_text(Eng3D::string_format("%.0f", unit_size));
    });
}

void BattleWidget::set_battle(Province& _province) {
    this->province = &_province;
    const auto& battle = this->province->battle;
    const auto& camera = *map.camera;
    const auto battle_pos = province->get_pos();
    const auto screen_pos = camera.get_tile_screen_pos(battle_pos);

    this->x = screen_pos.x - this->width / 2;
    this->y = screen_pos.y - this->height / 2;

    auto& gs = static_cast<GameState&>(Eng3D::State::get_instance());

    const auto attacker_unit_ids = battle.get_attacker_unit_ids();
    if(!attacker_unit_ids.empty()) {
        auto left_nation_flag = gs.get_nation_flag(gs.world->nations[gs.world->unit_manager.units[attacker_unit_ids[0]].owner_id]);
        this->left_flag_img->current_texture = left_nation_flag;
        this->left_size_label->on_each_tick(*this->left_size_label);
    }

    const auto defender_unit_ids = battle.get_defender_unit_ids();
    if(!defender_unit_ids.empty()) {
        auto right_nation_flag = gs.get_nation_flag(gs.world->nations[gs.world->unit_manager.units[defender_unit_ids[0]].owner_id]);
        this->right_flag_img->current_texture = right_nation_flag;
        this->right_size_label->on_each_tick(*this->right_size_label);
    }
}
