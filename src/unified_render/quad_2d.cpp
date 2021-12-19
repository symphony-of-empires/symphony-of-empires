#include "quad_2d.hpp"
using namespace UnifiedRender::OpenGl;

Quad2D::Quad2D()
	: PackedModel(GL_TRIANGLES)
{
	buffer.resize(6);
	buffer[0] = PackedData<glm::vec2, glm::vec2>(glm::vec2(-1.0f, -1.0f), glm::vec2(0.f, 0.f));
	buffer[1] = PackedData<glm::vec2, glm::vec2>(glm::vec2(1.0f, -1.0f), glm::vec2(1.f, 0.f));
	buffer[2] = PackedData<glm::vec2, glm::vec2>(glm::vec2(-1.0f, 1.0f), glm::vec2(0.f, 1.f));
	buffer[3] = PackedData<glm::vec2, glm::vec2>(glm::vec2(-1.0f, 1.0f), glm::vec2(0.f, 1.f));
	buffer[4] = PackedData<glm::vec2, glm::vec2>(glm::vec2(1.0f, -1.0f), glm::vec2(1.f, 0.f));
	buffer[5] = PackedData<glm::vec2, glm::vec2>(glm::vec2(1.0f, 1.0f), glm::vec2(1.f, 1.f));

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