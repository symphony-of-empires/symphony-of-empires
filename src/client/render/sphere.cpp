#include "sphere.hpp"
#include <math.h>

using namespace UnifiedRender::OpenGl;

Sphere::Sphere(float center_x, float center_y, float center_z, float _radius, int _resolution)
	: radius{ _radius }, resolution{ _resolution }, PackedModel(GL_TRIANGLES) {
	buffer.resize(6 * resolution * resolution);

	for(int latitude = 0; latitude < resolution; latitude++) {
		for(int longitude = 0; longitude < resolution; longitude++) {
			buffer[(longitude + latitude * resolution) * 6 + 0] = calc_pos(longitude + 0, latitude + 0);
			buffer[(longitude + latitude * resolution) * 6 + 1] = calc_pos(longitude + 1, latitude + 0);
			buffer[(longitude + latitude * resolution) * 6 + 2] = calc_pos(longitude + 0, latitude + 1);
			buffer[(longitude + latitude * resolution) * 6 + 3] = calc_pos(longitude + 0, latitude + 1);
			buffer[(longitude + latitude * resolution) * 6 + 4] = calc_pos(longitude + 1, latitude + 0);
			buffer[(longitude + latitude * resolution) * 6 + 5] = calc_pos(longitude + 1, latitude + 1);
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

PackedData<glm::vec3, glm::vec2> Sphere::calc_pos(float longitude, float latitude) {
	float longitude_ratio = ((float)longitude) / resolution;
	float longitude_rad = longitude_ratio * 2 * M_PI;
	float latitude_ratio = ((float)latitude) / resolution;
	float latitude_rad = latitude_ratio * M_PI;
	float x = radius * cos(longitude_rad) * sin(latitude_rad);
	float y = radius * sin(longitude_rad) * sin(latitude_rad);
	float z = radius * cos(latitude_rad);
	glm::vec3 pos(x, y, z);
	glm::vec2 tex_coord(longitude_ratio, latitude_ratio);
	return PackedData<glm::vec3, glm::vec2>(pos, tex_coord);
}