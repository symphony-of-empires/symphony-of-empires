#include "unified_render/texture_array.hpp"

#include "unified_render/print.hpp"

UnifiedRender::TextureArray::TextureArray(const std::string& path, size_t _tiles_x, size_t _tiles_y)
    : BinaryImage(path), tiles_x(_tiles_x), tiles_y(_tiles_y) {}

void UnifiedRender::TextureArray::to_opengl(GLuint wrapp, GLuint min_filter, GLuint mag_filter) {
    glGenTextures(1, &gl_tex_num);
    glBindTexture(GL_TEXTURE_2D_ARRAY, gl_tex_num);

    // set up texture handle parameters
    // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);  // !single image!
    // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 1);   // !single image! mat->mips == 1
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    size_t p_dx = width / tiles_x;   // pixels of each tile in x
    size_t p_dy = height / tiles_y;  // pixels of each tile in y
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, p_dx, p_dy, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, height);

    for(size_t x = 0; x < tiles_x; x++) {
        for(size_t y = 0; y < tiles_y; y++) {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, x * tiles_x + y, p_dx, p_dy, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer + (x * p_dy * width + y * p_dx));
        }
    }
    //glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    // unbind texture handle
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
}
