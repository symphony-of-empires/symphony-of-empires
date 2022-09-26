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
//      client/interface/treaty.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include "eng3d/ui/components.hpp"
#include "action.hpp"

#include "client/interface/treaty.hpp"
#include "client/client_network.hpp"

using namespace Interface;

std::string treaty_to_text(const Treaty& treaty) {
    std::string text = "";
    for(const auto& clause : treaty.clauses) {
        if(clause->type == TreatyClauseType::MONEY) {
            const auto* dyn_clause = static_cast<const TreatyClause::WarReparations*>(clause);
            text += g_world.nations[dyn_clause->sender_id].name + " demands $" + std::to_string(dyn_clause->amount) + " from " + g_world.nations[dyn_clause->receiver_id].name;
        } else if(clause->type == TreatyClauseType::ANNEX_PROVINCES) {
            const auto* dyn_clause = static_cast<const TreatyClause::AnnexProvince*>(clause);
            text += g_world.nations[dyn_clause->sender_id].name + " obtains ";
            for(const auto& province : dyn_clause->provinces) {
                text += province->name + ", ";
            }
            text += " from " + g_world.nations[dyn_clause->receiver_id].name;
        } else if(clause->type == TreatyClauseType::LIBERATE_NATION) {
            const auto* dyn_clause = static_cast<const TreatyClause::LiberateNation*>(clause);
            text += g_world.nations[dyn_clause->sender_id].name + " liberates " + dyn_clause->liberated->name + " and gives them ";
            for(const auto& province : dyn_clause->provinces)
                text += province->name + " from " + g_world.nations[dyn_clause->receiver_id].name + ", ";
        } else if(clause->type == TreatyClauseType::HUMILIATE) {
            const auto* dyn_clause = static_cast<const TreatyClause::Humiliate*>(clause);
            text += g_world.nations[dyn_clause->sender_id].name + " humiliates " + g_world.nations[dyn_clause->receiver_id].name;
        } else if(clause->type == TreatyClauseType::IMPOSE_POLICIES) {
            const auto* dyn_clause = static_cast<const TreatyClause::ImposePolicies*>(clause);
            text += g_world.nations[dyn_clause->sender_id].name + " imposes policies on " + g_world.nations[dyn_clause->receiver_id].name;
        } else if(clause->type == TreatyClauseType::PUPPET) {
            const auto* dyn_clause = static_cast<const TreatyClause::Puppet*>(clause);
            text += g_world.nations[dyn_clause->sender_id].name + " vassalizes " + g_world.nations[dyn_clause->receiver_id].name;
        }
        text += ", ";
    }
    return text;
}

TreatyDraftView::TreatyDraftView(GameState& _gs, Nation& _nation)
    : UI::Window(0, 0, 256, 512),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;
    this->text("Draft treaty");

    this->treaty.sender_id = *gs.curr_nation;
    this->treaty.receiver_id = this->nation;

    auto* take_all_btn = new UI::Checkbox(0, 0, 128, 24, this);
    take_all_btn->text("Take all controlled land");
    take_all_btn->set_on_click([this](UI::Widget& w) {
        if(((UI::Checkbox&)w).get_value()) {
            auto* clause = new TreatyClause::AnnexProvince();
            clause->sender_id = *this->gs.curr_nation;
            clause->receiver_id = this->nation;
            clause->days_duration = 0;
            for(const auto province_id : this->nation.controlled_provinces)
                clause->provinces.push_back(&this->gs.world->provinces[province_id]);
            this->treaty.clauses.push_back(clause);
        } else {
            std::erase_if(this->treaty.clauses, [](const auto& e) {
                return e->type == TreatyClauseType::ANNEX_PROVINCES;
            });
        }
    });

    auto* annexx_btn = new UI::Checkbox(0, 0, 128, 24, this);
    annexx_btn->below_of(*take_all_btn);
    annexx_btn->text("Annexx");
    annexx_btn->set_on_click([this](UI::Widget& w) {
        if(((UI::Checkbox&)w).get_value()) {
            auto* clause = new TreatyClause::AnnexProvince();
            clause->sender_id = *this->gs.curr_nation;
            clause->receiver_id = this->nation;
            clause->days_duration = 0;
            for(const auto province_id : this->nation.owned_provinces)
                clause->provinces.push_back(&this->gs.world->provinces[province_id]);
            this->treaty.clauses.push_back(clause);
        } else {
            std::erase_if(this->treaty.clauses, [](const auto& e) {
                return e->type == TreatyClauseType::ANNEX_PROVINCES;
            });
        }
    });

    auto* draft_btn = new UI::Button(0, 0, this->width, 24, this);
    draft_btn->below_of(*annexx_btn);
    draft_btn->text("Draft");
    draft_btn->set_on_click([this](UI::Widget&) {
        Eng3D::Networking::Packet packet{};
        Archive ar{};
        ::serialize<ActionType>(ar, ActionType::DRAFT_TREATY);
        ::serialize(ar, this->treaty.clauses);
        ::serialize(ar, this->treaty.name);
        ::serialize(ar, this->treaty.sender_id);
        ::serialize(ar, this->treaty.receiver_id);
        packet.data(ar.get_buffer(), ar.size());
        this->gs.client->send(packet);
        this->gs.ui_ctx.prompt("Treaty", "Treaty drafted: " + treaty_to_text(this->treaty));
        this->kill();
    });
}

TreatyChooseWindow::TreatyChooseWindow(GameState& _gs, const TreatyId _treaty_id)
    : UI::Window(0, 0, 512, 256),
    gs{ _gs },
    treaty_id{ _treaty_id }
{
    this->is_scroll = true;
    this->text("Treaty proposal");

    this->body_txt = new UI::Text(0, 0, this->width, this->height - 24, this);
    this->body_txt->text(treaty_to_text(this->gs.world->treaties[this->treaty_id]));

    auto* approve_btn = new UI::Button(0, 0, 128, 24, this);
    approve_btn->below_of(*this->body_txt);
    approve_btn->text("Approve");
    approve_btn->set_on_click([this](UI::Widget& w) {
        Archive ar{};
        ::serialize<ActionType>(ar, ActionType::CHANGE_TREATY_APPROVAL);
        ::serialize(ar, this->treaty_id);
        ::serialize<TreatyApproval>(ar, TreatyApproval::ACCEPTED);
        Eng3D::Networking::Packet packet{};
        packet.data(ar.get_buffer(), ar.size());
        this->gs.client->send(packet);
        w.parent->kill();
    });

    auto* deny_btn = new UI::Button(0, 0, 128, 24, this);
    deny_btn->below_of(*this->body_txt);
    deny_btn->right_side_of(*approve_btn);
    deny_btn->text("Deny");
    deny_btn->set_on_click([this](UI::Widget& w) {
        Archive ar{};
        ::serialize<ActionType>(ar, ActionType::CHANGE_TREATY_APPROVAL);
        ::serialize(ar, this->treaty_id);
        ::serialize<TreatyApproval>(ar, TreatyApproval::DENIED);
        Eng3D::Networking::Packet packet{};
        packet.data(ar.get_buffer(), ar.size());
        this->gs.client->send(packet);
        w.parent->kill();
    });
}