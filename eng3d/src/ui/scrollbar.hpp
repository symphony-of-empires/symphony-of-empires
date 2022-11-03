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
//      eng3d/ui/scrollbar.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/widget.hpp"

namespace UI {
    class Scrollbar;

    /// @ingroup UI
    /// @brief The thumb of the scrollbar
    class ScrollbarThumb : public UI::Widget {
        float start_drag_scroll;
    public:
        ScrollbarThumb(int x, int y, UI::Widget* parent = nullptr);
        virtual ~ScrollbarThumb() override {};
    };

    /// @ingroup UI
    /// @brief Scrollbar widget
    class Scrollbar : public UI::Widget {
        UI::ScrollbarThumb* thumb_btn;
    public:
        Scrollbar(int x, int y, unsigned h, UI::Widget* parent = nullptr);
        virtual ~Scrollbar() override {};
        void update_thumb();
    };
};