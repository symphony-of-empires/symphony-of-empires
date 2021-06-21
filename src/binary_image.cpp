#include <string.h>
#include <string>
#include <errno.h>
#include <png.h>
#include <zlib.h>
#include "binary_image.hpp"
#include "path.hpp"
#include "print.hpp"

BinaryImage::BinaryImage(const char* path) {
	this->from_file(path);
}

void BinaryImage::from_file(const char* path) {
	png_image image;
	
	// Open initial file
	memset(&image, 0, sizeof(png_image));
	image.version = PNG_IMAGE_VERSION;
	if(!png_image_begin_read_from_file(&image, Path::get(path).c_str())) {
		print_error("%s %s", path, image.message);
		return;
	}
	
	// Convert onto RGBA so it can be easily read
	image.format = PNG_FORMAT_RGBA;

	// We cannot allow images bigger than our integers (we are avoiding overflows!)
	if(image.width >= UINT16_MAX || image.height >= UINT16_MAX) {
		png_image_free(&image);
		print_error("%s texture too big", path);
		return;
	}

	// We can't allow images with 0 size either
	if(!image.width || !image.height) {
		png_image_free(&image);
		print_error("%s texture is too small", path);
		return;
	}

	this->width = (size_t)image.width;
	this->height = (size_t)image.height;
	
	// Store information onto buffer
	this->buffer = new uint32_t[image.width* image.height];
	if(this->buffer != nullptr && png_image_finish_read(&image, NULL, this->buffer, 0, NULL) != 0) {
		// Free the image
		png_image_free(&image);
	} else {
		if(this->buffer == nullptr) {
			png_image_free(&image);
		} else {
			delete[] this->buffer;
		}
		exit(EXIT_FAILURE);
	}
}

BinaryImage::~BinaryImage() {
	delete[] this->buffer;
}
