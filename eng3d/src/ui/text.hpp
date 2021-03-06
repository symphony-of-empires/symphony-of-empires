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
//      eng3d/ui/text.cpp
//
// Abstract:
//      An extended version of Label that supports multiline text.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "eng3d/ui/widget.hpp"

namespace UI {
	class Context;
	class Label;

    /// @todo Markdown support?
    /// @ingroup UI
    /// @brief Multiline textbox that allows more descriptive paragraphs
    /// than the Label widget.
    class Text: public Widget {
    public:
        Text(int x, int y, unsigned w, unsigned h, Widget* parent);
        virtual ~Text() {};
        virtual void on_render(Context& ctx, Eng3D::Rect viewport);
        virtual void text(const std::string& text);

        int min_height = 0;
    };
};