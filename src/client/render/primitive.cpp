#include "primitive.hpp"
using namespace UnifiedRender::OpenGl;

PrimitiveSquare::PrimitiveSquare(float start_x, float start_y, float end_x, float end_y)
: PackedModel(GL_TRIANGLES) {
    buffer.resize(6);
    buffer[0] = std::make_pair(glm::vec2(start_x, start_y), glm::vec2(0.f, 0.f));
    buffer[1] = std::make_pair(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
    buffer[2] = std::make_pair(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
    buffer[3] = std::make_pair(glm::vec2(start_x, end_y), glm::vec2(0.f, 1.f));
    buffer[4] = std::make_pair(glm::vec2(end_x, start_y), glm::vec2(1.f, 0.f));
    buffer[5] = std::make_pair(glm::vec2(end_x, end_y), glm::vec2(1.f, 1.f));

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texcoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}