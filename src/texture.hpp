#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <stddef.h>
#include <GL/gl.h>

class Texture {
	public:
	uint32_t * buffer;
	size_t width;
	size_t height;
	GLuint gl_tex_num;

	Texture();
	~Texture();
	int from_file(const char * name);
	void to_opengl();
	void delete_opengl();
};

#endif