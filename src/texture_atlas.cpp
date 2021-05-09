#include "texture_atlas.hpp"
#include <png.h>
#include <GL/gl.h>

TextureAtlas::TextureAtlas(size_t reserveTextures){
    m_Textures.reserve(reserveTextures);
}

void TextureAtlas::AddTexture(Texture* texture){
    m_Textures.push_back(texture);
}

int TextureAtlas::ToOpenGL(){
    
}

void TextureAtlas::MakeAtlas(){

}