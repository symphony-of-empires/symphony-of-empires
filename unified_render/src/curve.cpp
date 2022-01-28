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
//      curve.cpp
//
// Abstract:
//      Generalized "IO" functions for more abstract access to the OS resources
//      along with a custom "std::filesystem" implementation to aid and
//      implement custom paths.
// ----------------------------------------------------------------------------

#include "unified_render/curve.hpp"
#include "unified_render/shader.hpp"
#include "unified_render/primitive.hpp"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

using namespace UnifiedRender;

Curve::Curve(std::vector<glm::vec2> _points, float _width)
    : width{ _width }
{
    points = _points;
    create_line();
};

Curve::Curve(glm::vec2 p1, glm::vec2 p2, float _width)
    : width{ _width }
{
    points.push_back(p1);
    points.push_back(p2);
    create_line();
};

void Curve::create_line() {
    glm::vec3 prev_c1;
    glm::vec3 prev_c2;
    glm::vec3 prev_c3;
    glm::vec3 prev_c4;
    for(int i = 0; i < points.size() - 1; i++) {
        glm::vec2 p1 = points[i];
        glm::vec2 p2 = points[i + 1];
        // Vec from p1 to p2
        glm::vec2 p1t2 = p2 - p1;
        // Orthogonal vector to the line from p1 to p2
        glm::vec2 ortho = glm::vec2(p1t2.y, -p1t2.x);
        // ortho = ortho.y > 0 ? ortho : -ortho; // Needed for window order of triangle 
        ortho = glm::normalize(ortho);
        // Creates the line corners
        glm::vec3 c1 = glm::vec3(p1 + ortho * width * 0.5f, -0.05);
        glm::vec3 c2 = glm::vec3(p1 - ortho * width * 0.5f, -0.05);
        if(i != 0) {
            c1 = glm::mix(c1, prev_c3, .5f);
            c2 = glm::mix(c2, prev_c4, .5f);
            prev_c3 = c1;
            prev_c4 = c2;
            UnifiedRender::Quad* quad = new UnifiedRender::Quad(prev_c1, prev_c2, prev_c4, prev_c3);
            quads.push_back(quad);
        }
        glm::vec3 c3 = glm::vec3(p2 + ortho * width * 0.5f, -0.05);
        glm::vec3 c4 = glm::vec3(p2 - ortho * width * 0.5f, -0.05);
        prev_c1 = c1;
        prev_c2 = c2;
        prev_c3 = c3;
        prev_c4 = c4;
    }
    UnifiedRender::Quad* quad = new UnifiedRender::Quad(prev_c1, prev_c2, prev_c4, prev_c3);
    quads.push_back(quad);
}

#ifdef UR_BACKEND_OPENGL
void Curve::draw() {
    for(auto quad : quads) {
        quad->draw();
    }
}
#endif