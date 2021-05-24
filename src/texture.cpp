#include <string.h>
#include <string>
#include <errno.h>
#include <png.h>
#include <zlib.h>
#include "texture.hpp"
#include "path.hpp"
#include "print.hpp"

Texture::Texture(const char * path) {
	this->from_file(path);
}

void Texture::create_dummy() {
	this->width = 32;
	this->height = 32;
	this->buffer = new uint32_t[this->width * this->height];
	if(this->buffer == nullptr) {
		print_error("out of memory for dummy texture\n");
		exit(EXIT_FAILURE);
	}

	// Fill in with a pattern
	// This should be autovectorized by gcc
	for(size_t i = 0; i < this->width * this->height; i++) {
		this->buffer[i] = (i % 2) ? 0xff000000 : 0xff00ff00;
	}
}

void Texture::from_file(const char * path) {
	png_image image;
	
	// Open initial file
	memset(&image, 0, sizeof(png_image));
	image.version = PNG_IMAGE_VERSION;
	if(!png_image_begin_read_from_file(&image, Path::get(path).c_str())) {
		print_error("%s %s", path, image.message);
		this->create_dummy();
		return;
	}
	
	// Convert onto RGBA so it can be easily read
	image.format = PNG_FORMAT_RGBA;

	// We cannot allow images bigger than our integers (we are avoiding overflows!)
	if(image.width >= UINT16_MAX || image.height >= UINT16_MAX) {
		png_image_free(&image);
		print_error("%s texture too big", path);
		this->create_dummy();
		return;
	}

	// We can't allow images with 0 size either
	if(!image.width || !image.height) {
		png_image_free(&image);
		print_error("%s texture is too small", path);
		this->create_dummy();
		return;
	}

	this->width = (size_t)image.width;
	this->height = (size_t)image.height;
	
	// Store information onto buffer
	this->buffer = new uint32_t[image.width * image.height];
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

	this->gl_tex_num = 0;
}

Texture::~Texture() {
	delete[] this->buffer;
}

#include <GL/gl.h>

/**
  * Converts the texture into a OpenGL texture, and assigns it a number
  */
void Texture::to_opengl() {
	/*for(size_t i = 0; i < this->width * this->height; i++) {
		uint32_t * comp = &this->buffer[i];
		*comp &= 0xffe0e0e0;
	}*/

	glGenTextures(1, &this->gl_tex_num);
	glBindTexture(GL_TEXTURE_2D, this->gl_tex_num);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return;
}

/**
  * Deletes the OpenGL representation of this texture
  */
void Texture::delete_opengl() {
	glDeleteTextures(1, &this->gl_tex_num);
}

TextureManager * g_texture_manager;