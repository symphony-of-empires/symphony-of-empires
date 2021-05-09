#include <string.h>
#include <string>
#include <errno.h>
#include <png.h>
#include <zlib.h>
#include "texture.hpp"
#include "path.hpp"

Texture::Texture(const char * path) {
	this->from_file(path);
}

void Texture::from_file(const char * path) {
	png_image image;
	
	/* Open initial file */
	memset(&image, 0, sizeof(png_image));
	image.version = PNG_IMAGE_VERSION;
	if(!png_image_begin_read_from_file(&image, Resource_GetPath(path).c_str())) {
		perror(image.message);
		exit(EXIT_FAILURE);
	}
	
	/* Convert onto RGBA so it can be easily read */
	image.format = PNG_FORMAT_RGBA;

	/* We cannot allow images bigger than our integers (we are avoiding overflows!) */
	if(image.width > UINT16_MAX || image.height > UINT16_MAX) {
		png_image_free(&image);
		perror("image too big\n");
		exit(EXIT_FAILURE);
	}

	/* We can't allow images with 0 size either */
	if(!image.width || !image.height) {
		png_image_free(&image);
		perror("size of zero\n");
		exit(EXIT_FAILURE);
	}

	this->width = (size_t)image.width;
	this->height = (size_t)image.height;
	
	/* Store information onto buffer */
	this->buffer = new uint32_t[image.width * image.height];
	if(this->buffer != nullptr && png_image_finish_read(&image, NULL, this->buffer, 0, NULL) != 0) {
		/* Free the image */
		png_image_free(&image);
	} else {
		if(this->buffer == nullptr) {
			png_image_free(&image);
		} else {
			delete[] this->buffer;
		}
		exit(EXIT_FAILURE);
	}
	
	printf("texture %s loaded\n", path);
	this->gl_tex_num = 0;
}

Texture::~Texture() {
	delete[] this->buffer;
}

#include <GL/gl.h>
void Texture::to_opengl() {
	glGenTextures(1, &this->gl_tex_num);
	glBindTexture(GL_TEXTURE_2D, this->gl_tex_num);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return;
}

void Texture::delete_opengl() {
	glDeleteTextures(1, &this->gl_tex_num);
}
