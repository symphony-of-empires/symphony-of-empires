// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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
//      curve.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace UnifiedRender {
    class TriangleList;
    namespace OpenGL {
        class Program;
    }

    class Curve {
    public:
        Curve() {};
        Curve(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, float width);
        // Curve(glm::vec2 p1, glm::vec2 p2, float width);

        void add_line(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, float width);
        void upload();

#ifdef UR_BACKEND_OPENGL
        void draw();
#endif
    private:
        void create_line(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, float width);
        void add_quad(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4);
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> tex_coords;
        TriangleList* quads = nullptr;
    };
};