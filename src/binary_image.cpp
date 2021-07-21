#include <sstream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <errno.h>
#include <png.h>
#include "binary_image.hpp"
#include "path.hpp"
#include "print.hpp"

BinaryImage::BinaryImage(std::string path) {
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

void BinaryImage::from_file(std::string path) {
    png_image image;
    
    // Open initial file
    memset(&image, 0, sizeof(png_image));
    image.version = PNG_IMAGE_VERSION;
    if(!png_image_begin_read_from_file(&image, Path::get(path).c_str())) {
        throw BinaryImageException(path, image.message);
    }
    
    // Convert onto RGBA so it can be easily read
    image.format = PNG_FORMAT_RGBA;

    // We cannot allow images bigger than our integers (we are avoiding overflows!)
    if(image.width >= UINT16_MAX || image.height >= UINT16_MAX) {
        png_image_free(&image);
        throw BinaryImageException(path, "Texture too big");
    }

    // We can't allow images with 0 size either
    if(!image.width || !image.height) {
        png_image_free(&image);
        throw BinaryImageException(path, "Texture too small");
    }

    width = (size_t)image.width;
    height = (size_t)image.height;
    
    // Store information onto buffer
    buffer = new uint32_t[image.width* image.height];
    if(buffer != nullptr && png_image_finish_read(&image, NULL, buffer, 0, NULL) != 0) {
        // Free the image
        png_image_free(&image);
    } else {
        if(buffer == nullptr) {
            png_image_free(&image);
        } else {
            delete[] buffer;
        }
        throw BinaryImageException(path, "Allocation error");
    }
}

BinaryImage::~BinaryImage() {
    delete[] buffer;
}
