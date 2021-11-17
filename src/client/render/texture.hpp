#pragma once

/**
 * This file implements a texture which is based from the binary image class to avoid
 * code repetition.
 *
 * The difference of the texture from the binary image is that the texture is oriented towards
 * OpenGL rendering more than the aforementioned binary image.
 *
 * A binary image is usable on any context but rendering, while the texture is
 * intended to be used in rendering cases only
 */

#include <GL/glew.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <set>
#ifdef windows
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
#endif
#include <GL/gl.h>

struct SDL_Surface;

#include "binary_image.hpp"

namespace UnifiedRender {
    class TextureException : public BinaryImageException {
    public:
        TextureException(const std::string& filename, const std::string& message) : BinaryImageException(filename, message){};
    };

    class Texture : public BinaryImage {
    public:
        Texture() {};
        ~Texture() override;
        Texture(const std::string& path) : BinaryImage(path) {};
        Texture(size_t _width, size_t _height) : BinaryImage(_width, _height) {};

        GLuint gl_tex_num = 0;
        void create_dummy();
        void to_opengl(GLuint wrapp = GL_REPEAT, GLuint min_filter = GL_NEAREST, GLuint mag_filter = GL_NEAREST);
        void to_opengl_test(GLuint wrapp = GL_REPEAT, GLuint min_filter = GL_NEAREST, GLuint mag_filter = GL_NEAREST);
        void gen_mipmaps() const;
        void to_opengl(SDL_Surface* surface);
        void bind(void) const;
        void delete_opengl();
        void guillotine(const UnifiedRender::Texture& map, int x, int y, int w, int h);
    };

    /**
     * This texture manager helps to cache textures instead of loading them of the disk each time they are used
     * and also acts as a "texture loader"
     */
    class TextureManager {
    private:
        std::set<std::pair<UnifiedRender::Texture*, std::string>> textures;
    public:
        const Texture& load_texture(const std::string& path, GLuint wrapp = GL_REPEAT, GLuint min_filter = GL_NEAREST, GLuint mag_filter = GL_NEAREST);
    };
};

extern UnifiedRender::TextureManager* g_texture_manager;