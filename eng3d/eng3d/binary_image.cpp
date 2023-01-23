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
//      binary_image.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <sstream>
#include <stdexcept>
#include <cstring>

#include "eng3d/stb_image.h"
#include "eng3d/stb_image_write.h"

#include "eng3d/binary_image.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/io.hpp"
#include "eng3d/state.hpp"
#include "eng3d/log.hpp"

Eng3D::BinaryImage::BinaryImage(const Eng3D::IO::Path& path)
    : buffer(nullptr, &free)
{
    from_file(path.str);
}

Eng3D::BinaryImage::BinaryImage(size_t _width, size_t _height, size_t _bpp)
    : buffer(nullptr, &free),
    width{ _width },
    height{ _height },
    bpp{ _bpp }
{
    if(bpp == 32)
        buffer.reset(reinterpret_cast<uint32_t *>(std::malloc(width * height * sizeof(uint32_t))));
    /*else if(bpp == 16)
        buffer = std::make_unique<uint16_t[]>(width * height);
    else if(bpp == 8)
        buffer = std::make_unique<uint8_t[]>(width * height);*/
}

Eng3D::BinaryImage::BinaryImage(const Eng3D::BinaryImage& tex)
    : Eng3D::BinaryImage(tex.width, tex.height, tex.bpp)
{
    if(tex.bpp != this->bpp && this->bpp != 32)
        CXX_THROW(BinaryImageException, "internal", "Can't do 16-bit/8-bit copies");
    std::memcpy(buffer.get(), tex.buffer.get(), sizeof(uint32_t) * (width * height));
}

void Eng3D::BinaryImage::from_file(const Eng3D::IO::Path& path) {
    // stbi can do the conversion to RGBA for us ;)
    int i_width, i_height, i_channels;
    auto* c_buffer = stbi_load(path.str.c_str(), &i_width, &i_height, &i_channels, 4);
    if(c_buffer == nullptr)
        CXX_THROW(BinaryImageException, path.str, stbi_failure_reason());
    width = static_cast<size_t>(i_width);
    height = static_cast<size_t>(i_height);

    // Transfer ownership to our binary image
    buffer.reset(reinterpret_cast<uint32_t *>(c_buffer));
}

void Eng3D::BinaryImage::to_file(const std::string& filename) {
    int channel_count = bpp == 32 ? 4 : bpp == 16 ? 2 : bpp == 8 ? 1 : 0;
    int stride = channel_count * width;
    stbi_write_png(filename.c_str(), width, height, channel_count, buffer.get(), stride);
}
