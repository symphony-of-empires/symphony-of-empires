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
//      primitive.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include "eng3d/primitive.hpp"

Eng3D::Line::Line(float start_x, float start_y, float end_x, float end_y)
    : Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::LINES)
{
    buffer.resize(2);
    buffer[0] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(start_x, start_y), glm::vec2(0.f, 0.f));
    buffer[1] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(end_x, end_y), glm::vec2(1.f, 1.f));

    upload();
}

Eng3D::Square::Square(float start_x, float start_y, float end_x, float end_y)
    : Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
{
    buffer.resize(6);
    buffer[0] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(start_x, start_y), glm::vec2(0.f, 0.f));
    buffer[1] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
    buffer[2] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
    buffer[3] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
    buffer[4] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
    buffer[5] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(end_x, end_y), glm::vec2(1.f, 1.f));

    upload();
}

Eng3D::Square::Square(const Eng3D::Rect& pos, const Eng3D::Rect& texcoord)
    : Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
{
    buffer.resize(6);
    buffer[0] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(pos.left, pos.top), glm::vec2(texcoord.left, texcoord.top));
    buffer[1] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(pos.right, pos.top), glm::vec2(texcoord.right, texcoord.top));
    buffer[2] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(pos.left, pos.bottom), glm::vec2(texcoord.left, texcoord.bottom));
    buffer[3] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(pos.left, pos.bottom), glm::vec2(texcoord.left, texcoord.bottom));
    buffer[4] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(pos.right, pos.top), glm::vec2(texcoord.right, texcoord.top));
    buffer[5] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(pos.right, pos.bottom), glm::vec2(texcoord.right, texcoord.bottom));

    upload();
}

Eng3D::TriangleList::TriangleList(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& tex_coords)
    : Eng3D::Mesh<glm::vec3, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
{
    buffer.resize(positions.size());
    for(size_t i = 0; i < positions.size(); i++) {
        buffer[i] = Eng3D::MeshData<glm::vec3, glm::vec2>(positions[i], tex_coords[i]);
    }
    upload();
}

Eng3D::Quad::Quad(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4)
    : Eng3D::Mesh<glm::vec3, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
{
    buffer.resize(6);
    buffer[0] = Eng3D::MeshData<glm::vec3, glm::vec2>(c1, glm::vec2(0.f, 0.f));
    buffer[1] = Eng3D::MeshData<glm::vec3, glm::vec2>(c2, glm::vec2(0.f, 1.f));
    buffer[2] = Eng3D::MeshData<glm::vec3, glm::vec2>(c3, glm::vec2(1.f, 1.f));
    buffer[3] = Eng3D::MeshData<glm::vec3, glm::vec2>(c3, glm::vec2(1.f, 1.f));
    buffer[4] = Eng3D::MeshData<glm::vec3, glm::vec2>(c4, glm::vec2(1.f, 0.f));
    buffer[5] = Eng3D::MeshData<glm::vec3, glm::vec2>(c1, glm::vec2(0.f, 0.f));

    upload();
}

Eng3D::Quad2D::Quad2D()
    : Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
{
    buffer.resize(6);
    buffer[0] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(-1.f, -1.f), glm::vec2(0.f, 0.f));
    buffer[1] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f));
    buffer[2] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f));
    buffer[3] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f));
    buffer[4] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f));
    buffer[5] = Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(1.f, 1.f), glm::vec2(1.f, 1.f));

    upload();
}

Eng3D::Sphere::Sphere(float center_x, float center_y, float center_z, float _radius, int _resolution, bool cw_winding)
    : Eng3D::Mesh<glm::vec3, glm::vec2>(Eng3D::MeshMode::TRIANGLES),
    radius{ _radius },
    resolution{ _resolution }
{
    buffer.resize(6 * resolution * resolution);
    glm::vec3 center_pos(center_x, center_y, center_z);

    // Switch the order of the vertices if not clockwise winding
    for(int latitude = 0; latitude < resolution; latitude++) {
        for(int longitude = 0; longitude < resolution; longitude++) {
            buffer[(longitude + latitude * resolution) * 6 + 0] = calc_pos(center_pos, longitude + 0, latitude + 0);
            buffer[(longitude + latitude * resolution) * 6 + 1] = calc_pos(center_pos, longitude + cw_winding, latitude + !cw_winding);
            buffer[(longitude + latitude * resolution) * 6 + 2] = calc_pos(center_pos, longitude + !cw_winding, latitude + cw_winding);
            buffer[(longitude + latitude * resolution) * 6 + 3] = calc_pos(center_pos, longitude + !cw_winding, latitude + cw_winding);
            buffer[(longitude + latitude * resolution) * 6 + 4] = calc_pos(center_pos, longitude + cw_winding, latitude + !cw_winding);
            buffer[(longitude + latitude * resolution) * 6 + 5] = calc_pos(center_pos, longitude + 1, latitude + 1);
        }
    }

    upload();
}

Eng3D::MeshData<glm::vec3, glm::vec2> Eng3D::Sphere::calc_pos(glm::vec3 center_pos, float longitude, float latitude) {
    const float longitude_ratio = ((float)longitude) / resolution;
    const float longitude_rad = longitude_ratio * 2 * glm::pi<float>();
    const float latitude_ratio = ((float)latitude) / resolution;
    const float latitude_rad = latitude_ratio * glm::pi<float>();

    const float x = radius * glm::cos(longitude_rad) * glm::sin(latitude_rad);
    const float y = radius * glm::sin(longitude_rad) * glm::sin(latitude_rad);
    const float z = radius * glm::cos(latitude_rad);

    glm::vec3 pos{ x, y, z };
    pos += center_pos;
    glm::vec2 tex_coord(longitude_ratio, latitude_ratio);
    return Eng3D::MeshData<glm::vec3, glm::vec2>(pos, tex_coord);
}
