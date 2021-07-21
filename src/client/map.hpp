#ifndef MAP_H
#define MAP_H

#include <cstddef>
#ifdef _MSC_VER
/* required before GL/gl.h */
#	ifndef _WINDOWS_
#		define WIN32_LEAN_AND_MEAN 1
#		include <windows.h>
#		undef WIN32_LEAN_AND_MEAN
#	endif
#endif
#include <GL/gl.h>
#include "world.hpp"
#include "province.hpp"
#include "texture.hpp"

class ProvinceShape;
class Map {
private:
public:
    Map(const World& world);

    const World& world;
    std::vector<ProvinceShape> province_shapes;
    void draw(float zoom);
    
    Texture* topo_tex;
    GLuint coastline_gl_list;
};

class ProvinceShape {
public:
    ProvinceShape(const Map& map, const Province& base);
    GLuint shape_gl_list;
};

#endif
