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
//      curve.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "eng3d/primitive.hpp"

namespace Eng3D {
    namespace OpenGL {
        class Program;
    }

    struct Curve {
        Curve() = default;
        Curve(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, float width);
        ~Curve() = default;
        // Curve(glm::vec2 p1, glm::vec2 p2, float width);
        void add_line(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, float width);
        void upload();
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        void draw();
#else
#   error not implemented
#endif
    private:
        void create_line(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, float width);
        void add_quad(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4);
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> tex_coords;
        std::unique_ptr<TriangleList> quads;
    };
}
