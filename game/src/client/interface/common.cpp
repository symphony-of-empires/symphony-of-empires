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
//      client/interface/common.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/texture.hpp"
#include "unified_render/ui/image.hpp"
#include "unified_render/ui/button.hpp"
#include "unified_render/ui/chart.hpp"
#include "unified_render/ui/label.hpp"
#include "unified_render/ui/checkbox.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/ui/progress_bar.hpp"
#include "unified_render/locale.hpp"

#include "client/interface/common.hpp"
#include "client/interface/good_view.hpp"
#include "unified_render/path.hpp"
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

UnitButton::UnitButton(GameState& _gs, int x, int y, Unit* _unit, UI::Widget* parent)
    : UI::Button(x, y, parent->width, 24, parent),
    gs{ _gs },
    unit{ _unit }
{
    text(std::to_string(unit->size) + " " + unit->type->name);
    on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<UnitButton&>(*w.parent);
        w.text(std::to_string(o.unit->size) + " " + UnifiedRender::Locale::translate(o.unit->type->name));
    });
}

UnitTypeButton::UnitTypeButton(GameState& _gs, int x, int y, UnitType* _unit_type, UI::Widget* parent)
    : UI::Group(x, y, parent->width, 24, parent),
    gs{ _gs },
    unit_type{ _unit_type }
{
    this->is_scroll = false;

    this->icon_img = new UI::Image(0, 0, 32, 24, nullptr, this);
    this->icon_img->current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/unittype/" + unit_type->ref_name + ".png"));

    this->name_btn = new UI::Button(0, 0, this->width - 32, 24, this);
    this->name_btn->right_side_of(*this->icon_img);
    this->name_btn->text(unit_type->name);
}

ProvinceButton::ProvinceButton(GameState& _gs, int x, int y, Province* _province, UI::Widget* parent)
    : UI::Button(x, y, parent->width, 24, parent),
    gs{ _gs },
    province{ _province }
{
    text(province->name);
    on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<ProvinceButton&>(w);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }
        w.text(o.province->name);
    });
}

NationButton::NationButton(GameState& _gs, int x, int y, Nation* _nation, UI::Widget* parent)
    : UI::Group(x, y, parent->width, 24, parent),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;

    this->flag_icon = new UI::Image(0, 0, 32, 24, nullptr, this);
    this->flag_icon->current_texture = &gs.get_nation_flag(*nation);
    this->flag_icon->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<NationButton&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }
        w.current_texture = &o.gs.get_nation_flag(*o.nation);
    });

    this->name_btn = new UI::Button(0, 0, this->width - 32, 24, this);
    this->name_btn->right_side_of(*this->flag_icon);
    this->name_btn->text(nation->get_client_hint().alt_name);
    this->name_btn->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<NationButton&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }
        w.text(UnifiedRender::Locale::translate(o.nation->get_client_hint().alt_name));
    });
}

BuildingInfo::BuildingInfo(GameState& _gs, int x, int y, Province* _province, unsigned int _idx, UI::Widget* parent)
    : UI::Group(x, y, parent->width, 24 * 8, parent),
    gs{ _gs },
    province{ _province },
    idx{ _idx }
{
    is_scroll = false;

    const Building& building = province->get_buildings().at(idx);
    auto* name_btn = new UI::Button(0, 0, 128, 24, this);
    name_btn->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<BuildingInfo&>(*w.parent);

        if(o.idx >= o.province->get_buildings().size()) {
            return;
        }
        const Building& building = o.province->get_buildings().at(o.idx);
        //w.text(building.type->name);
    });

    unsigned int dx;

    auto* input_lab = new UI::Label(0, 0, "Inputs:", this);
    input_lab->below_of(*name_btn);
    dx = input_lab->width;
    for(const auto& good : gs.world->building_types[idx]->inputs) {
        auto* icon_ibtn = new UI::Image(dx, 0, 24, 24, &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/good/" + good->ref_name + ".png")), this);
        icon_ibtn->below_of(*name_btn);
        icon_ibtn->on_click = ([good](UI::Widget& w) {
            auto& o = static_cast<BuildingInfo&>(*w.parent);
            new GoodView(o.gs, good);
        });
        dx += icon_ibtn->width;
    }

    auto* output_lab = new UI::Label(0, 0, "Outputs:", this);
    output_lab->below_of(*input_lab);
    dx = output_lab->width;
    for(const auto& good : gs.world->building_types[idx]->outputs) {
        auto* icon_ibtn = new UI::Image(dx, 0, 24, 24, &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/good/" + good->ref_name + ".png")), this);
        icon_ibtn->below_of(*input_lab);
        icon_ibtn->on_click = ([good](UI::Widget& w) {
            auto& o = static_cast<BuildingInfo&>(*w.parent);
            new GoodView(o.gs, good);
        });
        dx += icon_ibtn->width;
    }
}

BuildingTypeButton::BuildingTypeButton(GameState& _gs, int x, int y, BuildingType* _building_type, UI::Widget* parent)
    : UI::Button(x, y, parent->width, 24, parent),
    gs{ _gs },
    building_type{_building_type}
{
    text(building_type->name);
}

TechnologyInfo::TechnologyInfo(GameState& _gs, int x, int y, Technology* _technology, UI::Widget* parent)
    : UI::Group(x, y, parent->width, 48, parent),
    gs{ _gs },
    technology{ _technology }
{
    is_scroll = false;

    auto* chk = new UI::Checkbox(0, 0, 128, 24, this);
    chk->text(technology->name);
    chk->tooltip = new UI::Tooltip(chk, 512, 24);
    chk->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<TechnologyInfo&>(*w.parent);
        if(o.technology == o.gs.curr_nation->focus_tech || !o.gs.curr_nation->research[o.gs.world->get_id(o.technology)]) {
            ((UI::Checkbox&)w).set_value(true);
        } else {
            ((UI::Checkbox&)w).set_value(false);
        }

        if(o.gs.curr_nation->can_research(o.technology)) {
            w.tooltip->text(UnifiedRender::Locale::translate("We can research this"));
        } else {
            std::string text = "";
            text = UnifiedRender::Locale::translate("We can't research this because we don't have ");
            for(const auto& req_tech : o.technology->req_technologies) {
                if(o.gs.curr_nation->research[o.gs.world->get_id(req_tech)] > 0.f) {
                    text += UnifiedRender::Locale::translate(req_tech->name) + ", ";
                }
            }
            w.tooltip->text(text);
        }
    });
    chk->set_on_click([](UI::Widget& w) {
        auto& o = static_cast<TechnologyInfo&>(*w.parent);
        if(o.gs.curr_nation->can_research(o.technology)) {
            o.gs.client->send(Action::FocusTech::form_packet(o.technology));
        }
    });
    chk->on_each_tick(*chk);

    auto* pgbar = new UI::ProgressBar(0, 24, 128, 24, 0.f, technology->cost, this);
    pgbar->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<TechnologyInfo&>(*w.parent);
        ((UI::ProgressBar&)w).value = std::fabs(o.gs.curr_nation->research[o.gs.world->get_id(o.technology)] - o.technology->cost);
    });
}


