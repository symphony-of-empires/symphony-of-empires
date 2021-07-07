#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>
#include <cstddef>
#include <GL/gl.h>

class Texture {
public:
	Texture() {};
	Texture(const char* path);
	Texture(size_t _width, size_t _height) : width(_width), height(_height) {
		this->buffer = new uint32_t[this->width* this->height];
	}
	Texture(const Texture& tex) {
		height = tex.height;
		width = tex.width;
		buffer = tex.buffer;
	};
	Texture& operator=(const Texture&) = default;
	~Texture();

	uint32_t* buffer;
	size_t width;
	size_t height;
	GLuint gl_tex_num;

	void from_file(const char* path);
	void create_dummy();
	void to_opengl();
	void delete_opengl();
};

#include <set>
#include <string>
#include <algorithm>
#include "print.hpp"
class TextureManager {
private:
	std::set<std::pair<Texture *, std::string>> textures;
public:
	const Texture& load_texture(std::string path) {
		// Find texture when wanting to be loaded
		auto it = std::find_if(this->textures.begin(), this->textures.end(), [&path](const std::pair<Texture *, std::string>& element) {
			return (element.second == path);
		});

		if(it != this->textures.end()) {
			print_error("Duplicate texture! %s", path.c_str());
			return *((*it).first);
		}

		// Otherwise texture is not in our control, so we create a new texture
		Texture* tex = new Texture(path.c_str());
		tex->to_opengl();
		this->textures.insert(std::make_pair(tex, path));
		return *((const Texture *)tex);
	}
};

#endif
