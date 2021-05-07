#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <stddef.h>
#include <GL/gl.h>

typedef struct {
	uint32_t * buffer;
	size_t width;
	size_t height;
	GLuint gl_tex_num;
}Texture;

void Texture_Create(Texture * tex);
void Texture_Delete(Texture * tex, int keep_opengl);
int Texture_FromFile(Texture * tex, const char * name);
void Texture_ToOpenGL(Texture * tex);

/*
class Texture {
	public:
	uint32_t * buffer;
	size_t width;
	size_t height;
	GLuint gl_tex_num;

	Texture();
	~Texture();
	void from_file(Texture tex, const char * name);
	void to_opengl(Texture tex);
};
*/

#endif