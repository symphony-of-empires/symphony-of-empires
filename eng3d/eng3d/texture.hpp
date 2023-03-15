// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
#include <mutex>

#include "eng3d/ttf.hpp"
#include "eng3d/binary_image.hpp"
#include "eng3d/color.hpp"

struct SDL_Surface;
namespace Eng3D::IO::Asset {
    struct Base;
}

namespace Eng3D {
    class State;
    class TextureManager;

    struct TextureException: BinaryImageException {
        TextureException(const std::string_view filename, const std::string_view message)
            : BinaryImageException(filename, message)
        {

        }
    };

    struct TextureOptions {
        TextureOptions() = default;
        enum Target {
            TEXTURE_2D,
        } target = Eng3D::TextureOptions::Target::TEXTURE_2D;
        enum Wrap {
            REPEAT,
            CLAMP_TO_EDGE,
        } wrap_s = Eng3D::TextureOptions::Wrap::REPEAT;
        Wrap wrap_t = Eng3D::TextureOptions::Wrap::REPEAT;
        enum Filter {
            NEAREST,
            LINEAR,
            LINEAR_MIPMAP,
            NEAREST_MIPMAP,
            NEAREST_LINEAR_MIPMAP,
            LINEAR_NEAREST_MIPMAP,
        } min_filter = Eng3D::TextureOptions::Filter::NEAREST;
        Filter mag_filter = Eng3D::TextureOptions::Filter::NEAREST;
        enum Format {
            RGBA,
            RED,
            RGB32F,
            SRGB,
            SRGB_ALPHA,
        } format = Eng3D::TextureOptions::Format::RGBA;
        Format internal_format = Eng3D::TextureOptions::Format::RGBA;
        enum Type {
            UNSIGNED_BYTE,
        } type = Eng3D::TextureOptions::Type::UNSIGNED_BYTE;
        bool editable = false;
        bool compressed = true;
        bool instant_upload = false; // Has to be uploaded immediately and not async
        
        constexpr bool operator==(const TextureOptions& o) const {
            return target == o.target && wrap_s == o.wrap_s && wrap_t == o.wrap_t && min_filter == o.min_filter && mag_filter == o.mag_filter && internal_format == o.internal_format && format == o.format && type == o.type && editable == o.editable;
        }
    };
    const TextureOptions default_options;

    class Texture: public Eng3D::BinaryImage {
        void _upload(TextureOptions options = default_options);
        void _upload(SDL_Surface* surface);
        void delete_gputex();
    public:
        Texture() = default;
        Texture(const std::string_view path)
            : Eng3D::BinaryImage(path)
        {

        }
        Texture(const Eng3D::IO::Asset::Base* asset)
            : Eng3D::BinaryImage((asset == nullptr) ? Eng3D::IO::Path("") : Eng3D::IO::Path(asset->abs_path))
        {

        }
        Texture(size_t _width, size_t _height, size_t _bpp = 32)
            : Eng3D::BinaryImage(_width, _height, _bpp)
        {

        }
        ~Texture() override;
        void create_dummy();

        /// @brief Frontend for uploading (schedules or instantly uploads)
        /// @param options Options for upload
        void upload(TextureOptions options = default_options) {
            this->_upload(options); // Do upload instantly
        }

        /// @brief Uploads a text texture (shceduled or not) if it's scheduled, the surface
        /// is handed ownership over to the scheduler and it will be automatically deallocated
        /// once the request is fullfilled
        /// @param surface Surface to base texture from
        void upload(SDL_Surface* surface) {
            this->_upload(surface);
        }

        void gen_mipmaps() const;
        void bind() const;
        void guillotine(const Eng3D::Texture& map, int x, int y, int w, int h);
        void to_file(const std::string_view filename) override;
        
        unsigned int id = 0;
        bool managed = false;
        friend class Eng3D::TextureManager;
    };

    // Array of textures
    struct TextureArray: Eng3D::BinaryImage {
        TextureArray(const std::string_view path, size_t _tiles_x, size_t _tiles_y)
            : Eng3D::BinaryImage(path),
            tiles_x{ _tiles_x },
            tiles_y{ _tiles_y }
        {

        }

        void upload();
        size_t layers;
        size_t tiles_x, tiles_y;
        unsigned int id = 0;
    };

    template <class T>
    inline void hash_combine(std::size_t& s, const T& v) {
        std::hash<T> h;
        s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
    }

    /// @brief Texture map has implementation
    struct TextureMapHash {
        inline std::size_t operator()(const std::pair<std::size_t, TextureOptions>& key) const {
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

    struct TextureUploadRequest {
        Texture* texture;
        TextureOptions options;
        SDL_Surface* surface = nullptr;
    };

    /// @brief General manager for textures, caches textures into the memory instead of reading them off the disk
    /// every time they need to be accessed.
    class TextureManager {
        std::unordered_map<std::pair<std::size_t, TextureOptions>, std::shared_ptr<Eng3D::Texture>, TextureMapHash> textures;
        std::vector<TextureUploadRequest> unuploaded_textures;
        std::mutex unuploaded_lock;
        std::shared_ptr<Eng3D::Texture> white;

        /// @brief Helper structure for hashing pairs
        struct pair_hash {
            std::size_t operator()(const auto& x) const noexcept {
                return std::hash<decltype(x.first)>{}(x.first) ^ std::hash<decltype(x.second)>{}(x.second);
            }
        };
        /// @brief Stores the text textures
        std::unordered_map<std::pair<std::size_t, Eng3D::Color>, std::shared_ptr<Eng3D::Texture>, pair_hash> text_textures;
        Eng3D::State& s;
    public:
        TextureManager() = delete;
        TextureManager(Eng3D::State& _s);
        ~TextureManager();
        std::shared_ptr<Eng3D::Texture> load(const std::string_view path, TextureOptions options = default_options);
        std::shared_ptr<Eng3D::Texture> load(std::shared_ptr<Eng3D::IO::Asset::Base> asset, TextureOptions options = default_options);
        std::shared_ptr<Eng3D::Texture> gen_text(Eng3D::TrueType::Font& font, Eng3D::Color color, const std::string_view msg);
        std::shared_ptr<Eng3D::Texture> get_white();

        friend class Eng3D::Texture;
    };
}
