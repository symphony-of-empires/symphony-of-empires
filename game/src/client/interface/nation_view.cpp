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
//      client/interface/nation_view.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/components.hpp"
#include "eng3d/string.hpp"

#include "client/interface/nation_view.hpp"
#include "client/client_network.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"

Interface::NationView::NationView(GameState& _gs, Nation& _nation)
    : UI::Window(-400, -400, 800, 800),
    gs{ _gs },
    nation{ _nation }
{
    this->set_text(translate_format("General overview for %s", nation.name.data()));
    this->origin = UI::Origin::CENTER_SCREEN;
    this->is_scroll = false;
    this->set_on_each_tick([this](UI::Widget& w) {
        w.set_text(this->nation.get_client_hint().name.data());
    });
    this->on_each_tick(*this);
    this->set_close_btn_function([this](Widget&) {
        this->kill();
    });

    auto& flag_img = this->make_widget<UI::AspectImage>(0, 0, 128, 96, this->gs.get_nation_flag(this->nation));
    flag_img.set_on_each_tick([this](UI::Widget& w) {
        w.current_texture = this->gs.get_nation_flag(this->nation);
    });
    flag_img.on_each_tick(flag_img);

    auto& flex_actions_column = this->make_widget<UI::Div>(0, 0, 128, this->height);
    flex_actions_column.below_of(flag_img);
    flex_actions_column.flex = UI::Flex::COLUMN;

    auto& name_lab = flex_actions_column.make_widget<UI::Label>(0, 0, "?");
    name_lab.set_on_each_tick([this](UI::Widget& w) {
        w.set_text(this->nation.get_client_hint().name.data());
    });
    name_lab.on_each_tick(name_lab);

    auto& ideology_img = this->make_widget<UI::Image>(0, 0, 24, 24);
    ideology_img.set_on_each_tick([this](UI::Widget& w) {
        const auto& ideology = this->gs.world->ideologies[this->nation.get_client_hint().ideology_id];
        ((UI::Image&)w).current_texture = this->gs.tex_man.load(gs.package_man.get_unique(ideology.get_icon_path()));
        w.set_tooltip(ideology.ref_name.data());
    });
    ideology_img.on_each_tick(ideology_img);
    
    if(gs.curr_nation != &nation) {
        auto& rel_lab = flex_actions_column.make_widget<UI::Label>(0, 0, "?");
        rel_lab.set_on_each_tick([this](UI::Widget& w) {
            const auto& relation = this->gs.world->get_relation(*this->gs.curr_nation, this->nation);
            w.set_text(string_format("%.2f%%+%.2f%%", relation.relation * 100.f, relation.alliance * 100.f));

            std::string text = translate("Our diplomatic relations with them and our alliance:\n");
            text += (relation.is_customs_union() ? translate("- In customs union") : translate("- Not in a customs union")) + "\n";
            text += (relation.is_allied() ? translate("- Allied") : translate("- Not allied")) + "\n";
            text += (relation.has_war ? translate("- At war") : translate("- Not at war")) + "\n";
            text += (relation.has_landpass() ? translate("- Land access") : translate("- No land access")) + "\n";
            w.set_tooltip(text);
        });
        rel_lab.on_each_tick(rel_lab);
    }

    if(gs.curr_nation != &nation) {
        auto& dow_btn = flex_actions_column.make_widget<UI::Button>(0, 0, 128, 24);
        dow_btn.set_on_each_tick([this](UI::Widget& w) {
            const auto& relation = this->gs.world->get_relation(this->gs.world->get_id(*this->gs.curr_nation), this->gs.world->get_id(this->nation));
            if(relation.has_war) {
                w.set_text(translate("Propose treaty"));
                w.set_on_click([this](UI::Widget&) {
                    //new Interface::TreatyDraftView(this->gs, this->nation);
                });
                w.set_tooltip(translate("End the war against this country and propose a peace deal"));
            } else {
                w.set_text(translate("Declare war"));
                w.set_on_click([this](UI::Widget&) {
                    //new Interface::WarDeclarePrompt(this->gs, this->nation);
                });
                w.set_tooltip(translate("Declaring war on this nation will bring all their allies to their side"));
            }
        });
        dow_btn.on_each_tick(dow_btn);

        auto& allow_market_access_btn = flex_actions_column.make_widget<UI::Button>(0, 0, 128, 24);
        allow_market_access_btn.set_text(translate("Market access"));

        auto& allow_military_access_btn = flex_actions_column.make_widget<UI::Button>(0, 0, 128, 24);
        allow_military_access_btn.set_text(translate("Land access"));
        allow_military_access_btn.set_tooltip(translate("Allow this nation to cross our land with their units"));
    }

    if(gs.editor && gs.curr_nation != &nation) {
        auto& switch_btn = flex_actions_column.make_widget<UI::Button>(0, 0, 128, 24);
        switch_btn.set_text(translate("Switch to this nation"));
        switch_btn.set_tooltip(translate("Switches to this nation (multiplayer disallow rule)"));
        switch_btn.set_on_click([this](UI::Widget&) {
            this->gs.curr_nation = &this->nation;
            this->gs.map->map_render->request_update_visibility();
            this->gs.map->map_render->update();
        });
    }
}