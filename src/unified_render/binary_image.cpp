#include <sstream>
#include <stdexcept>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    buffer = std::unique_ptr<uint32_t>(new uint32_t[width * height]);
}

BinaryImage::BinaryImage(const BinaryImage& tex)
    : BinaryImage(tex.width, tex.height)
{
    std::memcpy(buffer.get(), tex.buffer.get(), sizeof(uint32_t) * (width * height));
}

void BinaryImage::from_file(const UnifiedRender::IO::Path& path) {
    int i_width, i_height, i_channels;

    // stbi can do the conversion to RGBA for us ;)
    void* c_buffer = stbi_load(Path::get(path.str.c_str()).c_str(), &i_width, &i_height, &i_channels, 4);
    if(c_buffer == nullptr) {
        throw BinaryImageException(path.str, std::string() + "Image load error: " + stbi_failure_reason());
    }
    std::memcpy(buffer.get(), c_buffer, sizeof(uint32_t) * (width * height));

    width = (size_t)i_width;
    height = (size_t)i_height;
}

uint32_t BinaryImage::get_pixel(size_t x, size_t y) const {
    return buffer.get()[x + y * width];
}

BinaryImage::~BinaryImage() {
    buffer.reset(nullptr);
}
