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
//      primitive.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include "unified_render/primitive.hpp"

#ifndef M_PI
#	define M_PI 3.1418f
#endif

UnifiedRender::Line::Line(float start_x, float start_y, float end_x, float end_y)
	: UnifiedRender::Mesh<glm::vec2, glm::vec2>(UnifiedRender::MeshMode::LINES)
{
	buffer.resize(2);
	buffer[0] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(start_x, start_y), glm::vec2(0.f, 0.f));
	buffer[1] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(end_x, end_y), glm::vec2(1.f, 1.f));

	upload();
}

UnifiedRender::Line::~Line(void) {

}

UnifiedRender::Square::Square(float start_x, float start_y, float end_x, float end_y)
	: UnifiedRender::Mesh<glm::vec2, glm::vec2>(UnifiedRender::MeshMode::TRIANGLES)
{
	buffer.resize(6);
	buffer[0] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(start_x, start_y), glm::vec2(0.f, 0.f));
	buffer[1] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
	buffer[2] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
	buffer[3] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
	buffer[4] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
	buffer[5] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(end_x, end_y), glm::vec2(1.f, 1.f));

	upload();
}

UnifiedRender::Square::~Square(void) {

}

UnifiedRender::TriangleList::TriangleList(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& tex_coords)
	: UnifiedRender::Mesh<glm::vec3, glm::vec2>(UnifiedRender::MeshMode::TRIANGLES)
{
	buffer.resize(positions.size());
	for(size_t i = 0; i < positions.size(); i++) {
		buffer[i] = UnifiedRender::MeshData<glm::vec3, glm::vec2>(positions[i], tex_coords[i]);
	}
	upload();
}

UnifiedRender::TriangleList::~TriangleList(void) {

}

UnifiedRender::Quad::Quad(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4)
	: UnifiedRender::Mesh<glm::vec3, glm::vec2>(UnifiedRender::MeshMode::TRIANGLES)
{
	buffer.resize(6);
	buffer[0] = UnifiedRender::MeshData<glm::vec3, glm::vec2>(c1, glm::vec2(0.f, 0.f));
	buffer[1] = UnifiedRender::MeshData<glm::vec3, glm::vec2>(c2, glm::vec2(0.f, 1.f));
	buffer[2] = UnifiedRender::MeshData<glm::vec3, glm::vec2>(c3, glm::vec2(1.f, 1.f));
	buffer[3] = UnifiedRender::MeshData<glm::vec3, glm::vec2>(c3, glm::vec2(1.f, 1.f));
	buffer[4] = UnifiedRender::MeshData<glm::vec3, glm::vec2>(c4, glm::vec2(1.f, 0.f));
	buffer[5] = UnifiedRender::MeshData<glm::vec3, glm::vec2>(c1, glm::vec2(0.f, 0.f));

	upload();
}

UnifiedRender::Quad::~Quad(void) {

}

UnifiedRender::Quad2D::Quad2D(void)
	: UnifiedRender::Mesh<glm::vec2, glm::vec2>(UnifiedRender::MeshMode::TRIANGLES)
{
	buffer.resize(6);
	buffer[0] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(-1.f, -1.f), glm::vec2(0.f, 0.f));
	buffer[1] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f));
	buffer[2] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f));
	buffer[3] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f));
	buffer[4] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f));
	buffer[5] = UnifiedRender::MeshData<glm::vec2, glm::vec2>(glm::vec2(1.f, 1.f), glm::vec2(1.f, 1.f));

	upload();
}

UnifiedRender::Quad2D::~Quad2D(void) {

}

UnifiedRender::Sphere::Sphere(float center_x, float center_y, float center_z, float _radius, int _resolution, bool cw_winding)
	: UnifiedRender::Mesh<glm::vec3, glm::vec2>(UnifiedRender::MeshMode::TRIANGLES),
	resolution{ _resolution },
	radius{ _radius }
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

UnifiedRender::Sphere::~Sphere(void) {

}

UnifiedRender::MeshData<glm::vec3, glm::vec2> UnifiedRender::Sphere::calc_pos(glm::vec3 center_pos, float longitude, float latitude) {
	float longitude_ratio = ((float)longitude) / resolution;
	float longitude_rad = longitude_ratio * 2 * M_PI;
	float latitude_ratio = ((float)latitude) / resolution;
	float latitude_rad = latitude_ratio * M_PI;

	float x = radius * std::cos(longitude_rad) * std::sin(latitude_rad);
	float y = radius * std::sin(longitude_rad) * std::sin(latitude_rad);
	float z = radius * std::cos(latitude_rad);

	glm::vec3 pos(x, y, z);
	pos += center_pos;
	glm::vec2 tex_coord(longitude_ratio, latitude_ratio);
	return UnifiedRender::MeshData<glm::vec3, glm::vec2>(pos, tex_coord);
}
