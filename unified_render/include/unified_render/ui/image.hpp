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
//      unified_render/ui/image.hpp
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

namespace UnifiedRender {
	class Texture;
}

namespace UI {
	class Context;
    /**
     * @ingroup UI
     * @brief Image widget
     *
     */
    class Image: public Widget {
    public:
        Image(int x, int y, unsigned w, unsigned h, const UnifiedRender::Texture* tex, Widget* parent = nullptr);
        Image(int x, int y, unsigned w, unsigned h, const std::string& tex_path, Widget* parent = nullptr);
        static Image* make_transparent(int x, int y, unsigned w, unsigned h, const std::string& tex_path, Widget* parent = nullptr);
        virtual ~Image() override {};
    };
};