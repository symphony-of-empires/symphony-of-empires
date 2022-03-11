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
    this->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<NationView&>(w);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        w.text(o.nation->get_client_hint().alt_name);
    });

    auto* flag_img = new UI::Image(0, 0, 128, 96, nullptr, this);
    flag_img->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<NationView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }
        w.current_texture = &o.gs.get_nation_flag(*o.nation);
    });
    flag_img->tooltip = new UI::Tooltip(flag_img, 512, 24);
    flag_img->tooltip->text(UnifiedRender::Locale::translate("The flag which represents the country"));

    auto* name_lab = new UI::Label(0, 0, "?", this);
    name_lab->below_of(*flag_img);
    name_lab->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<NationView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }
        w.text(o.nation->get_client_hint().alt_name);
    });
    name_lab->tooltip = new UI::Tooltip(name_lab, 512, 24);
    name_lab->tooltip->text(UnifiedRender::Locale::translate("The official name"));

    auto* ideology_lab = new UI::Label(0, 0, "?", this);
    ideology_lab->below_of(*name_lab);
    ideology_lab->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<NationView&>(*w.parent);
       if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }
        w.text(o.nation->get_client_hint().ideology->name);
    });
    ideology_lab->tooltip = new UI::Tooltip(ideology_lab, 512, 24);
    ideology_lab->tooltip->text(UnifiedRender::Locale::translate("The ideology according to their policies"));

    UI::Label* interest_lab;
    UI::Label* rel_lab;
    if(gs.curr_nation != nation) {
        rel_lab = new UI::Label(0, 0, "?", this);
        rel_lab->below_of(*ideology_lab);
        rel_lab->on_each_tick = ([](UI::Widget& w) {
            auto& o = static_cast<NationView&>(*w.parent);
            if(o.gs.world->time % o.gs.world->ticks_per_month) {
                return;
            }
            w.text(std::to_string(o.gs.curr_nation->relations[o.gs.world->get_id(*o.nation)].relation));
        });
        rel_lab->tooltip = new UI::Tooltip(rel_lab, 512, 24);
        rel_lab->tooltip->text(UnifiedRender::Locale::translate("Our diplomatic relations with them"));

        interest_lab = new UI::Label(0, 0, "?", this);
        interest_lab->below_of(*rel_lab);
        interest_lab->on_each_tick = ([](UI::Widget& w) {
            auto& o = static_cast<NationView&>(*w.parent);
            if(o.gs.world->time % o.gs.world->ticks_per_month) {
                return;
            }
            w.text(std::to_string(o.gs.curr_nation->relations[o.gs.world->get_id(*o.nation)].interest));
        });
        interest_lab->tooltip = new UI::Tooltip(interest_lab, 512, 24);
        interest_lab->tooltip->text(UnifiedRender::Locale::translate("Interest/Tolerance towards them"));
    }

    auto* market_btn = new UI::Button(0, 0, this->width, 24, this);
    if(gs.curr_nation != nation) {
        market_btn->below_of(*interest_lab);
    } else {
        market_btn->below_of(*ideology_lab);
    }
    market_btn->text(UnifiedRender::Locale::translate("Examine market"));
    market_btn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<NationView&>(*w.parent);
        new NationMarketView(o.gs, o.nation);
    });
    market_btn->tooltip = new UI::Tooltip(market_btn, 512, 24);
    market_btn->tooltip->text(UnifiedRender::Locale::translate("View market information"));

    auto* close_btn = new UI::CloseButton(0, 0, this->width, 24, this);
    if(gs.curr_nation != nation) {
        auto* inc_btn = new UI::Button(0, 0, this->width, 24, this);
        inc_btn->below_of(*market_btn);
        inc_btn->text(UnifiedRender::Locale::translate("Increment relations"));
        inc_btn->on_click = ([](UI::Widget& w) {
            auto& o = static_cast<NationView&>(*w.parent);
            g_client->send(Action::DiploIncRelations::form_packet(o.nation));
        });

        auto* dec_btn = new UI::Button(0, 0, this->width, 24, this);
        dec_btn->below_of(*inc_btn);
        dec_btn->text(UnifiedRender::Locale::translate("Decrement relations"));
        dec_btn->on_click = ([](UI::Widget& w) {
            auto& o = static_cast<NationView&>(*w.parent);
            g_client->send(Action::DiploDecRelations::form_packet(o.nation));
        });

        auto* dow_btn = new UI::Button(0, 0, this->width, 24, this);
        dow_btn->below_of(*dec_btn);
        dow_btn->tooltip = new UI::Tooltip(dow_btn, 512, 24);
        dow_btn->on_each_tick = ([](UI::Widget& w) {
            auto& o = static_cast<NationView&>(*w.parent);

            if(o.gs.curr_nation->relations[o.gs.world->get_id(*o.nation)].has_war) {
                w.text(UnifiedRender::Locale::translate("Propose treaty"));
                w.on_click = ([](UI::Widget& w) {
                    auto& o = static_cast<NationView&>(*w.parent);
                    new Interface::TreatyDraftView(o.gs, o.nation);
                });
                w.tooltip->text(UnifiedRender::Locale::translate("End the war against this country and propose a peace deal"));
            } else {
                w.text(UnifiedRender::Locale::translate("Declare war"));
                w.on_click = ([](UI::Widget& w) {
                    auto& o = static_cast<NationView&>(*w.parent);
                    new Interface::WarDeclarePrompt(o.gs, o.nation);
                });
                w.tooltip->text(UnifiedRender::Locale::translate("Declaring war on this nation will bring all their allies to their side"));
            }
        });
        dow_btn->on_each_tick(*dow_btn);

        auto* ally_btn = new UI::Button(0, 0, this->width, 24, this);
        ally_btn->below_of(*dow_btn);
        ally_btn->text(UnifiedRender::Locale::translate("Offer alliance"));
        ally_btn->tooltip = new UI::Tooltip(ally_btn, 512, 24);
        ally_btn->tooltip->text(UnifiedRender::Locale::translate("Agree to mutually defend our interests forcing them to enter war with us"));

        auto* defensive_pact_btn = new UI::Button(0, 0, this->width, 24, this);
        defensive_pact_btn->below_of(*ally_btn);
        defensive_pact_btn->text(UnifiedRender::Locale::translate("Defensive pact"));
        defensive_pact_btn->tooltip = new UI::Tooltip(defensive_pact_btn, 512, 24);
        defensive_pact_btn->tooltip->text(UnifiedRender::Locale::translate("Mutually defend our countries from any foreign attacks"));

        auto* embargo_btn = new UI::Button(0, 0, this->width, 24, this);
        embargo_btn->below_of(*defensive_pact_btn);
        embargo_btn->text(UnifiedRender::Locale::translate("Embargo"));
        embargo_btn->tooltip = new UI::Tooltip(embargo_btn, 512, 24);
        embargo_btn->tooltip->text(UnifiedRender::Locale::translate("Prevent imports/exports to this country"));
        
        auto* allow_market_access_btn = new UI::Button(0, 0, this->width, 24, this);
        allow_market_access_btn->below_of(*embargo_btn);
        allow_market_access_btn->text(UnifiedRender::Locale::translate("Allow market access"));

        auto* allow_military_access_btn = new UI::Button(0, 0, this->width, 24, this);
        allow_military_access_btn->below_of(*allow_market_access_btn);
        allow_military_access_btn->text(UnifiedRender::Locale::translate("Allow military access"));
        allow_military_access_btn->tooltip = new UI::Tooltip(allow_military_access_btn, 512, 24);
        allow_military_access_btn->tooltip->text(UnifiedRender::Locale::translate("Allow this nation to cross our land with their units"));
        close_btn->below_of(*allow_military_access_btn);
    } else {
        close_btn->below_of(*market_btn);
    }
    close_btn->text("Close");

    if(gs.editor && gs.curr_nation != nation) {
        auto* switch_btn = new UI::Button(0, 0, this->width, 24, this);
        switch_btn->below_of(*close_btn);
        switch_btn->text(UnifiedRender::Locale::translate("Switch to this nation"));
        switch_btn->tooltip = new UI::Tooltip(switch_btn, 512, 24);
        switch_btn->tooltip->text(UnifiedRender::Locale::translate("Switches to this nation (multiplayer disallow rule)"));
        switch_btn->on_click = ([](UI::Widget& w) {
            auto& o = static_cast<NationView&>(*w.parent);
            o.gs.curr_nation = o.nation;
        });
    }
}