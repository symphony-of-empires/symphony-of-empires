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
//      client/interface/event.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/serializer.hpp"
#include "eng3d/ui/text.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/close_button.hpp"
#include "eng3d/network.hpp"

#include "client/interface/editor.hpp"
#include "client/client_network.hpp"
#include "event.hpp"
#include "action.hpp"
#include "io_impl.hpp"
#include "client/game_state.hpp"

using namespace Interface;

EventEditorNode::EventEditorNode(GameState& _gs, EventEditor::BaseNode* node, UI::Widget* _parent)
    : UI::Group(0, 0, _parent->width, _parent->height),
    gs{ _gs }
{
    this->is_scroll = false;
}

EventEditorWindow::EventEditorWindow(GameState& _gs, Event* _event)
    : UI::Window(128, 128, 320, 570),
    gs{ _gs },
    event{ _event }
{
    this->text("Editing event " + this->event->title);
}
