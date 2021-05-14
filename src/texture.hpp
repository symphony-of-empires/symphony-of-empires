#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <stddef.h>
#ifdef _WIN32
#include <windows.h>
#include <gl/GL.h>
#else
#include <GL/gl.h>
#endif

class Texture {
public:
	Texture() {};
	Texture(const char * path);
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
