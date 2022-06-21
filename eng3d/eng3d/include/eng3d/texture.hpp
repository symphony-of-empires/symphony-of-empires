// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
#include <unordered_map>
#include <functional>
#include <memory>

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl31.h>
#endif

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_surface.h>

#include "eng3d/binary_image.hpp"
#include "eng3d/color.hpp"

namespace Eng3D::IO {
    namespace Asset {
        class Base;
    };
};

namespace Eng3D {
    class State;

    class TextureException: public BinaryImageException {
    public:
        TextureException(const std::string& filename, const std::string& message)
            : BinaryImageException(filename, message)
        {

        }
    };

    class TextureOptions {
    public:
        TextureOptions() {};
#ifdef E3D_BACKEND_OPENGL
        GLenum target = GL_TEXTURE_2D;
        GLuint wrap_s = GL_REPEAT;
        GLuint wrap_t = GL_REPEAT;
        GLuint min_filter = GL_NEAREST;
        GLuint mag_filter = GL_NEAREST;
        GLuint internal_format = GL_RGBA;
        GLuint format = GL_RGBA;
        GLuint type = GL_UNSIGNED_BYTE;
#else
        GLenum target;
        GLuint wrap_s;
        GLuint wrap_t;
        GLuint min_filter;
        GLuint mag_filter;
        GLuint internal_format;
        GLuint format;
        GLuint type;
#endif
        bool editable = false;
        bool compressed = true;

        constexpr bool operator==(const TextureOptions& o) const {
            return target == o.target && wrap_s == o.wrap_s && wrap_t == o.wrap_t && min_filter == o.min_filter && mag_filter == o.mag_filter && internal_format == o.internal_format && format == o.format && type == o.type && editable == o.editable;
        }
    };
    const TextureOptions default_options;

    class Texture: public BinaryImage {
    public:
        Texture();
        Texture(const std::string& path);
        Texture(const Eng3D::IO::Asset::Base* asset);
        Texture(size_t _width, size_t _height);
        Texture(TTF_Font* font, Eng3D::Color color, const std::string& msg);
        ~Texture() override;
        void create_dummy();
        void upload(TextureOptions options = default_options);
        void upload(SDL_Surface* surface);
        void gen_mipmaps() const;
        void bind() const;
        void delete_gputex();
        void guillotine(const Eng3D::Texture& map, int x, int y, int w, int h);
        void to_file(const std::string& filename);

#ifdef E3D_BACKEND_OPENGL
        GLuint gl_tex_num = 0;
#endif
    };

    // Array of textures
    class TextureArray: public BinaryImage {
    public:
        TextureArray(const std::string& path, size_t _tiles_x, size_t _tiles_y);
        void upload();
        size_t layers;
        size_t tiles_x, tiles_y;
#ifdef E3D_BACKEND_OPENGL
        GLuint gl_tex_num = 0;
#endif
    };

    template <class T>
    void hash_combine(std::size_t& s, const T& v)
    {
        std::hash<T> h;
        s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
    }

    /**
     * @brief Texture map has implementation
     * 
     */
    struct TextureMapHash {
        std::size_t operator()(const std::pair<std::string, TextureOptions>& key) const
        {
            std::size_t res = 0;
            hash_combine(res, key.first);
            TextureOptions s = key.second;
            hash_combine(res, s.target);
            hash_combine(res, s.wrap_s);
            hash_combine(res, s.wrap_t);
            hash_combine(res, s.min_filter);
            hash_combine(res, s.mag_filter);
            hash_combine(res, s.internal_format);
            hash_combine(res, s.format);
            hash_combine(res, s.type);
            hash_combine(res, s.editable);
            return res;
        }
    };

    /**
     * @brief General manager for textures, caches textures into the memory instead of reading them off the disk
     * every time they need to be accessed.
     * 
     */
    class TextureManager {
    private:
        std::unordered_map<std::pair<std::string, TextureOptions>, std::shared_ptr<Eng3D::Texture>, TextureMapHash> textures;
        std::shared_ptr<Eng3D::Texture> white;
        Eng3D::State& s;
    public:
        TextureManager() = delete;
        TextureManager(Eng3D::State& s);
        ~TextureManager() = default;
        std::shared_ptr<Texture> load(const std::string& path, TextureOptions options = default_options);
        std::shared_ptr<Texture> load(std::shared_ptr<Eng3D::IO::Asset::Base> asset, TextureOptions options = default_options);
        std::shared_ptr<Texture> get_white();
    };
};
