#ifndef BINARY_IMAGE_H
#define BINARY_IMAGE_H

#include <cstdint>
#include <cstddef>

class BinaryImage {
public:
	BinaryImage() {};
	BinaryImage(const char* path);
	BinaryImage(size_t _width, size_t _height) : width(_width), height(_height) {
		this->buffer = new uint32_t[this->width* this->height];
	}
	BinaryImage(const BinaryImage& tex) {
		height = tex.height;
		width = tex.width;
		buffer = tex.buffer;
	};
	BinaryImage& operator=(const BinaryImage&) = default;
	~BinaryImage();
	
	uint32_t* buffer;
	size_t width;
	size_t height;
	
	void from_file(const char* path);
};

#endif
