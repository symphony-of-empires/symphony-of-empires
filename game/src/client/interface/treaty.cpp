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
//      client/interface/treaty.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/treaty.hpp"
#include "io_impl.hpp"
#include "unified_render/ui/components.hpp"
#include "client/client_network.hpp"

using namespace Interface;

std::string treaty_to_text(Treaty* treaty) {
    std::string text = "";
    for(const auto& clause : treaty->clauses) {
        if(clause->type == TreatyClauseType::WAR_REPARATIONS) {
            const auto* dyn_clause = static_cast<const TreatyClause::WarReparations*>(clause);
            text += dyn_clause->sender->name + " demands $" + std::to_string(dyn_clause->amount) + " from " + dyn_clause->receiver->name;
        }
        else if(clause->type == TreatyClauseType::ANEXX_PROVINCES) {
            const auto* dyn_clause = static_cast<const TreatyClause::AnexxProvince*>(clause);
            text += dyn_clause->sender->name + " obtains ";
            for(const auto& province : dyn_clause->provinces) {
                text += province->name + ", ";
            }
            text += " from " + dyn_clause->receiver->name;
        }
        else if(clause->type == TreatyClauseType::LIBERATE_NATION) {
            const auto* dyn_clause = static_cast<const TreatyClause::LiberateNation*>(clause);
            text += dyn_clause->sender->name + " liberates " + dyn_clause->liberated->name + " and gives them ";
            for(const auto& province : dyn_clause->provinces) {
                text += province->name + " from " + dyn_clause->receiver->name + ", ";
            }
        }
        else if(clause->type == TreatyClauseType::HUMILIATE) {
            const auto* dyn_clause = static_cast<const TreatyClause::Humiliate*>(clause);
            text += dyn_clause->sender->name + " humiliates " + dyn_clause->receiver->name;
        }
        else if(clause->type == TreatyClauseType::IMPOSE_POLICIES) {
            const auto* dyn_clause = static_cast<const TreatyClause::ImposePolicies*>(clause);
            text += dyn_clause->sender->name + " imposes policies on " + dyn_clause->receiver->name;
        }
        else if(clause->type == TreatyClauseType::CEASEFIRE) {
            const auto* dyn_clause = static_cast<const TreatyClause::Ceasefire*>(clause);
            text += dyn_clause->sender->name + " proposes ceasefire to " + dyn_clause->receiver->name;
        }
        text += ", ";
    }
    return text;
}

