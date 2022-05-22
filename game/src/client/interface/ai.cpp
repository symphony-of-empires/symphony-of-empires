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
#include "eng3d/ui/components.hpp"
#include "client/client_network.hpp"
#include "eng3d/locale.hpp"

using namespace Interface;

AISettingsWindow::AISettingsWindow(GameState& _gs)
    : UI::Window(0, 0, 512, 256),
    gs{ _gs }
{
    this->is_scroll = false;
    this->text("AI Control");

    auto* cmd_chk = new UI::Checkbox(0, 0, this->width, 24, this);
    cmd_chk->text(Eng3D::Locale::translate("Command troops"));
    cmd_chk->set_value(this->gs.curr_nation->ai_do_cmd_troops);
    cmd_chk->set_on_click([this](UI::Widget& w) {
        this->gs.curr_nation->ai_do_cmd_troops = ((UI::Checkbox&)w).get_value();
    });
    cmd_chk->set_tooltip("Units will be managed by the AI");

    auto* close_btn = new UI::Button(0, 0, this->width, 24, this);
    close_btn->below_of(*cmd_chk);
    close_btn->text(Eng3D::Locale::translate("Close"));
    close_btn->set_on_click([this](UI::Widget& w) {
        this->gs.client->send(Action::AiControl::form_packet(this->gs.curr_nation));
        this->kill();
    });
}