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
//      binary_image.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <sstream>
#include <stdexcept>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION 1
#include "unified_render/stb_image.h"

#include "unified_render/binary_image.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"

BinaryImage::BinaryImage(void) {
    
}

BinaryImage::BinaryImage(const UnifiedRender::IO::Path& path) {
    from_file(path.str);
}

BinaryImage::BinaryImage(size_t _width, size_t _height)
    : width{ _width },
    height{ _height }
{
    buffer = std::make_unique<uint32_t[]>(width * height);
    std::memset(buffer.get(), 0, sizeof(uint32_t) * width * height);
}

BinaryImage::BinaryImage(const BinaryImage& tex)
    : BinaryImage(tex.width, tex.height)
{
    std::memcpy(buffer.get(), tex.buffer.get(), sizeof(uint32_t) * (width * height));
}

void BinaryImage::from_file(const UnifiedRender::IO::Path& path) {
    int i_width, i_height, i_channels;

    // stbi can do the conversion to RGBA for us ;)
    stbi_uc* c_buffer = stbi_load(Path::get(path.str.c_str()).c_str(), &i_width, &i_height, &i_channels, 4);
    if(c_buffer == nullptr) {
        throw BinaryImageException(path.str, std::string() + "Image load error: " + stbi_failure_reason());
    }

    width = static_cast<size_t>(i_width);
    height = static_cast<size_t>(i_height);

    // valgrind complains about mismatched frees
    buffer = std::make_unique<uint32_t[]>(width * height);
    std::memcpy(buffer.get(), c_buffer, sizeof(uint32_t) * width * height);
    std::free(c_buffer);
}

uint32_t BinaryImage::get_pixel(size_t x, size_t y) const {
    return buffer.get()[x + y * width];
}

BinaryImage::~BinaryImage() {
    
}
