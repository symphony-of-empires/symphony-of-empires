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
//      eng3d/ui/group.hpp
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
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/widget.hpp"

namespace Eng3D {
	class Rectangle;
    typedef Rectangle Rect;
}

namespace UI {
	class Context;
    /// @ingroup UI
    /// @brief Grouping to keep widgets together without triggering events
    ///
    /// Doesn't capture mouse on mouse events
    /// The width and height are meaningless for this widget
    /// Doesn't do any cropping of child widgets if they are outside it's bounds
    class Group: public Widget {
    public:
        Group(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        Group(int x, int y, Widget* parent = nullptr);
        virtual ~Group() override {};

        virtual void on_render(Context& ctx, Eng3D::Rect viewport) override;
    };
};
