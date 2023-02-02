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
//      eng3d/ui/tooltip.hpp
//
// Abstract:
//      Tooltip widget emulates a Window (but it's not actually one!) for
//      displaying tooltips on the screen.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "eng3d/ui/widget.hpp"

namespace UI {
    /// @ingroup UI
    /// @brief Tooltip widget, used entirely for hovering purpouses, don't use any other
    /// widget for hovering unless it inherits from Tooltip.
    class Tooltip: public UI::Widget {
        void init(UI::Widget* parent, size_t w, size_t h);
    public:
        Tooltip(UI::Widget* parant = nullptr);
        Tooltip(UI::Widget* parent, const std::string_view text);
        Tooltip(UI::Widget* parent, unsigned w, unsigned h);
        virtual ~Tooltip() {}
        void set_pos(int x, int y, int width, int height, int screen_width, int screen_height);
        void set_text(const std::string_view text);
    };
}
