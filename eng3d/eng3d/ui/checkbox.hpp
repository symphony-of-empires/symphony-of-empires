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
//      eng3d/ui/checkbox.hpp
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

#include "eng3d/ui/widget.hpp"

namespace UI {
    struct Div;
    /// @ingroup UI
    /// @brief Checkbox widget
    struct Checkbox : Widget {
        Checkbox(Widget* parent = nullptr);
        Checkbox(int size, Widget* parent = nullptr);
        Checkbox(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Checkbox() noexcept override {}
        bool get_value() const noexcept;
        void set_value(bool checked) noexcept;
        void set_on_click(std::function<void(Widget&)> on_click) noexcept override;
        void set_text(const std::string_view _text) noexcept override;
    private:
        static void on_click_default(Widget& w) noexcept;
        void init_checkbox(int size) noexcept;
        Div* box;
        bool value;
        std::function<void(Widget&)> outside_on_click = nullptr;
        std::shared_ptr<Eng3D::Texture> checked_texture;
        std::shared_ptr<Eng3D::Texture> unchecked_texture;
    };
}
