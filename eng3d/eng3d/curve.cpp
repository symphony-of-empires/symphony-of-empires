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
//      curve.cpp
//
// Abstract:
//      Generalized "IO" functions for more abstract access to the OS resources
//      along with a custom "std::filesystem" implementation to aid and
//      implement custom paths.
// ----------------------------------------------------------------------------

#include "eng3d/curve.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/primitive.hpp"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

Eng3D::Curve::Curve(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, float width)
{
    this->create_line(points, normals, width);
}

void Eng3D::Curve::add_line(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, float width)
{
    this->create_line(points, normals, width);
}

void Eng3D::Curve::upload()
{
    this->quads = std::make_unique<Eng3D::TriangleList>(positions, tex_coords);
}

void Eng3D::Curve::add_quad(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4) {
    positions.reserve(positions.size() + 6);
    tex_coords.reserve(tex_coords.size() + 6);
    positions.push_back(c1);
    tex_coords.push_back(glm::vec2(0.f, 0.f));
    positions.push_back(c2);
    tex_coords.push_back(glm::vec2(0.f, 1.f));
    positions.push_back(c3);
    tex_coords.push_back(glm::vec2(1.f, 1.f));
    positions.push_back(c3);
    tex_coords.push_back(glm::vec2(1.f, 1.f));
    positions.push_back(c4);
    tex_coords.push_back(glm::vec2(1.f, 0.f));
    positions.push_back(c1);
    tex_coords.push_back(glm::vec2(0.f, 0.f));
}

void Eng3D::Curve::create_line(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, float width) {
    glm::vec3 prev_c1, prev_c2, prev_c3, prev_c4;
    for(size_t i = 0; i < points.size() - 1; i++) {
        auto p1 = points[i];
        auto p2 = points[i + 1];
        auto normal = normals[i];

        // Vec from p1 to p2
        glm::vec3 p1t2 = p2 - p1;
        // Orthogonal vector to the line from p1 to p2
        glm::vec3 ortho = glm::cross(p1t2, normal);//glm::vec2(p1t2.y, -p1t2.x);
        // ortho = ortho.y > 0 ? ortho : -ortho; // Needed for window order of triangle 
        ortho = glm::normalize(ortho);
        // Creates the line corners
        glm::vec3 c1 = p1 - ortho * width * 0.5f;
        glm::vec3 c2 = p1 + ortho * width * 0.5f;
        if(i != 0) {
            c1 = glm::mix(c1, prev_c3, .5f);
            c2 = glm::mix(c2, prev_c4, .5f);
            prev_c3 = c1;
            prev_c4 = c2;
            this->add_quad(prev_c1, prev_c2, prev_c4, prev_c3);
        }
        glm::vec3 c3 = p2 - ortho * width * 0.5f;
        glm::vec3 c4 = p2 + ortho * width * 0.5f;
        prev_c1 = c1;
        prev_c2 = c2;
        prev_c3 = c3;
        prev_c4 = c4;
    }
    this->add_quad(prev_c1, prev_c2, prev_c4, prev_c3);
}

#if defined(E3D_BACKEND_OPENGL) || defined(E3D_BACKEND_GLES)
void Eng3D::Curve::draw() {
    if(this->quads.get() == nullptr)
        this->upload();
    this->quads->draw();
}
#elif defined E3D_BACKEND_RGX
void Eng3D::Curve::draw() {

}
#endif
