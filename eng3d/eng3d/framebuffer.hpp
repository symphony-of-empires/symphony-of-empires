// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      framebuffer.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/texture.hpp"

namespace Eng3D {
    namespace OpenGL {
        class Framebuffer {
            unsigned int id = 0;
        public:
            Framebuffer();
            ~Framebuffer();
            void set_texture(int index, const Eng3D::Texture& texture);
            void use();
        private:
        };
    }
    using Framebuffer = OpenGL::Framebuffer;
}
