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
//      client/interface/ai.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/ai.hpp"
#include "io_impl.hpp"
#include "unified_render/ui/components.hpp"
#include "client/client_network.hpp"
#include "unified_render/locale.hpp"

using namespace Interface;

AISettingsWindow::AISettingsWindow(GameState& _gs)
    : UI::Window(0, 0, 512, 256),
    gs{ _gs }
{
    this->is_scroll = false;
    this->text("AI Control");

    auto* build_prod_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    build_prod_chk->text(UnifiedRender::Locale::translate("Build and production"));
    build_prod_chk->set_value(this->gs.curr_nation->ai_do_build_production);
    build_prod_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_do_build_production = ((UI::Checkbox&)w).get_value();
    });
    build_prod_chk->set_tooltip("Building construction will be managed by AI");

    auto* cmd_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    cmd_chk->below_of(*build_prod_chk);
    cmd_chk->text(UnifiedRender::Locale::translate("Command troops"));
    cmd_chk->set_value(this->gs.curr_nation->ai_do_cmd_troops);
    cmd_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_do_cmd_troops = ((UI::Checkbox&)w).get_value();
    });
    cmd_chk->set_tooltip("Units will be managed by the AI");

    auto* diplo_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    diplo_chk->below_of(*cmd_chk);
    diplo_chk->text(UnifiedRender::Locale::translate("Diplomacy"));
    diplo_chk->set_value(this->gs.curr_nation->ai_do_diplomacy);
    diplo_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_do_diplomacy = ((UI::Checkbox&)w).get_value();
    });
    diplo_chk->set_tooltip("AI will manage relations, colonization and wars with foreign states");

    auto* policies_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    policies_chk->below_of(*diplo_chk);
    policies_chk->text(UnifiedRender::Locale::translate("Policies"));
    policies_chk->set_value(this->gs.curr_nation->ai_do_policies);
    policies_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_do_policies = ((UI::Checkbox&)w).get_value();
    });
    policies_chk->set_tooltip("Policies will be enacted by AI");

    auto* research_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    research_chk->below_of(*policies_chk);
    research_chk->text(UnifiedRender::Locale::translate("Research"));
    research_chk->set_value(this->gs.curr_nation->ai_do_research);
    research_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_do_research = ((UI::Checkbox&)w).get_value();
    });
    research_chk->set_tooltip("Research will automatically be done by AI");

    auto* unit_production_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    unit_production_chk->below_of(*research_chk);
    unit_production_chk->text(UnifiedRender::Locale::translate("Unit Production"));
    unit_production_chk->set_value(this->gs.curr_nation->ai_do_unit_production);
    unit_production_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_do_unit_production = ((UI::Checkbox&)w).get_value();
    });
    unit_production_chk->set_tooltip("Unit production will be managed by AI");

    auto* hdl_event_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    hdl_event_chk->below_of(*unit_production_chk);
    hdl_event_chk->text(UnifiedRender::Locale::translate("Handle events"));
    hdl_event_chk->set_value(this->gs.curr_nation->ai_handle_events);
    hdl_event_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_handle_events = ((UI::Checkbox&)w).get_value();
    });
    hdl_event_chk->set_tooltip("AI will automatically decide on events/descisions");

    auto* hdl_treaties_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    hdl_treaties_chk->below_of(*hdl_event_chk);
    hdl_treaties_chk->text(UnifiedRender::Locale::translate("Handle treaties"));
    hdl_treaties_chk->set_value(this->gs.curr_nation->ai_handle_treaties);
    hdl_treaties_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_handle_treaties = ((UI::Checkbox&)w).get_value();
    });
    hdl_treaties_chk->set_tooltip("Treaties will be signed/rejected by the AI");

    auto* close_btn = new UI::CloseButton(0, 0, this->width, 24, this);
    close_btn->below_of(*hdl_treaties_chk);
    close_btn->text(UnifiedRender::Locale::translate("Close"));
    close_btn->set_on_click([this](UI::Widget& w) {
        this->gs.client->send(Action::AiControl::form_packet(this->gs.curr_nation));
        this->kill();
    });
}