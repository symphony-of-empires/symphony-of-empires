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
//      unified_render/ui/tooltip.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "unified_render/ui/widget.hpp"

namespace UI {
	class Label;

    /**
     * @ingroup UI
     * @brief Tooltip widget
     *
     */
    class Tooltip: public Widget {
    public:
        Tooltip();
        Tooltip(Widget* parent, unsigned w, unsigned h);
        virtual ~Tooltip();
        void set_pos(int x, int y, int width, int height, int screen_width, int screen_height);
        void text(const std::string& text);
    };
};