PopInfo::PopInfo(GameState& _gs, int x, int y, Province* _province, int _index, UI::Widget* parent)
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

    this->religion_ibtn = new UI::Image(0, 0, 24, 24, nullptr, this);
    this->religion_ibtn->right_side_of(*this->budget_btn);
    this->religion_ibtn->set_tooltip(new UI::Tooltip(this->religion_ibtn, 512, 24));

    this->culture_btn = new UI::Button(0, 0, 128, 24, this);
    this->culture_btn->right_side_of(*this->religion_ibtn);
    
    this->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<PopInfo&>(w);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        if(o.index >= o.province->pops.size()) {
            return;
        }

        const Pop& pop = o.province->pops[o.index];
        o.size_btn->text(std::to_string(pop.size));
        o.budget_btn->text(std::to_string(pop.budget / pop.size));
        o.budget_btn->tooltip->text(UnifiedRender::Locale::translate("A total budget of") + " " + std::to_string(pop.budget));
        o.religion_ibtn->tooltip->text(UnifiedRender::Locale::translate(pop.religion->name));
        o.culture_btn->text(UnifiedRender::Locale::translate(pop.culture->name));
    });
    this->on_each_tick(*this);
}

ProductInfo::ProductInfo(GameState& _gs, int x, int y, Province* _province, Good* _good, UI::Widget* parent)
    : UI::Group(x, y, parent->width, 24, parent),
    gs{ _gs },
    province{ _province },
    good{ _good }
{
    this->is_scroll = false;

    this->good_ibtn = new UI::Image(0, 0, 24, 24, nullptr, this);
    this->good_ibtn->current_texture = &gs.tex_man->load(Path::get("gfx/good/" + good->ref_name + ".png"));
    this->good_ibtn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        new GoodView(o.gs, o.good);
    });
    this->good_ibtn->set_tooltip(new UI::Tooltip(this->good_ibtn, 512, 24));
    this->good_ibtn->tooltip->text(UnifiedRender::Locale::translate(good->name));

    this->price_rate_btn = new UI::Button(0, 0, 96, 24, this);
    this->price_rate_btn->right_side_of(*this->good_ibtn);

    this->price_chart = new UI::Chart(0, 0, 96, 24, this);
    this->price_chart->right_side_of(*this->price_rate_btn);
    this->price_chart->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        //new ProductView(o.gs, o.product);
    });
    this->price_chart->set_tooltip(new UI::Tooltip(this->price_chart, 512, 24));

    this->supply_chart = new UI::Chart(0, 0, 96, 24, this);
    this->supply_chart->right_side_of(*this->price_chart);
    this->supply_chart->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        //new ProductView(o.gs, o.product);
    });
    this->supply_chart->set_tooltip(new UI::Tooltip(this->supply_chart, 512, 24));

    this->demand_chart = new UI::Chart(0, 0, 96, 24, this);
    this->demand_chart->right_side_of(*this->supply_chart);
    this->demand_chart->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        //new ProductView(o.gs, o.province->products[o.gs.world->get_id(o.good)]);
    });
    this->demand_chart->set_tooltip(new UI::Tooltip(this->demand_chart, 512, 24));

    this->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<ProductInfo&>(w);

        // Only update every ticks_per_month ticks
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        Product& product = o.province->products[o.gs.world->get_id(o.good)];

        o.price_chart->data.push_back(product.price);
        if(o.price_chart->data.size() >= 30) {
            o.price_chart->data.pop_back();
        }
        o.price_chart->tooltip->text(std::to_string(product.price));

        o.supply_chart->data.push_back(product.supply);
        if(o.supply_chart->data.size() >= 30) {
            o.supply_chart->data.pop_back();
        }
        o.supply_chart->tooltip->text(std::to_string(product.supply));

        o.demand_chart->data.push_back(product.demand);
        if(o.demand_chart->data.size() >= 30) {
            o.demand_chart->data.pop_back();
        }
        o.demand_chart->tooltip->text(std::to_string(product.demand));
        o.price_rate_btn->text(std::to_string(product.price_vel));
        if(product.price_vel >= 0.f) {
            o.price_rate_btn->text_color = UnifiedRender::Color(0, 255, 0);
        } else {
            o.price_rate_btn->text_color = UnifiedRender::Color(255, 0, 0);
        }
    });
    this->on_each_tick(*this);
}