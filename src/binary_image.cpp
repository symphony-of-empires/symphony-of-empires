#include <sstream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <errno.h>
#include <png.h>
#include "binary_image.hpp"
#include "path.hpp"
#include "print.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BinaryImage::BinaryImage(const std::string& path) {
    from_file(path);
}

BinaryImage::BinaryImage(size_t _width, size_t _height) : width(_width), height(_height) {
    buffer = new uint32_t[width * height];
}

BinaryImage::BinaryImage(const BinaryImage& tex) {
    height = tex.height;
    width = tex.width;
    buffer = new uint32_t[width * height];
    memcpy(buffer, tex.buffer, sizeof(uint32_t) * (width * height));
}

void BinaryImage::from_file(const std::string& path) {
    int i_width, i_height, i_channels;

    // stbi can do the conversion to RGBA for us ;)
    buffer = (uint32_t *)stbi_load(Path::get(path).c_str(), &i_width, &i_height, &i_channels, 4);
    if(buffer == NULL) {
        throw BinaryImageException(path, "Image load error");
    }

    width = (size_t)i_width;
    height = (size_t)i_height;
}

BinaryImage::~BinaryImage() {
    delete[] buffer;
}
