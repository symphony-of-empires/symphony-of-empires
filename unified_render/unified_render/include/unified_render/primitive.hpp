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
//      primitive.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "unified_render/mesh.hpp"

namespace UnifiedRender {
    class Line: public UnifiedRender::Mesh<glm::vec2, glm::vec2> {
    public:
        Line(float start_x, float start_y, float end_x, float end_y);
        ~Line();
    };

    class Square: public UnifiedRender::Mesh<glm::vec2, glm::vec2> {
    public:
        Square(float start_x, float start_y, float end_x, float end_y);
        ~Square();
    };

    class TriangleList: public UnifiedRender::Mesh<glm::vec3, glm::vec2> {
    public:
        TriangleList(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& tex_coords);
        ~TriangleList();
    };
    
    class Quad: public UnifiedRender::Mesh<glm::vec3, glm::vec2> {
    public:
        Quad(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4);
        ~Quad();
    };

    class Quad2D: public UnifiedRender::Mesh<glm::vec2, glm::vec2> {
    public:
        Quad2D();
        ~Quad2D();
    };

    class Sphere: public UnifiedRender::Mesh<glm::vec3, glm::vec2> {
        UnifiedRender::MeshData<glm::vec3, glm::vec2> calc_pos(glm::vec3 center_pos, float longitude, float latitude);
        int resolution;
    public:
        float radius;
        Sphere(float x, float y, float z, float radius, int resolution, bool cw_winding = true);
        ~Sphere();
    };
}