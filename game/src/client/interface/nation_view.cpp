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

#include "unified_render/ui/components.hpp"
#include "unified_render/locale.hpp"

#include "client/interface/nation_view.hpp"
#include "client/client_network.hpp"
#include "client/interface/common.hpp"
#include "client/interface/treaty.hpp"
#include "client/interface/war.hpp"
#include "io_impl.hpp"

using namespace Interface;

NationMarketView::NationMarketView(GameState& _gs, Nation* _nation)
    : UI::Window(0, 0, 700, 600),
    gs{ _gs },
    nation{ _nation }
{
    //this->is_scroll = false;
    this->text(UnifiedRender::Locale::translate("Market information"));

    unsigned int i = 0;
    /*for(const auto& product : gs.world->products) {
        //if(product->building == nullptr || product->building->get_owner() != nation) {
        //    continue;
        //}

        new ProductInfo(gs, 0, (i * 24) + 128, product, this);
        i++;
    }*/

    auto* close_btn = new UI::CloseButton(0, 0, this->width, 24, this);
    close_btn->text(UnifiedRender::Locale::translate("Close"));
}

NationView::NationView(GameState& _gs, Nation* _nation)
    : UI::Window(0, 0, 256, 512),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;
    this->on_each_tick = ([this](UI::Widget& w) {
        w.text(this->nation->get_client_hint().alt_name);
    });
    this->on_each_tick(*this);

    auto* flag_img = new UI::AspectImage(0, 0, 128, 96, this->gs.get_nation_flag(*this->nation), this);
    flag_img->on_each_tick = ([this](UI::Widget& w) {
        w.current_texture = this->gs.get_nation_flag(*this->nation);
    });
    flag_img->on_each_tick(*flag_img);
    flag_img->tooltip = new UI::Tooltip(flag_img, 512, 24);
    flag_img->tooltip->text(UnifiedRender::Locale::translate("The flag which represents the country"));
    auto* flag_rug = new UI::Image(0, 0, flag_img->width, flag_img->height, gs.tex_man->load(Path::get("gfx/flag_rug.png")), this);

    auto* flex_actions_column = new UI::Div(0, 0, 512, 512, this);
    flex_actions_column->below_of(*flag_img);
    flex_actions_column->flex = UI::Flex::COLUMN;

    auto* name_lab = new UI::Label(0, 0, "?", flex_actions_column);
    name_lab->on_each_tick = ([this](UI::Widget& w) {
        w.text(this->nation->get_client_hint().alt_name);
    });
    name_lab->on_each_tick(*name_lab);
    name_lab->tooltip = new UI::Tooltip(name_lab, 512, 24);
    name_lab->tooltip->text(UnifiedRender::Locale::translate("The official name"));

    auto* ideology_lab = new UI::Label(0, 0, "?", flex_actions_column);
    ideology_lab->on_each_tick = ([this](UI::Widget& w) {
        w.text(this->nation->get_client_hint().ideology->name);
    });
    ideology_lab->on_each_tick(*ideology_lab);
    ideology_lab->tooltip = new UI::Tooltip(ideology_lab, 512, 24);
    ideology_lab->tooltip->text(UnifiedRender::Locale::translate("The ideology according to their policies"));

    UI::Label* interest_lab;
    UI::Label* rel_lab;
    if(gs.curr_nation != nation) {
        rel_lab = new UI::Label(0, 0, "?", flex_actions_column);
        rel_lab->on_each_tick = ([this](UI::Widget& w) {
            w.text(std::to_string(this->gs.curr_nation->relations[this->gs.world->get_id(*this->nation)].relation));
        });
        rel_lab->on_each_tick(*rel_lab);
        rel_lab->tooltip = new UI::Tooltip(rel_lab, 512, 24);
        rel_lab->tooltip->text(UnifiedRender::Locale::translate("Our diplomatic relations with them"));

        interest_lab = new UI::Label(0, 0, "?", flex_actions_column);
        interest_lab->on_each_tick = ([this](UI::Widget& w) {
            w.text(std::to_string(this->gs.curr_nation->relations[this->gs.world->get_id(*this->nation)].interest));
        });
        interest_lab->on_each_tick(*interest_lab);
        interest_lab->tooltip = new UI::Tooltip(interest_lab, 512, 24);
        interest_lab->tooltip->text(UnifiedRender::Locale::translate("Interest/Tolerance towards them"));
    }

    auto* market_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
    market_btn->text(UnifiedRender::Locale::translate("Examine market"));
    market_btn->set_on_click([](UI::Widget& w) {
        auto& o = static_cast<NationView&>(*w.parent);
        new NationMarketView(o.gs, o.nation);
    });
    market_btn->tooltip = new UI::Tooltip(market_btn, 512, 24);
    market_btn->tooltip->text(UnifiedRender::Locale::translate("View market information"));

    if(gs.curr_nation != nation) {
        auto* inc_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        inc_btn->text(UnifiedRender::Locale::translate("Increment relations"));
        inc_btn->set_on_click([](UI::Widget& w) {
            auto& o = static_cast<NationView&>(*w.parent);
            g_client->send(Action::DiploIncRelations::form_packet(o.nation));
        });

        auto* dec_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        dec_btn->text(UnifiedRender::Locale::translate("Decrement relations"));
        dec_btn->set_on_click([](UI::Widget& w) {
            auto& o = static_cast<NationView&>(*w.parent);
            g_client->send(Action::DiploDecRelations::form_packet(o.nation));
        });

        auto* dow_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        dow_btn->tooltip = new UI::Tooltip(dow_btn, 512, 24);
        dow_btn->on_each_tick = ([this](UI::Widget& w) {
            if(this->gs.curr_nation->relations[this->gs.world->get_id(*this->nation)].has_war) {
                w.text(UnifiedRender::Locale::translate("Propose treaty"));
                w.set_on_click([this](UI::Widget& w) {
                    new Interface::TreatyDraftView(this->gs, this->nation);
                });
                w.tooltip->text(UnifiedRender::Locale::translate("End the war against this country and propose a peace deal"));
            } else {
                w.text(UnifiedRender::Locale::translate("Declare war"));
                w.set_on_click([this](UI::Widget& w) {
                    new Interface::WarDeclarePrompt(this->gs, this->nation);
                });
                w.tooltip->text(UnifiedRender::Locale::translate("Declaring war on this nation will bring all their allies to their side"));
            }
        });
        dow_btn->on_each_tick(*dow_btn);

        auto* ally_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        ally_btn->text(UnifiedRender::Locale::translate("Offer alliance"));
        ally_btn->tooltip = new UI::Tooltip(ally_btn, 512, 24);
        ally_btn->tooltip->text(UnifiedRender::Locale::translate("Agree to mutually defend our interests forcing them to enter war with us"));

        auto* defensive_pact_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        defensive_pact_btn->text(UnifiedRender::Locale::translate("Defensive pact"));
        defensive_pact_btn->tooltip = new UI::Tooltip(defensive_pact_btn, 512, 24);
        defensive_pact_btn->tooltip->text(UnifiedRender::Locale::translate("Mutually defend our countries from any foreign attacks"));

        auto* embargo_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        embargo_btn->text(UnifiedRender::Locale::translate("Embargo"));
        embargo_btn->tooltip = new UI::Tooltip(embargo_btn, 512, 24);
        embargo_btn->tooltip->text(UnifiedRender::Locale::translate("Prevent imports/exports to this country"));

        auto* allow_market_access_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        allow_market_access_btn->text(UnifiedRender::Locale::translate("Allow market access"));

        auto* allow_military_access_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        allow_military_access_btn->text(UnifiedRender::Locale::translate("Allow military access"));
        allow_military_access_btn->tooltip = new UI::Tooltip(allow_military_access_btn, 512, 24);
        allow_military_access_btn->tooltip->text(UnifiedRender::Locale::translate("Allow this nation to cross our land with their units"));
    }

    if(gs.editor && gs.curr_nation != nation) {
        auto* switch_btn = new UI::Button(0, 0, this->width, 24, flex_actions_column);
        switch_btn->text(UnifiedRender::Locale::translate("Switch to this nation"));
        switch_btn->tooltip = new UI::Tooltip(switch_btn, 512, 24);
        switch_btn->tooltip->text(UnifiedRender::Locale::translate("Switches to this nation (multiplayer disallow rule)"));
        switch_btn->set_on_click([this](UI::Widget& w) {
            this->gs.curr_nation = this->nation;
        });
    }
}