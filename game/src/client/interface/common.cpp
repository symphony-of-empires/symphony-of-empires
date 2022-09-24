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
#include "eng3d/locale.hpp"
#include "eng3d/path.hpp"
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
#include "io_impl.hpp"

using namespace Interface;

UnitButton::UnitButton(GameState& _gs, int x, int y, Unit& _unit, UI::Widget* _parent)
    : UI::Button(x, y, _parent->width, 24, _parent),
    gs{ _gs },
    unit{ _unit }
{
    this->set_on_each_tick([this](UI::Widget& w) {
        w.text(Eng3D::string_format("%zu %s", this->unit.size, _(this->unit.type->name.get_string())));
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

ProvinceButton::ProvinceButton(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Button(x, y, _parent->width, 24, _parent),
    gs{ _gs },
    province{ _province }
{
    this->text(this->province.name.get_string());
    this->set_on_each_tick([this](UI::Widget& w) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;
        w.text(this->province.name.get_string());
    });
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
        w.text(_(o.nation.get_client_hint().alt_name.get_string()));
    });
}

BuildingInfo::BuildingInfo(GameState& _gs, int x, int y, Province& _province, unsigned int _idx, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width, 64, _parent),
    gs{ _gs },
    province{ _province },
    idx{ _idx }
{
    this->is_scroll = false;

    const auto& building_type = gs.world->building_types[idx];
    auto* name_btn = new UI::Label(0, 0, building_type.name.get_string(), this);

    auto* production_flex = new UI::Div(0, 24, this->width, 24);
    production_flex->flex = UI::Flex::COLUMN;
    auto* makes_lab = new UI::Label(0, 0, _("Produces:"), production_flex);
    makes_lab->below_of(*name_btn);
    if(!building_type.inputs.empty()) {
        for(const auto& good : building_type.inputs) {
            auto* icon_ibtn = new UI::Image(0, 0, 24, 24, this->gs.tex_man.load(gs.package_man.get_unique("gfx/good/" + good->ref_name + ".png")), production_flex);
            icon_ibtn->set_tooltip(good->name.get_string());
            icon_ibtn->set_on_click([this, good](UI::Widget&) {
                new Interface::GoodView(this->gs, *good);
            });
        }
    }

    if(building_type.output != nullptr) {
        if(!building_type.inputs.empty()) {
            auto* arrow_lab = new UI::Label(0, 0, "?", production_flex);
            arrow_lab->text(_("into"));
        }
        auto* good = building_type.output;
        auto* icon_ibtn = new UI::Image(0, 0, 24, 24, this->gs.tex_man.load(gs.package_man.get_unique("gfx/good/" + good->ref_name + ".png")), production_flex);
        icon_ibtn->set_tooltip(good->name.get_string());
        icon_ibtn->set_on_click([this, good](UI::Widget&) {
            new GoodView(this->gs, *good);
        });
    }

    auto* stats_lab = new UI::Label(0, 48, "?", this);
    stats_lab->below_of(*name_btn);
    stats_lab->set_on_each_tick([this](UI::Widget& w) {
        const auto& building = this->province.buildings[this->idx];
        w.text(Eng3D::string_format("Level %.0f, %.2f$", building.level, building.budget));
    });
    stats_lab->on_each_tick(*stats_lab);
}

BuildingTypeButton::BuildingTypeButton(GameState& _gs, int x, int y, const BuildingType& _building_type, UI::Widget* _parent)
    : UI::Button(x, y, _parent->width - x, 24, _parent),
    gs{ _gs },
    building_type{ _building_type }
{
    this->text(this->building_type.name.get_string());
    if(this->gs.editor)
        this->text(this->building_type.name.get_string() + "(" + this->building_type.ref_name.get_string() + ")");
}

TechnologyInfo::TechnologyInfo(GameState& _gs, int x, int y, Technology& _technology, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width, 48, _parent),
    gs{ _gs },
    technology{ _technology }
{
    this->is_scroll = false;

    auto* chk = new UI::Checkbox(0, 0, 128, 24, this);
    chk->text(technology.name.get_string());
    chk->set_on_each_tick([this](UI::Widget& w) {
        if(&this->technology == this->gs.curr_nation->focus_tech || !this->gs.curr_nation->research[this->gs.world->get_id(this->technology)]) {
            ((UI::Checkbox&)w).set_value(true);
        } else {
            ((UI::Checkbox&)w).set_value(false);
        }

        if(this->gs.curr_nation->can_research(this->technology)) {
            w.set_tooltip(_("We can research this"));
        } else {
            std::string text = _("We can't research this because we don't have ");
            for(const auto& req_tech_id : this->technology.req_technologies) {
                if(this->gs.curr_nation->research[req_tech_id] > 0.f)
                    text += _(this->gs.world->technologies[req_tech_id].name.get_string()) + ", ";
            }
            w.set_tooltip(text);
        }
    });
    chk->set_on_click([this](UI::Widget&) {
        if(this->gs.curr_nation->can_research(this->technology))
            this->gs.client->send(Action::FocusTech::form_packet(this->technology));
    });
    chk->on_each_tick(*chk);

    auto* pgbar = new UI::ProgressBar(0, 24, 128, 24, 0.f, technology.cost, this);
    pgbar->set_on_each_tick([this](UI::Widget& w) {
        ((UI::ProgressBar&)w).set_value(std::fabs(this->gs.curr_nation->research[this->gs.world->get_id(this->technology)] - this->technology.cost));
    });
}

PopInfo::PopInfo(GameState& _gs, int x, int y, Province& _province, std::size_t _index, UI::Widget* parent)
    : UI::Group(x, y, parent->width, 24, parent),
    gs{ _gs },
    province{ _province },
    index{ _index }
{
    this->is_scroll = false;

    this->size_btn = new UI::Button(0, 0, 96, 24, this);

    this->budget_btn = new UI::Button(0, 0, 128, 24, this);
    this->budget_btn->right_side_of(*this->size_btn);
    this->budget_btn->set_tooltip(new UI::Tooltip(this->budget_btn, 512, 24));
    
    this->set_on_each_tick([this](UI::Widget&) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;
        if(this->index >= this->province.pops.size()) return;

        const auto& pop = this->province.pops[this->index];
        this->size_btn->text(std::to_string(pop.size));
        this->budget_btn->text(std::to_string(pop.budget / pop.size));
        this->budget_btn->tooltip->text(Eng3D::Locale::translate("A total budget of") + " " + std::to_string(pop.budget));
    });
    this->on_each_tick(*this);
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
    this->good_ibtn->tooltip->text(_(good.name.get_string()));

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
            this->price_chart->set_tooltip(Eng3D::string_format("%.4f", this->price_history.back()));

        this->supply_chart->data.clear();
        for(const auto& data : this->supply_history)
            this->supply_chart->data.push_back(data);
        this->supply_history.push_back(product.supply);
        if(!this->supply_history.empty())
            this->supply_chart->set_tooltip(Eng3D::string_format("%.4f", this->supply_history.back()));

        this->demand_chart->data.clear();
        for(const auto& data : this->demand_history)
            this->demand_chart->data.push_back(data);
        this->demand_history.push_back(product.demand);
        if(!this->demand_history.empty())
            this->demand_chart->set_tooltip(Eng3D::string_format("%.4f", this->demand_history.back()));

        this->price_rate_btn->text(std::to_string(product.price_vel));
        if(product.price_vel >= 0.f)
            this->price_rate_btn->text_color = Eng3D::Color(0, 255, 0);
        else
            this->price_rate_btn->text_color = Eng3D::Color(255, 0, 0);
    });
    this->on_each_tick(*this);
}