TreatyDraftView::TreatyDraftView(GameState& _gs, Nation* _nation)
    : UI::Window(0, 0, 256, 512),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;
    this->text("Draft treaty");

    this->treaty.sender = gs.curr_nation;
    this->treaty.receiver = nation;

    auto* ceasefire_btn = new UI::Checkbox(0, 0, 128, 24, this);
    ceasefire_btn->text("Ceasefire");
    ceasefire_btn->set_on_click([](UI::Widget& w) {
        auto& o = static_cast<TreatyDraftView&>(*w.parent);

        if(((UI::Checkbox&)w).get_value()) {
            auto* clause = new TreatyClause::Ceasefire();
            clause->sender = o.gs.curr_nation;
            clause->receiver = o.nation;
            clause->days_duration = 360;
            o.treaty.clauses.push_back(clause);
        }
        else {
            auto it = std::find_if(o.treaty.clauses.begin(), o.treaty.clauses.end(), [](const auto& e) {
                return e->type == TreatyClauseType::CEASEFIRE;
            });

            if(it != o.treaty.clauses.end()) {
                o.treaty.clauses.erase(it);
            }
        }
    });

    auto* take_all_btn = new UI::Checkbox(0, 0, 128, 24, this);
    take_all_btn->below_of(*ceasefire_btn);
    take_all_btn->text("Take all controlled land");
    take_all_btn->set_on_click([](UI::Widget& w) {
        auto& o = static_cast<TreatyDraftView&>(*w.parent);

        if(((UI::Checkbox&)w).get_value()) {
            auto* clause = new TreatyClause::AnexxProvince();
            clause->sender = o.gs.curr_nation;
            clause->receiver = o.nation;
            clause->days_duration = 0;
            for(auto& province : o.nation->controlled_provinces) {
                clause->provinces.push_back(province);
            }
            o.treaty.clauses.push_back(clause);
        }
        else {
            auto it = std::find_if(o.treaty.clauses.begin(), o.treaty.clauses.end(), [](const auto& e) {
                return e->type == TreatyClauseType::ANEXX_PROVINCES;
            });

            if(it != o.treaty.clauses.end()) {
                o.treaty.clauses.erase(it);
            }
        }
    });

    auto* annexx_btn = new UI::Checkbox(0, 0, 128, 24, this);
    annexx_btn->below_of(*take_all_btn);
    annexx_btn->text("Annexx");
    annexx_btn->set_on_click([](UI::Widget& w) {
        auto& o = static_cast<TreatyDraftView&>(*w.parent);

        if(((UI::Checkbox&)w).get_value()) {
            auto* clause = new TreatyClause::AnexxProvince();
            clause->sender = o.gs.curr_nation;
            clause->receiver = o.nation;
            clause->days_duration = 0;
            for(auto& province : o.nation->owned_provinces) {
                clause->provinces.push_back(province);
            }
            o.treaty.clauses.push_back(clause);
        }
        else {
            auto it = std::find_if(o.treaty.clauses.begin(), o.treaty.clauses.end(), [](const auto& e) {
                return e->type == TreatyClauseType::ANEXX_PROVINCES;
            });

            if(it != o.treaty.clauses.end()) {
                o.treaty.clauses.erase(it);
            }
        }
    });

    auto* draft_btn = new UI::Button(0, 0, this->width, 24, this);
    draft_btn->below_of(*annexx_btn);
    draft_btn->text("Draft");
    draft_btn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TreatyDraftView&>(*w.parent);

        UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
        Archive ar = Archive();
        ActionType action = ActionType::DRAFT_TREATY;
        ::serialize(ar, &action);
        ::serialize(ar, &o.treaty.clauses);
        ::serialize(ar, &o.treaty.name);
        ::serialize(ar, &o.treaty.sender);
        packet.data(ar.get_buffer(), ar.size());
        std::scoped_lock lock(o.gs.client->pending_packets_mutex);
        o.gs.client->pending_packets.push_back(packet);

        o.gs.ui_ctx->prompt("Treaty", "Treaty drafted: " + treaty_to_text(&o.treaty));

        o.kill();
    });
}

TreatyChooseWindow::TreatyChooseWindow(GameState& _gs, Treaty* _treaty)
    : UI::Window(0, 0, 512, 256),
    gs{ _gs },
    treaty{ _treaty }
{
    this->is_scroll = true;
    this->text("Treaty proposal");

    this->body_txt = new UI::Text(0, 0, this->width, this->height - 24, this);
    this->body_txt->text(treaty_to_text(_treaty));

    auto* approve_btn = new UI::Button(0, 0, this->width, 24, this);
    approve_btn->below_of(*this->body_txt);
    approve_btn->text("Approve");
    approve_btn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TreatyChooseWindow&>(*w.parent);
        Archive ar = Archive();
        ActionType action = ActionType::CHANGE_TREATY_APPROVAL;
        ::serialize(ar, &action);
        ::serialize(ar, &o.treaty);
        TreatyApproval approval = TreatyApproval::ACCEPTED;
        ::serialize(ar, &approval);
        o.gs.send_command(ar);

        w.parent->kill();
    });

    auto* deny_btn = new UI::Button(0, 0, this->width, 24, this);
    deny_btn->below_of(*this->body_txt);
    deny_btn->right_side_of(*approve_btn);
    deny_btn->text("Deny");
    deny_btn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TreatyChooseWindow&>(*w.parent);

        Archive ar = Archive();
        ActionType action = ActionType::CHANGE_TREATY_APPROVAL;
        ::serialize(ar, &action);
        ::serialize(ar, &o.treaty);
        TreatyApproval approval = TreatyApproval::DENIED;
        ::serialize(ar, &approval);
        o.gs.send_command(ar);

        w.parent->kill();
    });
}