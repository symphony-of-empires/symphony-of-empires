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
//      primitive.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include "eng3d/mesh.hpp"
#include "eng3d/rectangle.hpp"

namespace Eng3D {
    struct Line : Eng3D::MeshStatic<2, 0, glm::vec2, glm::vec2> {
        Line(glm::vec2 start, glm::vec2 end)
            : Eng3D::MeshStatic<2, 0, glm::vec2, glm::vec2>(Eng3D::MeshMode::LINES)
        {
            this->buffer[0] = Eng3D::MeshData(glm::vec2(start.x, start.y), glm::vec2(0.f, 0.f));
            this->buffer[1] = Eng3D::MeshData(glm::vec2(end.x, end.y), glm::vec2(1.f, 1.f));
            this->upload();
        }
        ~Line() = default;
    };

    struct Quad : Eng3D::MeshStatic<6, 0, glm::vec3, glm::vec2> {
        Quad(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4)
            : Eng3D::MeshStatic<6, 0, glm::vec3, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
        {
            this->buffer[0] = Eng3D::MeshData(c1, glm::vec2(0.f, 0.f));
            this->buffer[1] = Eng3D::MeshData(c2, glm::vec2(0.f, 1.f));
            this->buffer[2] = Eng3D::MeshData(c3, glm::vec2(1.f, 1.f));
            this->buffer[3] = Eng3D::MeshData(c3, glm::vec2(1.f, 1.f));
            this->buffer[4] = Eng3D::MeshData(c4, glm::vec2(1.f, 0.f));
            this->buffer[5] = Eng3D::MeshData(c1, glm::vec2(0.f, 0.f));
            this->upload();
        }
        ~Quad() = default;
    };

    struct Quad2D : Eng3D::MeshStatic<6, 0, glm::vec2, glm::vec2> {
        Quad2D()
            : Eng3D::MeshStatic<6, 0, glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
        {
            this->buffer[0] = Eng3D::MeshData(glm::vec2(-1.f, -1.f), glm::vec2(0.f, 0.f));
            this->buffer[1] = Eng3D::MeshData(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f));
            this->buffer[2] = Eng3D::MeshData(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f));
            this->buffer[3] = Eng3D::MeshData(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f));
            this->buffer[4] = Eng3D::MeshData(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f));
            this->buffer[5] = Eng3D::MeshData(glm::vec2(1.f, 1.f), glm::vec2(1.f, 1.f));
            this->upload();
        }
        ~Quad2D() = default;
    };

    struct Square : Eng3D::MeshStatic<6, 0, glm::vec2, glm::vec2> {
        Square(float start_x, float start_y, float end_x, float end_y)
            : Eng3D::MeshStatic<6, 0, glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
        {
            this->buffer[0] = Eng3D::MeshData(glm::vec2(start_x, start_y), glm::vec2(0.f, 0.f));
            this->buffer[1] = Eng3D::MeshData(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
            this->buffer[2] = Eng3D::MeshData(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
            this->buffer[3] = Eng3D::MeshData(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
            this->buffer[4] = Eng3D::MeshData(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
            this->buffer[5] = Eng3D::MeshData(glm::vec2(end_x, end_y), glm::vec2(1.f, 1.f));
            this->upload();
        }
        Square(const Eng3D::Rectangle& pos, const Eng3D::Rectangle& texcoord)
            : Eng3D::MeshStatic<6, 0, glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
        {
            this->buffer[0] = Eng3D::MeshData(glm::vec2(pos.left, pos.top), glm::vec2(texcoord.left, texcoord.top));
            this->buffer[1] = Eng3D::MeshData(glm::vec2(pos.right, pos.top), glm::vec2(texcoord.right, texcoord.top));
            this->buffer[2] = Eng3D::MeshData(glm::vec2(pos.left, pos.bottom), glm::vec2(texcoord.left, texcoord.bottom));
            this->buffer[3] = Eng3D::MeshData(glm::vec2(pos.left, pos.bottom), glm::vec2(texcoord.left, texcoord.bottom));
            this->buffer[4] = Eng3D::MeshData(glm::vec2(pos.right, pos.top), glm::vec2(texcoord.right, texcoord.top));
            this->buffer[5] = Eng3D::MeshData(glm::vec2(pos.right, pos.bottom), glm::vec2(texcoord.right, texcoord.bottom));
            this->upload();
        }
        ~Square() = default;
    };

    struct TriangleList : Eng3D::Mesh<glm::vec3, glm::vec2> {
        TriangleList(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& tex_coords)
            : Eng3D::Mesh<glm::vec3, glm::vec2>(Eng3D::MeshMode::TRIANGLES)
        {
            this->buffer.resize(positions.size());
            for(size_t i = 0; i < positions.size(); i++)
                this->buffer[i] = Eng3D::MeshData<glm::vec3, glm::vec2>(positions[i], tex_coords[i]);
            this->upload();
        }
        ~TriangleList() = default;
    };

    class Sphere : public Eng3D::Mesh<glm::vec3, glm::vec2> {
        Eng3D::MeshData<glm::vec3, glm::vec2> calc_pos(glm::vec3 center_pos, float longitude, float latitude) {
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
        int resolution;
    public:
        Sphere(float center_x, float center_y, float center_z, float _radius, int _resolution, bool cw_winding = true)
            : Eng3D::Mesh<glm::vec3, glm::vec2>(Eng3D::MeshMode::TRIANGLES),
            resolution{ _resolution },
            radius{ _radius }
        {
            this->buffer.resize(6 * resolution * resolution);
            glm::vec3 center_pos(center_x, center_y, center_z);
            // Switch the order of the vertices if not clockwise winding
            for(int latitude = 0; latitude < resolution; latitude++) {
                for(int longitude = 0; longitude < resolution; longitude++) {
                    this->buffer[(longitude + latitude * resolution) * 6 + 0] = calc_pos(center_pos, longitude + 0, latitude + 0);
                    this->buffer[(longitude + latitude * resolution) * 6 + 1] = calc_pos(center_pos, longitude + cw_winding, latitude + !cw_winding);
                    this->buffer[(longitude + latitude * resolution) * 6 + 2] = calc_pos(center_pos, longitude + !cw_winding, latitude + cw_winding);
                    this->buffer[(longitude + latitude * resolution) * 6 + 3] = calc_pos(center_pos, longitude + !cw_winding, latitude + cw_winding);
                    this->buffer[(longitude + latitude * resolution) * 6 + 4] = calc_pos(center_pos, longitude + cw_winding, latitude + !cw_winding);
                    this->buffer[(longitude + latitude * resolution) * 6 + 5] = calc_pos(center_pos, longitude + 1, latitude + 1);
                }
            }
            this->upload();
        }
        ~Sphere() = default;
        float radius;
    };
}
