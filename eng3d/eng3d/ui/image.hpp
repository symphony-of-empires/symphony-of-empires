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
//      eng3d/ui/image.hpp
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

namespace Eng3D {
	class Texture;
}

namespace UI {
	class Context;
    /// @ingroup UI
    /// @brief Image widget, can display pictures or effects on the screen
    class Image: public Widget {
    public:
        Image(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        Image(int x, int y, unsigned w, unsigned h, std::shared_ptr<Eng3D::Texture> tex, Widget* parent = nullptr);
        Image(int x, int y, unsigned w, unsigned h, const std::string_view tex_path, Widget* parent = nullptr);
        Image(int x, int y, unsigned w, unsigned h, const std::string_view tex_path, bool mipmap = true, Widget* parent = nullptr);
        static Image* make_transparent(int x, int y, unsigned w, unsigned h, const std::string_view tex_path, Widget* parent = nullptr);
        static Image* make_transparent(int x, int y, unsigned w, unsigned h, const std::string_view tex_path, bool mipmap = true, Widget* parent = nullptr);
        virtual ~Image() override {}
    };

    /// @ingroup UI
    /// @brief A version of the Image widget that keeps aspect ratio of the image (useful for flags)
    class AspectImage : public Image {
    public:
        AspectImage(int x, int y, unsigned w, unsigned max_h, std::shared_ptr<Eng3D::Texture> tex, Widget* parent = nullptr);
    };
}