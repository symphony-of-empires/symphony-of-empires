#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include "unified_render/primitive.hpp"

using namespace UnifiedRender::OpenGL;

UnifiedRender::Square::Square(float start_x, float start_y, float end_x, float end_y)
    : PackedModel(GL_TRIANGLES)
{
    buffer.resize(6);
    buffer[0] = PackedData(glm::vec2(start_x, start_y), glm::vec2(0.f, 0.f));
    buffer[1] = PackedData(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
    buffer[2] = PackedData(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
    buffer[3] = PackedData(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
    buffer[4] = PackedData(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
    buffer[5] = PackedData(glm::vec2(end_x, end_y), glm::vec2(1.f, 1.f));

    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)0);
    glEnableVertexAttribArray(0);
    // Texcoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

UnifiedRender::Square::~Square(void) {

}

UnifiedRender::Quad2D::Quad2D(void)
	: PackedModel(GL_TRIANGLES)
{
	buffer.resize(6);
	buffer[0] = PackedData(glm::vec2(-1.f, -1.f), glm::vec2(0.f, 0.f));
	buffer[2] = PackedData(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f));
	buffer[1] = PackedData(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f));
	buffer[4] = PackedData(glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f));
	buffer[3] = PackedData(glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f));
	buffer[5] = PackedData(glm::vec2(1.f, 1.f), glm::vec2(1.f, 1.f));

	vao.bind();
	vbo.bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);

	// Vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)0);
	glEnableVertexAttribArray(0);
	// Texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

UnifiedRender::Quad2D::~Quad2D(void) {

}

UnifiedRender::Sphere::Sphere(float center_x, float center_y, float center_z, float _radius, int _resolution)
	: PackedModel(GL_TRIANGLES),
	  resolution{_resolution},
    radius{_radius}
{
	buffer.resize(6 * resolution * resolution);
	glm::vec3 center_pos(center_x, center_y, center_z);

	for(int latitude = 0; latitude < resolution; latitude++) {
		for(int longitude = 0; longitude < resolution; longitude++) {
			buffer[(longitude + latitude * resolution) * 6 + 0] = calc_pos(center_pos, longitude + 0, latitude + 0);
			buffer[(longitude + latitude * resolution) * 6 + 1] = calc_pos(center_pos, longitude + 1, latitude + 0);
			buffer[(longitude + latitude * resolution) * 6 + 2] = calc_pos(center_pos, longitude + 0, latitude + 1);
			buffer[(longitude + latitude * resolution) * 6 + 3] = calc_pos(center_pos, longitude + 0, latitude + 1);
			buffer[(longitude + latitude * resolution) * 6 + 4] = calc_pos(center_pos, longitude + 1, latitude + 0);
			buffer[(longitude + latitude * resolution) * 6 + 5] = calc_pos(center_pos, longitude + 1, latitude + 1);
		}
	}

	vao.bind();
	vbo.bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);

	// Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)0);
	glEnableVertexAttribArray(0);
	// Texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

UnifiedRender::Sphere::~Sphere(void) {

}

UnifiedRender::OpenGL::PackedData<glm::vec3, glm::vec2> Sphere::calc_pos(glm::vec3 center_pos, float longitude, float latitude) {
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
	return UnifiedRender::OpenGL::PackedData<glm::vec3, glm::vec2>(pos, tex_coord);
}