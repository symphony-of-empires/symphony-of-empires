// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      framebuffer.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#elif defined E3D_BACKEND_RGX
#   include <gccore.h>
#endif

#include "eng3d/texture.hpp"

namespace Eng3D {
#ifdef E3D_BACKEND_OPENGL
    namespace OpenGL {
        class Framebuffer {
        public:
            Framebuffer();
            ~Framebuffer();
            void set_texture(int index, const Eng3D::Texture& texture);
            void use();
        private:
            GLuint id = 0;
        };
    }
    using Framebuffer = OpenGL::Framebuffer;
#elif defined E3D_BACKEND_RGX
    namespace RGX {
        class Framebuffer {
        public:
            Framebuffer();
            ~Framebuffer();
            void set_texture(int index, const Eng3D::Texture& texture);
            void use();
        };
    }
    using Framebuffer = RGX::Framebuffer;
#endif
}