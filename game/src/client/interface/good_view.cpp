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
//      client/interface/good_view.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/texture.hpp"
#include "eng3d/locale.hpp"
#include "eng3d/ui/piechart.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/close_button.hpp"
#include "eng3d/ui/chart.hpp"
#include "eng3d/ui/group.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/tooltip.hpp"

#include "client/interface/good_view.hpp"
#include "io_impl.hpp"

using namespace Interface;

ProductView::ProductView(GameState& _gs, Product& _product)
    : UI::Window(0, 0, 512, 320),
    gs{ _gs },
    product{ _product }
{
    this->is_scroll = false;
    this->text(translate("ProductName"));

    this->set_close_btn_function([this](Widget&) {
        this->kill();
    });

    this->supply_pie = new UI::PieChart(0, 0, 128, 128, this);
    this->supply_pie->set_on_each_tick([this](UI::Widget&) {
        std::vector<UI::ChartData> nations_data;
        for(const auto& nation : this->gs.world->nations)
            nations_data.push_back(UI::ChartData(0.f, nation.get_client_hint().alt_name.get_string(), nation.get_client_hint().color));

        /// @todo Account for products that are based on this good on every province
        this->supply_pie->set_data(nations_data);
    });

    // Show average price of the good (accounting for all products on the world)
    this->price_chart = new UI::Chart(0, 0, 128, 64, this);
    this->price_chart->right_side_of(*this->supply_pie);
    this->price_chart->set_on_each_tick([this](UI::Widget&) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;
        this->price_chart->data.push_back(this->product.price);
        if(this->price_chart->data.size() > 30)
            this->price_chart->data.pop_front();
    });

    this->supply_chart = new UI::Chart(0, 0, 128, 64, this);
    this->supply_chart->right_side_of(*this->price_chart);
    this->supply_chart->set_on_each_tick([this](UI::Widget&) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;
        this->supply_chart->data.push_back(this->product.supply);
        if(this->supply_chart->data.size() > 30)
            this->supply_chart->data.pop_front();
    });

    this->demand_chart = new UI::Chart(0, 0, 128, 64, this);
    this->demand_chart->right_side_of(*this->supply_chart);
    this->demand_chart->set_on_each_tick([this](UI::Widget&) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;
        this->demand_chart->data.push_back(this->product.demand);
        if(this->demand_chart->data.size() > 30)
            this->demand_chart->data.pop_front();
    });

    auto* good_btn = new UI::Button(0, 0, 128, 24, this);
    good_btn->below_of(*this->demand_chart);
    //good_btn->text(product->good->name);
    good_btn->set_on_click([](UI::Widget&) {

    });
}

GoodView::GoodView(GameState& _gs, Good& _good)
    : UI::Window(0, 0, 512, 320),
    gs{ _gs },
    good{ _good }
{
    this->is_scroll = false;
    this->text(good.name.get_string());

    this->set_close_btn_function([this](Widget&) {
        this->kill();
    });

    this->icon_img = new UI::Image(0, 0, 128, 96, this->gs.tex_man.load(gs.package_man.get_unique("gfx/good/" + good.ref_name + ".png")), this);

    // Piechart denoting countries which have more supply of this good
    this->sellers_pie = new UI::PieChart(0, 0, 128, 128, this);
    this->sellers_pie->right_side_of(*this->icon_img);
    this->sellers_pie->set_on_each_tick([this](UI::Widget&) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;
        std::vector<UI::ChartData> nations_data;
        for(const auto& nation : this->gs.world->nations)
            nations_data.push_back(UI::ChartData(0.f, nation.get_client_hint().alt_name.get_string(), nation.get_client_hint().color));

        /// @todo Account for products that are based on this good
        this->sellers_pie->set_data(nations_data);
    });

    // Show average price of the good (accounting for all products on the world)
    this->avg_price_chart = new UI::Chart(0, 0, 128, 128, this);
    this->avg_price_chart->right_side_of(*this->sellers_pie);
    this->avg_price_chart->set_on_each_tick([this](UI::Widget&) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;

        float price = 0.f;
        //for(const auto& product : this->gs.world->products) {
        //    if(product->good != this->good) {
        //        continue;
        //    }
        //    
        //    price += product->price;
        //}
        //price /= this->gs.world->products.size();

        this->avg_price_chart->data.push_back(price);
        if(this->avg_price_chart->data.size() > 30)
            this->avg_price_chart->data.pop_front();
    });

    // Industry types that produce this good
    unsigned int dx = 0;

    // Outputs
    auto* output_lab = new UI::Label(dx, 0, translate("Producers"), this);
    output_lab->below_of(*avg_price_chart);
    dx += output_lab->width;
    for(const auto& building_type : this->gs.world->building_types) {
        bool is_present = (building_type.output == &this->good);
        if(!is_present) continue;
        auto* icon_ibtn = new UI::Image(dx, 0, 24, 24, this->gs.tex_man.load(gs.package_man.get_unique("gfx/production.png")), this);
        icon_ibtn->below_of(*avg_price_chart);
        icon_ibtn->set_tooltip(building_type.name.get_string());
        dx += icon_ibtn->width;
    }

    // Inputs
    auto* input_lab = new UI::Label(dx, 0, translate("Consumers"), this);
    input_lab->below_of(*avg_price_chart);
    dx += input_lab->width;
    for(const auto& building_type : this->gs.world->building_types) {
        bool is_present = std::find(building_type.inputs.begin(), building_type.inputs.end(), &this->good) != building_type.inputs.end();
        if(!is_present) continue;
        auto* icon_ibtn = new UI::Image(dx, 0, 24, 24, this->gs.tex_man.load(gs.package_man.get_unique("gfx/production.png")), this);
        icon_ibtn->below_of(*avg_price_chart);
        icon_ibtn->set_tooltip(building_type.name.get_string());
        dx += icon_ibtn->width;
    }
}