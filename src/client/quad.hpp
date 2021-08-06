#ifndef QUAD_H
#define QUAD_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/vec3.hpp>

class Quad {
   public:
    Quad(float start_x, float start_y, float end_x, float end_y) {
        float vertices[] = {
            start_x, start_y, 0.0f, 0.0f, 0.0f,
            end_x, start_y, 0.0f, 1.0f, 0.0f,
            start_x, end_y, 0.0f, 0.0f, 1.0f,
            start_x, end_y, 0.0f, 0.0f, 1.0f,
            end_x, start_y, 0.0f, 1.0f, 0.0f,
            end_x, end_y, 0.0f, 1.0f, 1.0f};

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

   private:
    GLuint VBO, VAO;
};

#endif