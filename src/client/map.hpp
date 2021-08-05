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
#include "camera.hpp"

class ProvinceShape;
class Map {
private:
public:
    Map(const World& world);

    const World& world;
    std::vector<ProvinceShape> province_shapes;

    std::vector<const Texture*> unit_type_icons;
    std::vector<const Texture*> boat_type_icons;
    std::vector<const Texture*> outpost_type_icons;
    std::vector<const Texture*> nation_flags;

    void draw_flag(const Texture* flag, int x, int y);
    void draw(Camera& cam, const int width, const int height);

    // Wind oscillator (for flags)
    float wind_osc;
    
    Texture* topo_tex;
    const Texture* overlay_tex;
    GLuint coastline_gl_list;
};

class ProvinceShape {
public:
    ProvinceShape(const Map& map, const Province& base);
    GLuint shape_gl_list;
    GLuint outline_gl_list;
};

#endif
