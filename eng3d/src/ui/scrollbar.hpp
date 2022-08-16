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
    public:
        ScrollbarThumb(int x, int y, UI::Scrollbar* parent = nullptr);
        virtual ~ScrollbarThumb() override {};
    };

    /// @ingroup UI
    /// @brief Scrollbar widget
    class Scrollbar : public UI::Widget {
        UI::ScrollbarThumb* thumb_btn;
    public:
        Scrollbar(int x, int y, unsigned w, unsigned h, UI::Widget* parent = nullptr);
        virtual ~Scrollbar() override {};
        void update_thumb();
    };
};