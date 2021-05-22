#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <stddef.h>
#include <GL/gl.h>

class Texture {
public:
	Texture() {};
	Texture(const char * path);
	Texture(size_t _width, size_t _height) {
		this->width = _width;
		this->height = _height;
		this->buffer = new uint32_t[this->width * this->height];
	}
	Texture(const Texture& tex) : buffer(tex.buffer) {};
	Texture& operator=(const Texture&) = default;
	~Texture();

	uint32_t * buffer;
	size_t width;
	size_t height;
	GLuint gl_tex_num;

	void from_file(const char * path);
	void create_dummy();
	void to_opengl();
	void delete_opengl();
};

#endif
