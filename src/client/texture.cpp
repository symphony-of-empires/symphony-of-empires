#include <string>
#include "texture.hpp"
#include "path.hpp"
#include "print.hpp"

/**
 * This dummy texture helps to avoid crashes due to missing buffers or so, and also gives
 * visual aid of errors
 */
void Texture::create_dummy() {
    width = 16;
    height = 16;
    buffer = new uint32_t[width * height];
    if(buffer == nullptr) {
        throw TextureException("Dummy", "Out of memory for dummy texture");
    }

    // Fill in with a pattern of pink and black
    // This should be autovectorized by gcc
    for(size_t i = 0; i < width * height; i++) {
        buffer[i] = (i % 2) ? 0xff000000 : 0xff808000;
    }
}

/**
 * Converts the texture into a OpenGL texture, and assigns it a number
  */
void Texture::to_opengl() {
    glGenTextures(1, &gl_tex_num);
    glBindTexture(GL_TEXTURE_2D, gl_tex_num);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
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
    glDeleteTextures(1, &gl_tex_num);
}

#include <algorithm>
TextureManager* g_texture_manager;

/**
 * Finds a texture in the list of a texture manager
 * if the texture is already in the list we load the saved texture from the list
 * instead of loading it from the disk.
 *
 * Otherwise we load it from the disk and add it to the saved texture list
 *
 * The object returned is a pointer and we will not give ownership of textures in the list
 * and program should not modify the contents of it since it will differ from the texture
 * on the disk, and our main point is to mirror loaded textures from the disk - not modify
 * them.
 */
const Texture& TextureManager::load_texture(std::string path) {
    // Find texture when wanting to be loaded
    auto it = std::find_if(this->textures.begin(), this->textures.end(), [&path](const std::pair<Texture *, std::string>& element) {
        return (element.second == path);
    });

    // Load texture from cached texture list
    if(it != this->textures.end()) {
        return *((*it).first);
    }

    print_info("Loaded and cached texture %s", path.c_str());

    // Otherwise texture is not in our control, so we create a new texture
    Texture* tex;
    try {
        tex = new Texture(path);
    } catch(BinaryImageException&) {
        tex = new Texture();
        tex->create_dummy();
    }
    
    tex->to_opengl();
    this->textures.insert(std::make_pair(tex, path));
    return *((const Texture *)tex);
}
