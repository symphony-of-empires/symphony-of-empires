#pragma once

#include <cstddef>

// Required before GL/gl.h
#include <GL/glew.h>
#include <GL/gl.h>

#include "world.hpp"
#include "province.hpp"
#include "client/render/texture.hpp"
#include "client/camera.hpp"
#include "client/render/primitive.hpp"
#include "client/render/shader.hpp"
#include "client/render/texture_array.hpp"
#include "client/game_state.hpp"

class Map {
public:
    Map(const World& world);
    ~Map() {};

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