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
//      eng3d/ui/label.hpp
//
// Abstract:
//      Label is a simple widget that draws text on the screen - it does not
//      support multiline paragraphs, use Text for that instead.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "eng3d/ui/widget.hpp"

namespace UI {
	class Context;
    /// @ingroup UI
    /// @brief Simple widget for drawing text on the screen, no multiline support
    class Label: public Widget {
    public:
        Label(int x, int y, const std::string_view text = " ", Widget* parent = nullptr);
        Label(int x, int y, const std::u32string& text = U" ", Widget* parent = nullptr);
        virtual ~Label();
        virtual void set_text(const std::string_view _text);
        virtual void set_text(const std::u32string& _text);
    };
}
