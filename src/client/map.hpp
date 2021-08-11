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
#include <GL/glew.h>
#include <GL/gl.h>
#include "world.hpp"
#include "province.hpp"
#include "render/texture.hpp"
#include "camera.hpp"
#include "render/primitive.hpp"
#include "render/shader.hpp"

class ProvinceShape;
class Map {
private:
public:
    Map(const World& world);

    std::vector<const UnifiedRender::Texture*> unit_type_icons, boat_type_icons, outpost_type_icons, nation_flags;

    // Wind oscillator (for flags)
    float wind_osc = 0.f;

    const World& world;
    std::vector<ProvinceShape> province_shapes;
    
    UnifiedRender::Texture* div_topo_tex;
    UnifiedRender::Texture* div_sheet_tex;
    const UnifiedRender::Texture* water_tex;
    const UnifiedRender::Texture* noise_tex;
    UnifiedRender::OpenGl::PrimitiveSquare* map_quad;
    UnifiedRender::OpenGl::Program* map_shader;
    const UnifiedRender::Texture* overlay_tex;
    GLuint coastline_gl_list;

    void draw_flag(const Nation* nation, float x, float y);
    void draw(Camera& cam, const int width, const int height);
    void draw_old(Camera& cam, const int width, const int height);
};

class ProvinceShape {
public:
    ProvinceShape(const Map& map, const Province& base);
    GLuint shape_gl_list;
    GLuint outline_gl_list;
};

#endif
