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
//      client/interface/nation_view.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/components.hpp"
#include "eng3d/locale.hpp"

#include "client/interface/nation_view.hpp"
#include "client/client_network.hpp"
#include "client/interface/common.hpp"
#include "client/interface/treaty.hpp"
#include "client/interface/war.hpp"
#include "io_impl.hpp"

using namespace Interface;

NationMarketView::NationMarketView(GameState& _gs, Nation& _nation)
    : UI::Window(0, 0, 700, 600),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = true;
    this->text(Eng3D::Locale::translate("Market information"));
    this->set_close_btn_function([this](UI::Widget&) {
        this->kill();
    });

    size_t i = 0;
    for(const auto province_id : nation.controlled_provinces) {
        auto& province = gs.world->provinces[province_id];
        for(auto& good : gs.world->goods) {
            new ProductInfo(gs, 0, (i * 24) + 128, province, good, this);
            i++;
        }
    }
}

NationView::NationView(GameState& _gs, Nation& _nation)
    : UI::Window(0, 0, 256, 512),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;
    this->on_each_tick = ([this](UI::Widget& w) {
        w.text(this->nation.get_client_hint().alt_name.get_string());
    });
    this->on_each_tick(*this);
    this->set_close_btn_function([this](Widget&) {
        this->kill();
    });

    auto* flag_img = new UI::AspectImage(0, 0, 128, 96, this->gs.get_nation_flag(this->nation), this);
    flag_img->on_each_tick = ([this](UI::Widget& w) {
        w.current_texture = this->gs.get_nation_flag(this->nation);
    });
    flag_img->on_each_tick(*flag_img);
    flag_img->set_tooltip(Eng3D::Locale::translate("The flag which represents the country"));
    new UI::Image(0, 0, flag_img->width, flag_img->height, gs.tex_man.load(gs.package_man.get_unique("gfx/flag_rug.png")), this);

    auto* flex_actions_column = new UI::Div(0, 0, 512, 512, this);
    flex_actions_column->below_of(*flag_img);
    flex_actions_column->flex = UI::Flex::COLUMN;

    auto* name_lab = new UI::Label(0, 0, "?", flex_actions_column);
    name_lab->on_each_tick = ([this](UI::Widget& w) {
        w.text(this->nation.get_client_hint().alt_name.get_string());
    });
    name_lab->on_each_tick(*name_lab);
    name_lab->set_tooltip(Eng3D::Locale::translate("The official name"));

    auto* ideology_img = new UI::Image(0, 0, 24, 24, this);
    ideology_img->on_each_tick = ([this](UI::Widget& w) {
        ((UI::Image&)w).current_texture = this->gs.tex_man.load(this->gs.package_man.get_unique("gfx/ideology/" + this->nation.get_client_hint().ideology->ref_name.get_string() + ".png"));
        w.set_tooltip(Eng3D::Locale::translate(this->nation.get_client_hint().ideology->ref_name.get_string()));
    });
    ideology_img->on_each_tick(*ideology_img);
    
    if(gs.curr_nation != &nation) {
        auto* rel_lab = new UI::Label(0, 0, "?", flex_actions_column);
        rel_lab->on_each_tick = ([this](UI::Widget& w) {
            const auto& relation = this->gs.world->get_relation(this->gs.world->get_id(*this->gs.curr_nation), this->gs.world->get_id(this->nation));
            w.text(std::to_string(relation.relation));
        });
        rel_lab->on_each_tick(*rel_lab);
        rel_lab->set_tooltip(Eng3D::Locale::translate("Our diplomatic relations with them"));

        auto* interest_lab = new UI::Label(0, 0, "?", flex_actions_column);
        interest_lab->on_each_tick = ([this](UI::Widget& w) {
            const auto& relation = this->gs.world->get_relation(this->gs.world->get_id(*this->gs.curr_nation), this->gs.world->get_id(this->nation));
            w.text(std::to_string(relation.interest));
        });
        interest_lab->on_each_tick(*interest_lab);
        interest_lab->set_tooltip(Eng3D::Locale::translate("Interest/Tolerance towards them"));
    }

    auto* market_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
    market_btn->text(Eng3D::Locale::translate("Examine market"));
    market_btn->set_on_click([this](UI::Widget&) {
        new NationMarketView(this->gs, this->nation);
    });
    market_btn->set_tooltip(Eng3D::Locale::translate("View market information"));

    if(gs.curr_nation != &nation) {
        auto* inc_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        inc_btn->text(Eng3D::Locale::translate("Increment relations"));
        inc_btn->set_on_click([this](UI::Widget&) {
            this->gs.client->send(Action::DiploIncRelations::form_packet(this->nation));
        });

        auto* dec_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        dec_btn->text(Eng3D::Locale::translate("Decrement relations"));
        dec_btn->set_on_click([this](UI::Widget&) {
            this->gs.client->send(Action::DiploDecRelations::form_packet(this->nation));
        });

        auto* dow_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        dow_btn->on_each_tick = ([this](UI::Widget& w) {
            const auto& relation = this->gs.world->get_relation(this->gs.world->get_id(*this->gs.curr_nation), this->gs.world->get_id(this->nation));
            if(relation.has_war) {
                w.text(Eng3D::Locale::translate("Propose treaty"));
                w.set_on_click([this](UI::Widget&) {
                    new Interface::TreatyDraftView(this->gs, this->nation);
                });
                w.set_tooltip(Eng3D::Locale::translate("End the war against this country and propose a peace deal"));
            } else {
                w.text(Eng3D::Locale::translate("Declare war"));
                w.set_on_click([this](UI::Widget&) {
                    new Interface::WarDeclarePrompt(this->gs, this->nation);
                });
                w.set_tooltip(Eng3D::Locale::translate("Declaring war on this nation will bring all their allies to their side"));
            }
        });
        dow_btn->on_each_tick(*dow_btn);

        auto* ally_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        ally_btn->text(Eng3D::Locale::translate("Offer alliance"));
        ally_btn->set_tooltip(Eng3D::Locale::translate("Agree to mutually defend our interests forcing them to enter war with us"));

        auto* defensive_pact_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        defensive_pact_btn->text(Eng3D::Locale::translate("Defensive pact"));
        defensive_pact_btn->set_tooltip(Eng3D::Locale::translate("Mutually defend our countries from any foreign attacks"));

        auto* embargo_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        embargo_btn->text(Eng3D::Locale::translate("Embargo"));
        embargo_btn->set_tooltip(Eng3D::Locale::translate("Prevent imports/exports to this country"));

        auto* allow_market_access_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        allow_market_access_btn->text(Eng3D::Locale::translate("Allow market access"));

        auto* allow_military_access_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        allow_military_access_btn->text(Eng3D::Locale::translate("Allow military access"));
        allow_military_access_btn->set_tooltip(Eng3D::Locale::translate("Allow this nation to cross our land with their units"));
    }

    if(gs.editor && gs.curr_nation != &nation) {
        auto* switch_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        switch_btn->text(Eng3D::Locale::translate("Switch to this nation"));
        switch_btn->set_tooltip(Eng3D::Locale::translate("Switches to this nation (multiplayer disallow rule)"));
        switch_btn->set_on_click([this](UI::Widget&) {
            this->gs.curr_nation = &this->nation;
        });
    }
}