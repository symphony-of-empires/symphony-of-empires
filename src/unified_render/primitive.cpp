#include "unified_render/primitive.hpp"
using namespace UnifiedRender::OpenGl;

PrimitiveSquare::PrimitiveSquare(float start_x, float start_y, float end_x, float end_y)
    : PackedModel(GL_TRIANGLES)
{
    buffer.resize(6);
    buffer[0] = PackedData<glm::vec3, glm::vec2>(glm::vec3(start_x, start_y, 0.0f), glm::vec2(0.f, 0.f));
    buffer[1] = PackedData<glm::vec3, glm::vec2>(glm::vec3(end_x, start_y, 0.0f), glm::vec2(1.f, 0.f));
    buffer[2] = PackedData<glm::vec3, glm::vec2>(glm::vec3(start_x, end_y, 0.0f), glm::vec2(0.f, 1.f));
    buffer[3] = PackedData<glm::vec3, glm::vec2>(glm::vec3(start_x, end_y, 0.0f), glm::vec2(0.f, 1.f));
    buffer[4] = PackedData<glm::vec3, glm::vec2>(glm::vec3(end_x, start_y, 0.0f), glm::vec2(1.f, 0.f));
    buffer[5] = PackedData<glm::vec3, glm::vec2>(glm::vec3(end_x, end_y, 0.0f), glm::vec2(1.f, 1.f));

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