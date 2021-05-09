#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H
#include <vector>
#include <map>
#include <utility>
#include "texture.hpp"
typedef std::pair<float,float> region;
class TextureAtlas{
public:
TextureAtlas(size_t reserveTextures = 0);
void MakeAtlas();
int ToOpenGL();
void AddTexture(Texture* texture);
private:
std::vector<Texture*> m_Textures;
Texture* m_Atlas;
std::map<Texture*,float> m_Regions;
std::map<Texture*,region> m_AtlasRegions;

};

#endif