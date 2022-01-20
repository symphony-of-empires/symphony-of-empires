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
//      texture.hpp
//
// Abstract:
//      This file implements a texture which is based from the binary image
//      class to avoid code repetition. The difference of the texture from the
//      binary image is that the texture is oriented towards OpenGL rendering
//      more than the aforementioned binary image. A binary image is usable on
//      any context but rendering, while the texture is intended to be used in
//      rendering cases only.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "unified_render/binary_image.hpp"

struct SDL_Surface;
namespace UnifiedRender::IO {
    namespace Asset {
        class Base;
    };
};

namespace UnifiedRender {
    class TextureException: public BinaryImageException {
    public:
        TextureException(const std::string& filename, const std::string& message)
            : BinaryImageException(filename, message)
        {

        };
    };

    class TextureOptions {
    public:
        GLenum target = GL_TEXTURE_2D;
        GLuint wrap_s = GL_REPEAT;
        GLuint wrap_t = GL_REPEAT;
        GLuint min_filter = GL_NEAREST;
        GLuint mag_filter = GL_NEAREST;
        GLuint internal_format = GL_RGBA;
        GLuint format = GL_RGBA;
        GLuint type = GL_UNSIGNED_BYTE;
    };
    const TextureOptions default_options;

    class Texture : public BinaryImage {
    public:
        Texture(void);
        Texture(const std::string& path);
        Texture(const UnifiedRender::IO::Asset::Base* asset);
        Texture(size_t _width, size_t _height);
        ~Texture(void) override;
        void create_dummy();
        void to_opengl(TextureOptions options = default_options);
        void gen_mipmaps() const;
        void to_opengl(SDL_Surface* surface);
        void bind(void) const;
        void delete_opengl();
        void guillotine(const UnifiedRender::Texture& map, int x, int y, int w, int h);

        GLuint gl_tex_num = 0;
    };

    // Array of textures
    class TextureArray : public BinaryImage {
    public:
        TextureArray(const std::string& path, size_t _tiles_x, size_t _tiles_y);
        void to_opengl(GLuint wrapp = GL_REPEAT, GLuint min_filter = GL_NEAREST, GLuint mag_filter = GL_NEAREST);

        GLuint gl_tex_num = 0;
        size_t layers;
        size_t tiles_x, tiles_y;
    };

    // This texture manager helps to cache textures instead of loading them of the disk each time they are used
    // and also acts as a "texture loader"
    class TextureManager {
    private:
        std::map<std::string, UnifiedRender::Texture*> textures;
    public:
        const Texture& load(const std::string& path, TextureOptions options = default_options);
        const Texture& load(const UnifiedRender::IO::Asset::Base* asset, TextureOptions options = default_options);
    };
};
