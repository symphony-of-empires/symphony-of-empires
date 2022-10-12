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
//      client/interface/decision.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/serializer.hpp"
#include "eng3d/ui/text.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/close_button.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/network.hpp"

#include "client/client_network.hpp"
#include "client/interface/decision.hpp"
#include "event.hpp"
#include "action.hpp"
#include "client/game_state.hpp"

Interface::DecisionWindow::DecisionWindow(GameState& _gs, Event _event)
    : UI::Window(128, 128, 320, 570),
    gs{ _gs },
    event{ _event }
{
    // Title of the event
    this->text(this->event.title);
    this->flex = UI::Flex::COLUMN;

    auto* txt = new UI::Text(0, 0, this->width, 24, this);
    txt->text_color = Eng3D::Color::rgb8(0, 0, 0);
    txt->text(this->event.text);
    txt->is_scroll = true;

    // Buttons for decisions for the event
    for(const auto& decision : this->event.decisions) {
        auto* flex_column =  new UI::Div(0, 0, this->width, 24, this);
        flex_column->flex = UI::Flex::ROW;

        auto* decide_btn = new UI::Button(0, 0, flex_column->width - 24, 24, flex_column);
        decide_btn->text(decision.name);
        decide_btn->set_tooltip(decision.effects);
        decide_btn->set_on_click([this, &decision](UI::Widget&) {
            this->gs.client->send(Action::NationTakeDecision::form_packet(this->event, decision));
            this->kill();
        });

        auto* remind_ibtn = new UI::Image(0, 0, 24, 24, "gfx/noicon.png", flex_column);
        remind_ibtn->set_tooltip("Automatically take this descision");
        remind_ibtn->set_on_click([this, &decision](UI::Widget&) {
            this->gs.decision_autodo.push_back(decision.ref_name);
            this->gs.client->send(Action::NationTakeDecision::form_packet(this->event, decision));
            this->kill();
        });

        // Check if decision has been automatized
        auto it = std::find(this->gs.decision_autodo.begin(), this->gs.decision_autodo.end(), decision.ref_name);
        if(it != this->gs.decision_autodo.end()) {
            decide_btn->on_click(*decide_btn);
            return;
        }
    }
}
