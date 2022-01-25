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
//      unified_render/ui/label.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "unified_render/ui/widget.hpp"

namespace UI {
	class Context;
    /**
     * @ingroup UI
     * @brief Simple widget for drawing text on the screen
     *
     */
    class Label: public Widget {
    public:
        Label(int x, int y, const std::string& text = " ", Widget* parent = nullptr);
        virtual ~Label() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        virtual void text(const std::string& _text);
    };
};
