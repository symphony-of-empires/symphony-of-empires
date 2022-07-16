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

UnitWidget::UnitWidget(Unit& _unit, Map& _map, GameState& _gs, UI::Widget* parent)
    : UI::Div(0, 0, 100, 30, parent),
    unit_id{ _unit.cached_id },
    map{ _map },
    gs{ _gs }
{
    this->background_color = Eng3D::Color(1, 1, 1, 1);

    auto& s = Eng3D::State::get_instance();
    select_border_texture = s.tex_man.load(s.package_man.get_unique("gfx/border_sharp2.png"));
    this->border.size = glm::ivec2{this->width, this->height};
    this->border.texture_size = glm::ivec2{7, 7};

    new UI::Image(1, 1, this->width - 1, this->height - 1, "gfx/drop_shadow.png", this);

    this->set_on_click([this](UI::Widget&) {
        if (gs.input.is_selected_unit(this->unit_id)) {
            gs.input.unselect_unit(this->unit_id);
        } else {
            gs.input.select_unit(this->unit_id);
        }
    });

    auto nation_flag = map.nation_flags[0];
    this->flag_img = new UI::Image(1, 1, 38, 28, nation_flag, this);

    new UI::Image(1, 1, 38, 28, "gfx/drop_shadow.png", this);

    this->size_label = new UI::Div(41, 1, 48, 28, this);
    this->size_label->text_align_x = UI::Align::END;


    this->morale_bar = new UI::ProgressBar(91, 1, 8, 28, 0, 1, this);
    this->morale_bar->direction = UI::Direction::BOTTOM_TO_TOP;

    this->set_unit(_unit);
}

// This is expected to be called every framed
void UnitWidget::set_unit(Unit& _unit) {
    this->unit_id = _unit.get_id();

    const Eng3D::Camera& camera = *map.camera;
    auto unit_pos = _unit.get_pos();
    auto screen_pos = camera.get_tile_screen_pos(unit_pos);

    this->x = screen_pos.x - this->width / 2;
    this->y = screen_pos.y - this->height / 2;

    // If the unit is selected set give it a border
    if (gs.input.is_selected_unit(this->unit_id)) {
        this->border.texture = this->select_border_texture;
    } else {
        this->border.texture = nullptr;
    }

    // Paint according to relations
    if(gs.curr_nation != nullptr && _unit.owner_id != gs.curr_nation->get_id()) {
        const NationRelation& relation = gs.world->get_relation(gs.world->get_id(*gs.curr_nation), _unit.owner_id);
        if(relation.has_alliance) {
            this->size_label->background_color = Eng3D::Color::rgba8(0x1e, 0x80, 0x0f, 0x80);
        } else if(relation.has_war) {
            this->size_label->background_color = Eng3D::Color::rgba8(0xde, 0x23, 0x23, 0x80);
        } else {
            this->size_label->background_color = Eng3D::Color::rgba8(0xff, 0xff, 0xff, 0x80);
        }
    } else if(gs.curr_nation != nullptr && _unit.owner_id == gs.curr_nation->get_id()) {
        this->size_label->background_color = Eng3D::Color::rgba8(0x1e, 0x80, 0x0f, 0x80);
    }

    auto nation_flag = map.nation_flags[_unit.owner_id];
    this->flag_img->current_texture = nation_flag;
    auto unit_size = (int)_unit.size;
    this->set_size(unit_size);
    this->morale_bar->set_value(_unit.morale);
}

void UnitWidget::set_size(size_t size) {
    this->size_label->text(std::to_string(size));
}

UnitWidget::~UnitWidget() {

}
