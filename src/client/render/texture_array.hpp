#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

#include "../../binary_image.hpp"

namespace UnifiedRender {
    class TextureArray : public BinaryImage {
    public:
        TextureArray(const std::string& path, size_t _tiles_x, size_t _tiles_y);

        GLuint gl_tex_num;
        void to_opengl(GLuint wrapp = GL_REPEAT, GLuint min_filter = GL_NEAREST, GLuint mag_filter = GL_NEAREST);

        size_t layers;
        size_t tiles_x;
        size_t tiles_y;
    };
};