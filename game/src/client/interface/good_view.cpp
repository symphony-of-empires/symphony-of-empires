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
//      client/interface/good_view.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/good_view.hpp"
#include "io_impl.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"

#include "client/ui/piechart.hpp"
#include "client/ui/image.hpp"
#include "client/ui/label.hpp"
#include "client/ui/close_button.hpp"
#include "client/ui/chart.hpp"
#include "client/ui/group.hpp"
#include "client/ui/button.hpp"

using namespace Interface;

ProductView::ProductView(GameState& _gs, Product* _product)
    : UI::Window(0, 0, 512, 320),
    gs{ _gs },
    product{ _product }
{
    this->is_scroll = false;

    this->supply_pie = new UI::PieChart(0, 0, 128, 128, this);
    this->supply_pie->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductView&>(*w.parent);

        std::vector<UI::ChartData> nations_data;
        for(const auto& nation : o.gs.world->nations) {
            nations_data.push_back(UI::ChartData(0.f, nation->get_client_hint().alt_name, nation->get_client_hint().color));
        }

        // Account for products that are based on this good
        for(const auto& product : o.gs.world->products) {
            if(product->good != o.product->good && product->building != nullptr) {
                continue;
            }
            
            nations_data[o.gs.world->get_id(product->building->get_province()->controller)].num += product->building->get_province()->stockpile[o.gs.world->get_id(product->good)];
        }
        o.supply_pie->set_data(nations_data);
    });

    // Show average price of the good (accounting for all products on the world)
    this->price_chart = new UI::Chart(0, 152, 128, 64, this);
    this->price_chart->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }
        
        o.price_chart->data.push_back(o.product->price);
        if(o.price_chart->data.size() > 30) {
            o.price_chart->data.pop_front();
        }
    });

    this->supply_chart = new UI::Chart(0, 152, 128, 64, this);
    this->supply_chart->right_side_of(*this->price_chart);
    this->supply_chart->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        o.supply_chart->data.push_back(o.product->supply);
        if(o.supply_chart->data.size() > 30) {
            o.supply_chart->data.pop_front();
        }
    });

    this->demand_chart = new UI::Chart(0, 152, 128, 64, this);
    this->demand_chart->right_side_of(*this->supply_chart);
    this->demand_chart->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        o.demand_chart->data.push_back(o.product->demand);
        if(o.demand_chart->data.size() > 30) {
            o.demand_chart->data.pop_front();
        }
    });

    auto* good_btn = new UI::Button(0, 0, 128, 24, this);
    good_btn->below_of(*this->demand_chart);
    good_btn->text(product->good->name);
    good_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductView&>(*w.parent);
        new GoodView(o.gs, o.product->good);
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*good_btn);
    close_btn->text("Close");
}

GoodView::GoodView(GameState& _gs, Good* _good)
    : UI::Window(0, 0, 512, 320),
    gs{ _gs },
    good{ _good }
{
    unsigned int i;

    this->is_scroll = false;

    this->icon_img = new UI::Image(0, 0, 128, 96, nullptr, this);
    this->icon_img->current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/good/" + good->ref_name + ".png"));

    // Piechart denoting countries which have more supply of this good
    this->sellers_pie = new UI::PieChart(0, 0, 128, 128, this);
    this->sellers_pie->right_side_of(*this->icon_img);
    this->sellers_pie->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<GoodView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        std::vector<UI::ChartData> nations_data;
        for(const auto& nation : o.gs.world->nations) {
            nations_data.push_back(UI::ChartData(0.f, nation->get_client_hint().alt_name, nation->get_client_hint().color));
        }

        // Account for products that are based on this good
        for(const auto& product : o.gs.world->products) {
            if(product->building == nullptr || product->good != o.good) continue;
            nations_data[o.gs.world->get_id(product->building->owner)].num += product->supply;
        }

        o.sellers_pie->set_data(nations_data);
    });

    // Show average price of the good (accounting for all products on the world)
    this->avg_price_chart = new UI::Chart(0, 0, 128, 128, this);
    this->avg_price_chart->right_side_of(*this->sellers_pie);
    this->avg_price_chart->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<GoodView&>(*w.parent);

        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        float price = 0.f;
        for(const auto& product : o.gs.world->products) {
            if(product->good != o.good) {
                continue;
            }
            
            price += product->price;
        }
        price /= o.gs.world->products.size();

        o.avg_price_chart->data.push_back(price);
        if(o.avg_price_chart->data.size() > 30)
            o.avg_price_chart->data.pop_front();
    });

    // Industry types that produce this good
    this->producing_it_group = new UI::Group(0, 0, 128, 128, this);
    this->producing_it_group->below_of(*this->avg_price_chart);
    i = 0;
    for(const auto& it : this->gs.world->building_types) {
        bool is_present = std::find(it->outputs.begin(), it->outputs.end(), this->good) != it->outputs.end();
        if(!is_present) {
            continue;
        }

        UI::Button* it_btn = new UI::Button(0, 24 * i, 128, 24, this->producing_it_group);
        it_btn->text(it->name);
        i++;
    }

    // Industry types that consume this good
    this->consumer_it_group = new UI::Group(0, 0, 128, 128, this);
    this->consumer_it_group->right_side_of(*this->producing_it_group);
    i = 0;
    for(const auto& it : this->gs.world->building_types) {
        bool is_present = std::find(it->inputs.begin(), it->inputs.end(), this->good) != it->inputs.end();
        if(!is_present) {
            continue;
        }

        UI::Button* it_btn = new UI::Button(0, 24 * i, 128, 24, this->consumer_it_group);
        it_btn->text(it->name);
        i++;
    }

    this->name_lab = new UI::Label(0, 0, good->name, this);
    this->name_lab->below_of(*this->icon_img);

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*this->name_lab);
    close_btn->text("Close");
}