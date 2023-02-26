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
//      eng3d/ui/unit_widget.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/image.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/string.hpp"
#include "eng3d/camera.hpp"
#include "eng3d/color.hpp"

#include "client/interface/unit_widget.hpp"
#include "client/map.hpp"
#include "client/game_state.hpp"
#include "world.hpp"

using namespace Interface;

UnitWidget::UnitWidget(Map& _map, GameState& _gs, UI::Widget* _parent)
    : UI::Div(0, 0, 100, 30, _parent),
    map{ _map },
    gs{ _gs }
{
    this->unit_id = Unit::invalid();
    this->background_color = Eng3D::Color(1, 1, 1, 1);

    auto& s = Eng3D::State::get_instance();
    select_border_texture = s.tex_man.load(s.package_man.get_unique("gfx/border_sharp2.png"));
    this->border.size = glm::ivec2{this->width, this->height};
    this->border.texture_size = glm::ivec2{7, 7};

#ifndef E3D_HANDHELD
    this->make_widget<UI::Image>(1, 1, this->width - 1, this->height - 1, "gfx/drop_shadow.png");
#endif
    this->set_on_click([this](UI::Widget&) {
        if(Unit::is_invalid(this->unit_id)) return;
        if(gs.client_state.is_selected_unit(this->unit_id)) {
            gs.client_state.unselect_unit(this->unit_id);
        } else {
            gs.client_state.select_unit(this->unit_id);
        }
        new Interface::UnitView(this->gs, this->gs.world->unit_manager.units[this->unit_id]);
    });

    auto nation_flag = map.nation_flags[0];
    this->flag_img = new UI::Image(1, 1, 38, 28, nation_flag, this);

#ifndef E3D_HANDHELD
    this->make_widget<UI::Image>(1, 1, 38, 28, "gfx/drop_shadow.png");
#endif

    this->size_label = new UI::Div(41, 1, 48, 28, this);
    this->size_label->text_align_x = UI::Align::END;

    this->experience_bar = new UI::ProgressBar(91, 1, 8, 28, 0, 1, this);
    this->experience_bar->direction = UI::Direction::BOTTOM_TO_TOP;
}

// This is expected to be called every framed
void UnitWidget::set_unit(Unit& _unit) {
    this->unit_id = _unit.get_id();

    const auto& camera = *map.camera;
    auto unit_pos = _unit.get_pos();
    auto screen_pos = camera.get_tile_screen_pos(unit_pos);

    this->x = screen_pos.x - this->width / 2;
    this->y = screen_pos.y - this->height / 2;

    // If the unit is not selected set give it a border
    this->border.texture = nullptr;
    if(!gs.client_state.is_selected_unit(this->unit_id))
        this->border.texture = this->select_border_texture;

    // Paint according to relations
    if(gs.curr_nation != nullptr && _unit.owner_id != gs.curr_nation->get_id()) {
        const auto& relation = gs.world->get_relation(gs.world->get_id(*gs.curr_nation), _unit.owner_id);
        if(relation.is_allied()) {
            this->size_label->background_color = Eng3D::Color::rgba8(0x1e, 0x80, 0x0f, 0x80);
        } else if(relation.has_war) {
            this->size_label->background_color = Eng3D::Color::rgba8(0xde, 0x23, 0x23, 0x80);
        } else {
            this->size_label->background_color = Eng3D::Color::rgba8(0xff, 0xff, 0xff, 0x80);
        }
    } else if(gs.curr_nation != nullptr && _unit.owner_id == gs.curr_nation->get_id()) {
        this->size_label->background_color = Eng3D::Color::rgba8(0x1e, 0x80, 0x0f, 0x80);
    }
    
    this->flag_img->current_texture = this->gs.get_nation_flag(this->gs.world->nations[_unit.owner_id]);
    this->set_size(static_cast<size_t>(_unit.size));
    this->experience_bar->set_value(_unit.experience);
}

void UnitWidget::set_size(size_t size) {
    this->size_label->set_text(Eng3D::string_format("%zu", size));
}

UnitView::UnitView(GameState& _gs, Unit& _unit)
    : UI::Window(0, -200, 400, 200),
    gs{ _gs },
    unit_id{ _unit.get_id() }
{
    if(this->gs.lower_left_panel != nullptr)
        this->gs.lower_left_panel->kill();
    this->gs.lower_left_panel = this;
    this->set_close_btn_function([this](Widget&) {
        this->kill();
        this->gs.lower_left_panel = nullptr;
    });

    this->origin = UI::Origin::LOWER_LEFT_SCREEN;
    this->is_scroll = false;

    auto& unit = this->gs.world->unit_manager.units[this->unit_id];
    auto& unit_type = this->gs.world->unit_types[unit.type_id];
    this->set_text(Eng3D::translate_format("Unit %s from %s", unit_type.name.data(), this->gs.world->nations[unit.owner_id].name.data()));

    auto& flex_column = this->make_widget<UI::Div>(0, 0, this->width, this->height);
    flex_column.flex = UI::Flex::COLUMN;

    auto& target_lab = flex_column.make_widget<UI::Label>(0, 0, " ");
    target_lab.set_on_each_tick([this](UI::Widget& w) {
        auto& current_unit = this->gs.world->unit_manager.units[this->unit_id];
        auto target_id = current_unit.get_target_province_id();
        if(current_unit.has_target_province()) {
            const auto& target_province = this->gs.world->provinces[target_id];
            w.set_text(Eng3D::translate_format("Moving to %s", target_province.name.data()));
        } else {
            w.set_text(Eng3D::translate_format("No orders"));
        }
    });
    target_lab.on_each_tick(target_lab);

    auto& size_lab = flex_column.make_widget<UI::Label>(0, 0, " ");
    size_lab.set_on_each_tick([this](UI::Widget& w) {
        auto& current_unit = this->gs.world->unit_manager.units[this->unit_id];
        w.set_text(Eng3D::translate_format("Size: %.2f", current_unit.size));
    });
    size_lab.on_each_tick(size_lab);

    auto& experience_lab = flex_column.make_widget<UI::Label>(0, 0, " ");
    experience_lab.set_on_each_tick([this](UI::Widget& w) {
        auto& current_unit = this->gs.world->unit_manager.units[this->unit_id];
        w.set_text(Eng3D::translate_format("Experience: %.2f", current_unit.experience));
    });
    experience_lab.on_each_tick(experience_lab);

    auto& attdef_lab = flex_column.make_widget<UI::Label>(0, 0, " ");
    attdef_lab.set_on_each_tick([this](UI::Widget& w) {
        auto& current_unit = this->gs.world->unit_manager.units[this->unit_id];
        auto& current_type = this->gs.world->unit_types[current_unit.type_id];
        w.set_text(Eng3D::translate_format("Attack/Defense: %.2f/%.2f", current_type.attack, current_type.defense));
    });
    attdef_lab.on_each_tick(attdef_lab);

    auto& debug_lab = flex_column.make_widget<UI::Label>(0, 0, " ");
    debug_lab.set_on_each_tick([this](UI::Widget& w) {
        auto& current_unit = this->gs.world->unit_manager.units[this->unit_id];
        auto& current_type = this->gs.world->unit_types[current_unit.type_id];
        w.set_text(Eng3D::translate_format("On battle?: %s", current_unit.on_battle ? "Yes" : "No"));
    });
    debug_lab.on_each_tick(debug_lab);
}
