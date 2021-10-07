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
#include "../world.hpp"
#include "../province.hpp"
#include "render/texture.hpp"
#include "camera.hpp"
#include "render/primitive.hpp"
#include "render/shader.hpp"
#include "render/texture_array.hpp"
#include "game_state.hpp"

class Map {
private:
public:
    Map(const World& world);

    std::vector<const UnifiedRender::ComplexModel*> unit_type_icons, outpost_type_icons;
    std::vector<const UnifiedRender::Texture*> nation_flags;

    // Wind oscillator (for flags)
    float wind_osc = 0.f;

    const World& world;
    
    UnifiedRender::Texture* div_topo_tex;
    UnifiedRender::Texture* div_sheet_tex;
    const UnifiedRender::Texture* water_tex;
    const UnifiedRender::Texture* noise_tex;
    const UnifiedRender::Texture* topo_tex;
    const UnifiedRender::Texture* terrain_tex;
    // const UnifiedRender::Texture* terrain_sheet;
    UnifiedRender::TextureArray* terrain_sheet;
    UnifiedRender::OpenGl::PrimitiveSquare* map_quad;
    UnifiedRender::OpenGl::Program* map_shader,* obj_shader;
    const UnifiedRender::Texture* overlay_tex;
    GLuint coastline_gl_list;
    GLuint frame_buffer;

    void update(World& world);
    void draw_flag(const Nation* nation);
    void draw(Camera& cam, const int width, const int height);
    void handle_click(GameState& gs, SDL_Event event);
};

#endif
