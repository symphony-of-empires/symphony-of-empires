#pragma once

#include <cstdint>
#include <cstddef>
#include <exception>
#include <sstream>
#include <string>

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
    BinaryImage() {};
    BinaryImage(const std::string& path);
    BinaryImage(size_t _width, size_t _height);
    BinaryImage(const BinaryImage& tex);
    BinaryImage& operator=(const BinaryImage&) = default;
    virtual ~BinaryImage();

    uint32_t* buffer;
    size_t width, height;

    virtual void from_file(const std::string& path);
};