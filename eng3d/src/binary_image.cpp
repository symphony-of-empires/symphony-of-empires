// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      binary_image.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <sstream>
#include <stdexcept>
#include <cstring>

extern "C" {
#define STB_IMAGE_IMPLEMENTATION 1
#include "eng3d/stb_image.h"
}

#include "eng3d/binary_image.hpp"
#include "eng3d/path.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/io.hpp"
#include "eng3d/state.hpp"
#include "eng3d/log.hpp"

Eng3D::BinaryImage::BinaryImage(const Eng3D::IO::Path& path) {
    from_file(path.str);
}

Eng3D::BinaryImage::BinaryImage(size_t _width, size_t _height)
    : width{ _width },
    height{ _height }
{
    buffer = std::make_unique<uint32_t[]>(width * height);
    std::memset(buffer.get(), 0, sizeof(uint32_t) * width * height);
}

Eng3D::BinaryImage::BinaryImage(const Eng3D::BinaryImage& tex)
    : Eng3D::BinaryImage(tex.width, tex.height)
{
    std::memcpy(buffer.get(), tex.buffer.get(), sizeof(uint32_t) * (width * height));
}

void Eng3D::BinaryImage::from_file(const Eng3D::IO::Path& path) {
    int i_width, i_height, i_channels;

    // stbi can do the conversion to RGBA for us ;)
    stbi_uc* c_buffer = stbi_load(path.str.c_str(), &i_width, &i_height, &i_channels, 4);
    if(c_buffer == nullptr)
        CXX_THROW(BinaryImageException, path.str, stbi_failure_reason());

    width = static_cast<size_t>(i_width);
    height = static_cast<size_t>(i_height);

    // Requires re-allocation so it's handled by new[] (we don't know what stbi uses)
    buffer = std::make_unique<uint32_t[]>(width * height);
    std::memcpy(buffer.get(), c_buffer, sizeof(uint32_t) * width * height);
    std::free(c_buffer);
}
