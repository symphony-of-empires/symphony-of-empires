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
//      client/interface/war.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/war.hpp"
#include "client/client_network.hpp"
#include "io_impl.hpp"
#include "action.hpp"
#include "unified_render/ui/components.hpp"

using namespace Interface;

WarDeclarePrompt::WarDeclarePrompt(GameState& _gs, Nation* _nation)
    : UI::Window(0, 0, 256, 512),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;

    this->body_txt = new UI::Text(0, 0, this->width, 24, this);
    this->body_txt->text("TODO: add cassus bellis");

    this->approve_btn = new UI::Button(0, 24, this->width, 24, this);
    this->approve_btn->text("Declare war");
    this->approve_btn->set_on_click([](UI::Widget& w) {
        auto& o = static_cast<WarDeclarePrompt&>(*w.parent);
        o.gs.client->send(Action::DiploDeclareWar::form_packet(o.nation));
        w.parent->kill();
    });

    this->deny_btn = new UI::Button(0, 48, this->width, 24, this);
    this->deny_btn->text("Nevermind");
    this->deny_btn->set_on_click([](UI::Widget& w) {
        auto& o = static_cast<WarDeclarePrompt&>(*w.parent);
        w.parent->kill();
    });
}