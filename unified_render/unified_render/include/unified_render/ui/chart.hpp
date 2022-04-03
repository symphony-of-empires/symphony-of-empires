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
//      unified_render/ui/chart.hpp
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
#include "unified_render/rectangle.hpp"

namespace UnifiedRender {
    class Rectangle;
    typedef Rectangle Rect;
}

namespace UI {
    class Context;
    /**
     * @ingroup UI
     * @brief A graph chart
     *
     */
    class Chart: public Widget {
    public:
        Chart(int x, int y, unsigned w, unsigned h, Widget* _parent = nullptr);
        virtual ~Chart() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        std::deque<double> data;
    };
};
