#ifndef BINARY_IMAGE_H
#define BINARY_IMAGE_H

/**
 * This binary image class helps load images and visual resources from the disk; the
 * binary image IS NOT a texture, it is intended to be used in contextes which are
 * NOT rendering - for rendering purpouses see texture class from client's implementation
 */

#include <cstdint>
#include <cstddef>
#include <exception>
#include <sstream>
#include <string>

class BinaryImageException : public std::exception {
public:
	std::string buffer;
	BinaryImageException(std::string filename, std::string message) {
		buffer = "";
		buffer += filename;
		buffer += ": ";
		buffer += message;
	};

	virtual const char* what(void) {
		return buffer.c_str();
	};
};

class BinaryImage {
public:
	BinaryImage() {};
	BinaryImage(std::string path);
	BinaryImage(size_t _width, size_t _height) : width(_width), height(_height) {
		buffer = new uint32_t[width * height];
	}
	BinaryImage(const BinaryImage& tex) noexcept;
	BinaryImage& operator=(const BinaryImage&) = default;
	~BinaryImage();
	
	uint32_t* buffer;
	size_t width;
	size_t height;
	
	virtual void from_file(std::string path);
};

#endif
