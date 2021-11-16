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
#include "client/render/sphere.hpp"
#include "client/render/shader.hpp"
#include "client/render/texture_array.hpp"
#include "client/game_state.hpp"
#include "client/render/quad_2d.hpp"
#include "client/render/framebuffer.hpp"

enum class MapView {
    SPHERE_VIEW,
    PLANE_VIEW,
};

struct Input;
class Map {
public:
    Map(const World& world);
    ~Map() {};

    std::vector<const UnifiedRender::ComplexModel*> unit_type_icons, outpost_type_icons;
    std::vector<const UnifiedRender::Texture*> nation_flags;

    // Wind oscillator (for flags)
    float wind_osc = 0.f;

    const World& world;
    Camera* camera;
    MapView view_mode = MapView::PLANE_VIEW;

    // Map textures
    UnifiedRender::Texture* tile_map;
    UnifiedRender::Texture* tile_sheet;
    UnifiedRender::Texture* border_tex;
    UnifiedRender::Texture* border_sdf;
    const UnifiedRender::Texture* water_tex;
    const UnifiedRender::Texture* noise_tex;
    const UnifiedRender::Texture* topo_tex;
    const UnifiedRender::Texture* terrain_tex;
    const UnifiedRender::Texture* map_color;
    UnifiedRender::TextureArray* terrain_sheet;
    // const UnifiedRender::Texture* terrain_sheet;
    

    UnifiedRender::OpenGl::PrimitiveSquare* map_quad;
    UnifiedRender::OpenGl::Sphere* map_sphere;
    UnifiedRender::OpenGl::Quad2D* map_2d_quad;
    UnifiedRender::OpenGl::Program* map_shader, * obj_shader, * border_sdf_shader, * border_gen_shader;
    const UnifiedRender::Texture* overlay_tex;
    GLuint coastline_gl_list;
    GLuint frame_buffer;
    UnifiedRender::OpenGl::Framebuffer* border_fbuffer;

    void update(const SDL_Event& event, Input& input);
    void update_tiles(World& world);
    void draw_flag(const Nation* nation);
    void draw(const int width, const int height);
    void handle_click(GameState& gs, SDL_Event event);
    void set_map_mode(std::vector<std::pair<Province::Id, uint32_t>> province_colors);
    void set_view(MapView view);
    void reload_shaders();
private:
    UnifiedRender::Texture* gen_border_sdf();
};

