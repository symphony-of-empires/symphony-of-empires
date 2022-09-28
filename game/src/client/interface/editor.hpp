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
//      client/interface/event.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/string.hpp"
#include "eng3d/ui/window.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/group.hpp"

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"

namespace EventEditor {
    /// @brief Base node all nodes inherit from
    struct BaseNode {
    };

    /// @brief Conditional negation
    struct NodeCondNot : BaseNode {
        std::unique_ptr<BaseNode> lhs;
    };

    /// @brief Conditional "IS EQUAL"
    struct NodeCondIsEqual : BaseNode {
        std::unique_ptr<BaseNode> lhs;
        std::unique_ptr<BaseNode> rhs;
    };

    /// @brief Basic WHILE loop construct
    struct NodeWhileLoop : BaseNode {
        std::unique_ptr<BaseNode> condition;
        std::vector<std::unique_ptr<BaseNode>> body;
    };

    /// @brief An IF expression
    struct NodeIfExpr : BaseNode {
        std::unique_ptr<BaseNode> condition;
        std::vector<std::unique_ptr<BaseNode>> body;
    };

    /// @brief Declares a new variable
    struct NodeVariable : BaseNode {
        std::string name;
    };

    /// @brief References a variable
    struct NodeVarRef : BaseNode {
        std::shared_ptr<NodeVariable> var;
    };
};

namespace Interface {
    class EventEditorNode: public UI::Group {
        GameState& gs;
        EventEditor::BaseNode* node;
    public:
        EventEditorNode(GameState& gs, EventEditor::BaseNode* node, UI::Widget* parent);
    };

    class EventEditorWindow: public UI::Window {
        GameState& gs;
        Event* event;
    public:
        EventEditorWindow(GameState& gs, Event* event);
    };
};
