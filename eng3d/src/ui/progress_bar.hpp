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
//      eng3d/ui/progress_bar.hpp
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
    class Context;
    /// @ingroup UI
    /// @brief Progressbar widget
    enum class Direction {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        TOP_TO_BOTTOM,
        BOTTOM_TO_TOP
    };

    class ProgressBar: public Widget {
        float value;
    public:
        ProgressBar(int x, int y, unsigned w, unsigned h, float min, float max, Widget* _parent = nullptr);
        virtual ~ProgressBar() override {};
        virtual void on_render(Context& ctx, Eng3D::Rect viewport);
        inline void set_value(const float _value) {
            value = std::clamp(_value, min, max);
        }

        inline float get_value() const {
            return value;
        }

        float max, min;
        Direction direction = Direction::LEFT_TO_RIGHT;
    };
};