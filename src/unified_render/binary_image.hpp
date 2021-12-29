#pragma once

#include <cstdint>
#include <cstddef>
#include <exception>
#include <sstream>
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
    BinaryImage();
    BinaryImage(const UnifiedRender::IO::Path& path);
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
    BinaryImage(T _width, T _height) : BinaryImage((size_t)_width, (size_t)_height) {};
    BinaryImage(size_t _width, size_t _height);
    BinaryImage(const BinaryImage& tex);
    BinaryImage& operator=(const BinaryImage&) = default;
    virtual ~BinaryImage();
    virtual void from_file(const UnifiedRender::IO::Path& path);

    uint32_t* buffer;
    size_t width, height;
};