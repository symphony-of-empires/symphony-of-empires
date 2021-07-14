#ifndef TEXTURE_H
#define TEXTURE_H

/**
 * This file implements a texture which is based from the binary image class to avoid
 * code repetition.
 *
 * The difference of the texture from the binary image is that the texture is oriented towards
 * OpenGL rendering more than the aforementioned binary image.
 *
 * A binary image is usable on any context but rendering, while the texture is
 * intended to be used in rendering cases only
 */

#include <cstdint>
#include <cstddef>
#include <GL/gl.h>
#include <string>
#include "binary_image.hpp"

class TextureException : public BinaryImageException {
public:
	TextureException(std::string filename, std::string message) : BinaryImageException(filename, message) {};
};

class Texture : public BinaryImage {
public:
	Texture() {};
	Texture(std::string path) : BinaryImage(path) {};

	GLuint gl_tex_num;
	void create_dummy();
	void to_opengl();
	void delete_opengl();
	void guillotine(const Texture& map, int x, int y, int w, int h);
};

/**
 * This texture manager helps to cache textures instead of loading them of the disk each time they are used
 * and also acts as a "texture loader"
 */
#include <set>
class TextureManager {
private:
	std::set<std::pair<Texture*, std::string>> textures;
public:
	const Texture& load_texture(std::string path);
};

#endif
