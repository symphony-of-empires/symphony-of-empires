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
//      client/interface/common.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/texture.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/chart.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/checkbox.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/string.hpp"
#include "eng3d/string.hpp"

#include "client/interface/common.hpp"
#include "client/interface/good_view.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "unit.hpp"
#include "province.hpp"
#include "product.hpp"
#include "building.hpp"
#include "client/client_network.hpp"
#include "action.hpp"

using namespace Interface;

UnitButton::UnitButton(GameState& _gs, int x, int y, Unit& _unit, UI::Widget* _parent)
    : UI::Button(x, y, _parent->width, 24, _parent),
    gs{ _gs },
    unit{ _unit }
{
    this->set_on_each_tick([this](UI::Widget& w) {
        auto& type = this->gs.world->unit_types[this->unit.type_id];
        w.text(string_format("%zu %s", this->unit.size, type.name.get_string()));
    });
    this->on_each_tick(*this);
}

UnitTypeButton::UnitTypeButton(GameState& _gs, int x, int y, UnitType& _unit_type, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width, 24, _parent),
    gs{ _gs },
    unit_type{ _unit_type }
{
    this->is_scroll = false;

    this->icon_img = new UI::Image(0, 0, 32, 24, this);
    this->icon_img->current_texture = this->gs.tex_man.load(gs.package_man.get_unique("gfx/unittype/" + this->unit_type.ref_name + ".png"));

    this->name_btn = new UI::Button(0, 0, this->width - 32, 24, this);
    this->name_btn->right_side_of(*this->icon_img);
    this->name_btn->text(this->unit_type.name.get_string());
}

NationButton::NationButton(GameState& _gs, int x, int y, Nation& _nation, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width, 24, _parent),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;

    this->flag_icon = new UI::Image(0, 0, 32, 24, this);
    this->flag_icon->current_texture = this->gs.get_nation_flag(this->nation);
    this->flag_icon->set_on_each_tick([this](UI::Widget& w) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;
        w.current_texture = this->gs.get_nation_flag(this->nation);
    });

    this->name_btn = new UI::Button(0, 0, this->width - 32, 24, this);
    this->name_btn->right_side_of(*this->flag_icon);
    this->name_btn->text(nation.get_client_hint().alt_name.get_string());
    this->name_btn->set_on_each_tick([](UI::Widget& w) {
        auto& o = static_cast<NationButton&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) return;
        w.text(o.nation.get_client_hint().alt_name.get_string());
    });
}

ProductInfo::ProductInfo(GameState& _gs, int x, int y, Province& _province, Good& _good, UI::Widget* parent)
    : UI::Group(x, y, parent->width, 24, parent),
    gs{ _gs },
    province{ _province },
    good{ _good }
{
    this->is_scroll = false;

    this->good_ibtn = new UI::Image(0, 0, 24, 24, this->gs.tex_man.load(this->gs.package_man.get_unique("gfx/good/" + good.ref_name + ".png")), this);
    this->good_ibtn->set_on_click([](UI::Widget& w) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        new GoodView(o.gs, o.good);
    });
    this->good_ibtn->set_tooltip(new UI::Tooltip(this->good_ibtn, 512, 24));
    this->good_ibtn->tooltip->text(good.name.get_string());

    this->price_rate_btn = new UI::Button(0, 0, 96, 24, this);
    this->price_rate_btn->right_side_of(*this->good_ibtn);

    this->price_chart = new UI::Chart(0, 0, 96, 24, this);
    this->price_chart->right_side_of(*this->price_rate_btn);
    this->price_chart->text("Price");
    this->price_chart->set_on_click([](UI::Widget&) {

    });
    this->price_chart->set_tooltip(new UI::Tooltip(this->price_chart, 512, 24));

    this->supply_chart = new UI::Chart(0, 0, 96, 24, this);
    this->supply_chart->right_side_of(*this->price_chart);
    this->supply_chart->text("Supply");
    this->supply_chart->set_on_click([](UI::Widget&) {

    });
    this->supply_chart->set_tooltip(new UI::Tooltip(this->supply_chart, 512, 24));

    this->demand_chart = new UI::Chart(0, 0, 96, 24, this);
    this->demand_chart->right_side_of(*this->supply_chart);
    this->demand_chart->text("Demand");
    this->demand_chart->set_on_click([](UI::Widget&) {

    });
    this->demand_chart->set_tooltip(new UI::Tooltip(this->demand_chart, 512, 24));

    this->set_on_each_tick([this](UI::Widget&) {
        const auto& product = this->province.products[this->gs.world->get_id(this->good)];
        this->price_chart->data.clear();
        for(const auto& data : this->price_history)
            this->price_chart->data.push_back(data);
        this->price_history.push_back(product.price);
        if(!this->price_history.empty())
            this->price_chart->set_tooltip(string_format("%.4f", this->price_history.back()));

        this->supply_chart->data.clear();
        for(const auto& data : this->supply_history)
            this->supply_chart->data.push_back(data);
        this->supply_history.push_back(product.supply);
        if(!this->supply_history.empty())
            this->supply_chart->set_tooltip(string_format("%.4f", this->supply_history.back()));

        this->demand_chart->data.clear();
        for(const auto& data : this->demand_history)
            this->demand_chart->data.push_back(data);
        this->demand_history.push_back(product.demand);
        if(!this->demand_history.empty())
            this->demand_chart->set_tooltip(string_format("%.4f", this->demand_history.back()));

        this->price_rate_btn->text(std::to_string(product.price_delta));
        if(product.price_delta >= 0.f)
            this->price_rate_btn->text_color = Eng3D::Color(0, 255, 0);
        else
            this->price_rate_btn->text_color = Eng3D::Color(255, 0, 0);
    });
    this->on_each_tick(*this);
}