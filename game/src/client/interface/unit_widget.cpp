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
//      eng3d/ui/unit_widget.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/image.hpp"
#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/camera.hpp"
#include "eng3d/color.hpp"

#include "client/interface/unit_widget.hpp"
#include "client/map.hpp"
#include "client/game_state.hpp"
#include "nation.hpp"
#include "unit.hpp"
#include "world.hpp"

using namespace Interface;

UnitWidget::UnitWidget(Unit& _unit, Map& _map, UI::Widget* parent)
    : UI::Div(0, 0, 100, 30, parent),
    unit{ _unit },
    map{ _map }
{
    this->background_color = Eng3D::Color(1, 1, 1, 1);

    new UI::Image(1, 1, this->width - 1, this->height - 1, "gfx/drop_shadow.png", this);

    this->set_on_click([this](UI::Widget&) {
        auto& gs = (GameState&)Eng3D::State::get_instance();
        auto it = std::find(gs.input.selected_units.begin(), gs.input.selected_units.end(), &(const_cast<Unit&>(this->unit)));
        if(it == gs.input.selected_units.end()) {
            // Select if not on selected units list
            gs.input.selected_units.push_back(&(const_cast<Unit&>(this->unit)));
        } else {
            // Erase if already on selected units (deselect)
            gs.input.selected_units.erase(it);
        }
    });

    auto nation_flag = map.nation_flags[0];
    this->flag_img = new UI::Image(1, 1, 38, 28, nation_flag, this);

    new UI::Image(1, 1, 38, 28, "gfx/drop_shadow.png", this);

    this->size_label = new UI::Div(41, 1, 48, 28, this);
    this->size_label->text_align_x = UI::Align::END;

    this->size_label->on_each_tick = ([this](UI::Widget&) {
        auto unit_size = (int)this->unit.size;
        this->size_label->text(std::to_string(unit_size));
    });

    this->morale_bar = new UI::ProgressBar(91, 1, 8, 28, 0, 1, this);
    this->morale_bar->on_each_tick = ([this](UI::Widget&) {
        this->morale_bar->set_value(this->unit.morale);
    });
    this->morale_bar->direction = UI::Direction::BOTTOM_TO_TOP;

    this->set_unit(unit);
}

void UnitWidget::set_unit(Unit& _unit) {
    this->unit = _unit;

    const Eng3D::Camera& camera = *map.camera;
    auto unit_pos = this->unit.get_pos();
    auto screen_pos = camera.get_tile_screen_pos(glm::vec2(unit_pos.first, unit_pos.second));

    this->x = screen_pos.x - this->width / 2;
    this->y = screen_pos.y - this->height / 2;

    GameState& gs = (GameState&)Eng3D::State::get_instance();
    // Paint according to relations
    if(gs.curr_nation != nullptr && unit.owner_id != gs.curr_nation->get_id()) {
        const NationRelation& relation = gs.world->get_relation(gs.world->get_id(*gs.curr_nation), unit.owner_id);
        if(relation.has_alliance) {
            this->size_label->background_color = Eng3D::Color::rgba8(0x1e, 0x80, 0x0f, 0x80);
        } else if(relation.has_war) {
            this->size_label->background_color = Eng3D::Color::rgba8(0xde, 0x23, 0x23, 0x80);
        } else {
            this->size_label->background_color = Eng3D::Color::rgba8(0xff, 0xff, 0xff, 0x80);
        }
    } else if(gs.curr_nation != nullptr && unit.owner_id == gs.curr_nation->get_id()) {
        this->size_label->background_color = Eng3D::Color::rgba8(0x1e, 0x80, 0x0f, 0x80);
    }

    auto nation_flag = map.nation_flags[unit.owner_id];
    this->flag_img->current_texture = nation_flag;
    this->size_label->on_each_tick(*this->size_label);
    this->morale_bar->on_each_tick(*this->morale_bar);
}

UnitWidget::~UnitWidget() {

}
