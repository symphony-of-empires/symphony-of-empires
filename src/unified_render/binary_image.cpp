#include <sstream>
#include <stdexcept>
#include <cstring>

#include "unified_render/binary_image.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BinaryImage::BinaryImage(void) {
    
}

BinaryImage::BinaryImage(const UnifiedRender::IO::Path& path) {
    from_file(path.str);
}

BinaryImage::BinaryImage(size_t _width, size_t _height)
    : width{ _width },
    height{ _height }
{
    buffer = new uint32_t[width * height];
}

BinaryImage::BinaryImage(const BinaryImage& tex) {
    height = tex.height;
    width = tex.width;
    buffer = new uint32_t[width * height];
    std::memcpy(buffer, tex.buffer, sizeof(uint32_t) * (width * height));
}

void BinaryImage::from_file(const UnifiedRender::IO::Path& path) {
    int i_width, i_height, i_channels;

    // stbi can do the conversion to RGBA for us ;)
    buffer = (uint32_t*)stbi_load(Path::get(path.str.c_str()).c_str(), &i_width, &i_height, &i_channels, 4);
    if(buffer == nullptr) {
        throw BinaryImageException(path.str, std::string() + "Image load error: " + stbi_failure_reason());
    }

    width = (size_t)i_width;
    height = (size_t)i_height;
}

BinaryImage::~BinaryImage() {
    delete[] buffer;
}
