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
//      primitive.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "eng3d/mesh.hpp"
#include "eng3d/rectangle.hpp"

namespace Eng3D {
    class Line: public Eng3D::Mesh<glm::vec2, glm::vec2> {
    public:
        Line(float start_x, float start_y, float end_x, float end_y);
        ~Line() = default;
    };

    class Square: public Eng3D::Mesh<glm::vec2, glm::vec2> {
    public:
        Square(float start_x, float start_y, float end_x, float end_y);
        Square(const Eng3D::Rect& pos, const Eng3D::Rect& texcoord);
        ~Square() = default;
    };

    class TriangleList: public Eng3D::Mesh<glm::vec3, glm::vec2> {
    public:
        TriangleList(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& tex_coords);
        ~TriangleList() = default;
    };
    
    class Quad: public Eng3D::Mesh<glm::vec3, glm::vec2> {
    public:
        Quad(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4);
        ~Quad() = default;
    };

    class Quad2D: public Eng3D::Mesh<glm::vec2, glm::vec2> {
    public:
        Quad2D();
        ~Quad2D() = default;
    };

    class Sphere: public Eng3D::Mesh<glm::vec3, glm::vec2> {
    public:
        Sphere(float x, float y, float z, float radius, int resolution, bool cw_winding = true);
        ~Sphere() = default;
        float radius;
    private:
        Eng3D::MeshData<glm::vec3, glm::vec2> calc_pos(glm::vec3 center_pos, float longitude, float latitude);
        int resolution;
    };
}