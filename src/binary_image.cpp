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
    buffer = (uint32_t *)stbi_load(Path::get(path).c_str(), &i_width, &i_height, &i_channels, 0);
    if(buffer == NULL) {
        throw BinaryImageException(path, "Image load error");
    }

    width = (size_t)i_width;
    height = (size_t)i_height;

    if(i_channels != 4) {
        if(i_channels < 3) {
            throw BinaryImageException(path, "Image must be RGB");
        }

        // Extend the image to RGBA
        print_info("Extending non-RGBA image to RGBA");
        uint32_t *second_buffer = new uint32_t[width * height];
        if(second_buffer == NULL) {
            throw BinaryImageException(path, "Out of memory");
        }

        uint8_t *c_buffer = (uint8_t *)buffer;
        for(size_t i = 0, j = 0; i < (width * height) * i_channels && j < (width * height); i += i_channels, j++) {
            uint32_t colour = (uint32_t)c_buffer[i + 2]
                | ((uint32_t)c_buffer[i + 1] << 8)
                | ((uint32_t)c_buffer[i + 0] << 16)
                | ((uint32_t)0xff << 24);
            second_buffer[j] = colour;
        }
        free(buffer);
        buffer = second_buffer;
    }
}

BinaryImage::~BinaryImage() {
    delete[] buffer;
}
