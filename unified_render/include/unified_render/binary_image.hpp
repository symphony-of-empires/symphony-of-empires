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
//      binary_image.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <cstddef>
#include <exception>
#include <memory>
#include <string>

#include "unified_render/io.hpp"

class BinaryImageException: public std::exception {
    std::string buffer;
public:
    BinaryImageException(const std::string& filename, const std::string& message) {
        buffer = "" + filename + ":" + message;
    }

    virtual const char* what(void) const noexcept {
        return buffer.c_str();
    }
};

// This binary image class helps load images and visual resources from the disk; the
// binary image IS NOT a texture, it is intended to be used in contextes which are
// NOT rendering - for rendering purpouses see texture class from client's implementation
class BinaryImage {
public:
    BinaryImage(void);
    BinaryImage(const UnifiedRender::IO::Path& path);
    BinaryImage(size_t _width, size_t _height);
    BinaryImage(const BinaryImage& tex);
    BinaryImage& operator=(const BinaryImage&) = default;
    virtual ~BinaryImage(void);
    virtual void from_file(const UnifiedRender::IO::Path& path);
    uint32_t get_pixel(size_t x, size_t y) const;

    std::unique_ptr<uint32_t> buffer;
    size_t width, height;
};