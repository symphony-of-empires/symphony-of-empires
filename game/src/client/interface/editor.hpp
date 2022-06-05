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
    /**
     * @brief Base node all nodes inherit from
     * 
     */
    class BaseNode {
    public:
        BaseNode() {};
        virtual ~BaseNode() {};
    };

    /**
     * @brief Conditional negation
     * 
     */
    class NodeCondNot : public BaseNode {
    public:
        NodeCondNot() {};
        ~NodeCondNot() {};
        
        std::unique_ptr<BaseNode> lhs;
    };

    /**
     * @brief Conditional "IS EQUAL"
     * 
     */
    class NodeCondIsEqual : public BaseNode {
    public:
        NodeCondIsEqual() {};
        ~NodeCondIsEqual() {};

        std::unique_ptr<BaseNode> lhs;
        std::unique_ptr<BaseNode> rhs;
    };

    /**
     * @brief Basic WHILE loop construct
     * 
     */
    class NodeWhileLoop : public BaseNode {
    public:
        NodeWhileLoop() {};
        ~NodeWhileLoop() {};

        std::unique_ptr<BaseNode> condition;
        std::vector<std::unique_ptr<BaseNode>> body;
    };

    /**
     * @brief An IF expression
     * 
     */
    class NodeIfExpr : public BaseNode {
    public:
        NodeIfExpr() {};
        ~NodeIfExpr() {};

        std::unique_ptr<BaseNode> condition;
        std::vector<std::unique_ptr<BaseNode>> body;
    };

    /**
     * @brief Declares a new variable
     * 
     */
    class NodeVariable : public BaseNode {
    public:
        NodeVariable() {};
        ~NodeVariable() {};

        std::string name;
    };

    /**
     * @brief References a variable
     * 
     */
    class NodeVarRef : public BaseNode {
    public:
        NodeVarRef() {};
        ~NodeVarRef() {};

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
