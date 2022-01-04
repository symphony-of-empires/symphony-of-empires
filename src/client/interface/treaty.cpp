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
#include "client/ui/components.hpp"

using namespace Interface;

TreatyDraftView::TreatyDraftView(GameState& _gs)
    : UI::Window(0, 0, 256, 512),
    gs{ _gs }
{
    this->is_scroll = false;
    this->text("Draft treaty");
    
    auto* draft_btn = new UI::Button(0, 24 * 2, this->width, 24, this);
    draft_btn->text("Draft");
}

std::string treaty_to_text(Treaty* treaty) {
    std::string text = "";
    for(const auto& clause : treaty->clauses) {
        if(clause->type == TreatyClauseType::WAR_REPARATIONS) {
            auto dyn_clause = static_cast<TreatyClause::WarReparations*>(clause);
            text += dyn_clause->sender->name + " demands $" + std::to_string(dyn_clause->amount) + " from " + dyn_clause->receiver->name;
        } else if(clause->type == TreatyClauseType::ANEXX_PROVINCES) {
            auto dyn_clause = static_cast<TreatyClause::AnexxProvince*>(clause);
            text += dyn_clause->sender->name + " obtains ";
            for(const auto& province : dyn_clause->provinces) {
                text += province->name + " from " + dyn_clause->receiver->name + ", ";
            }
        } else if(clause->type == TreatyClauseType::LIBERATE_NATION) {
            auto dyn_clause = static_cast<TreatyClause::LiberateNation*>(clause);
            text += dyn_clause->sender->name + " liberates " + dyn_clause->liberated->name + " and gives them ";
            for(const auto& province : dyn_clause->provinces) {
                text += province->name + " from " + dyn_clause->receiver->name + ", ";
            }
        } else if(clause->type == TreatyClauseType::HUMILIATE) {
            auto dyn_clause = static_cast<TreatyClause::Humiliate*>(clause);
            text += dyn_clause->sender->name + " humiliates " + dyn_clause->receiver->name;
        } else if(clause->type == TreatyClauseType::IMPOSE_POLICIES) {
            auto dyn_clause = static_cast<TreatyClause::ImposePolicies*>(clause);
            text += dyn_clause->sender->name + " imposes policies on " + dyn_clause->receiver->name;
        } else if(clause->type == TreatyClauseType::CEASEFIRE) {
            auto dyn_clause = static_cast<TreatyClause::Ceasefire*>(clause);
            text += dyn_clause->sender->name + " proposes ceasefire to " + dyn_clause->receiver->name;
        }
        text += ", ";
    }
    return text;
}

TreatyChooseWindow::TreatyChooseWindow(GameState& _gs, Treaty* _treaty)
    : UI::Window(0, 0, 512, 256),
    gs{ _gs },
    treaty{ _treaty }
{
    this->is_scroll = false;
    this->text("Treaty proposal");
    
    this->body_txt = new UI::Text(0, 0, this->width, 256 - 24, this);
    this->body_txt->text(treaty_to_text(_treaty));

    auto* approve_btn = new UI::Button(0, 0, this->width, 24, this);
    approve_btn->below_of(*this->body_txt);
    approve_btn->text("Approve");
    approve_btn->on_click = ([](UI::Widget& w, void*) {
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
    deny_btn->on_click = ([](UI::Widget& w, void*) {
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