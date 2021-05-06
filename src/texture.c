#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <png.h>
#include <zlib.h>
#include "texture.h"

void Texture_Create(Texture * tex) {
	memset(tex, 0, sizeof(Texture));
	return;
}

void Texture_Delete(Texture * tex, int keep_opengl) {
	if(tex->gl_tex_num && !keep_opengl) {
		glDeleteTextures(1, &tex->gl_tex_num);
	}
	free(tex->buffer);
	return;
}

int Texture_FromFile(Texture * tex, const char * name) {
	png_image image;
	
	/* Open initial file */
	memset(&image, 0, sizeof(png_image));
	image.version = PNG_IMAGE_VERSION;
	if(!png_image_begin_read_from_file(&image, name)) {
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

	tex->width = (size_t)image.width;
	tex->height = (size_t)image.height;
	
	/* Store information onto buffer */
	tex->buffer = malloc(PNG_IMAGE_SIZE(image));
	if(tex->buffer != NULL && png_image_finish_read(&image, NULL, tex->buffer, 0, NULL) != 0) {
		/* Free the image */
		png_image_free(&image);
	} else {
		if(tex->buffer == NULL) {
			png_image_free(&image);
		} else {
			free(tex->buffer);
		}
		return 1;
	}
	
	tex->gl_tex_num = 0;
	return 0;
}

#include <GL/gl.h>
void Texture_ToOpenGL(Texture * tex) {
	glGenTextures(1, &tex->gl_tex_num);
	glBindTexture(GL_TEXTURE_2D, tex->gl_tex_num);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return;
}