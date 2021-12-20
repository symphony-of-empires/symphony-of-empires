#pragma once

#include <cstddef>

// Required before GL/gl.h
#include <GL/glew.h>
#include <GL/gl.h>

namespace UnifiedRender {
    class Texture;
    class Model;
    class TextureArray;
    class ComplexModel;
    namespace OpenGl {
        class Square;
        class Sphere;
        class Program;
        class Quad2D;
        class Framebuffer;
    }
}
#include "province.hpp"

#include <vector>
#include <utility>

enum class MapView {
    SPHERE_VIEW,
    PLANE_VIEW,
};

class World;
class Camera;
class Nation;
class GameState;
union SDL_Event;
struct Input;
class Map {
public:
    Map(const World& world, int screen_width, int screen_height);
    ~Map() {};

    std::vector<const UnifiedRender::Model*> building_type_models, unit_type_models;
    std::vector<const UnifiedRender::Texture*> building_type_icons;
    std::vector<const UnifiedRender::Texture*> unit_type_icons;
    std::vector<const UnifiedRender::Texture*> nation_flags;

    // Wind oscillator (for flags)
    float wind_osc = 0.f;

    const World& world;
    Camera* camera;
    MapView view_mode = MapView::PLANE_VIEW;

#if !defined TILE_GRANULARITY
    UnifiedRender::Texture* id_map;
    UnifiedRender::Texture* province_color_tex;
#endif

    // Map textures
    UnifiedRender::Texture* tile_map;
    UnifiedRender::Texture* tile_sheet;
    UnifiedRender::Texture* border_tex;
    UnifiedRender::Texture* border_sdf;
    const UnifiedRender::Texture* water_tex;
    const UnifiedRender::Texture* noise_tex;
    const UnifiedRender::Texture* topo_map;
    const UnifiedRender::Texture* river_tex;
    UnifiedRender::Texture* terrain_tex;
    const UnifiedRender::Texture* landscape_map;
    UnifiedRender::TextureArray* terrain_sheet;
    // const UnifiedRender::Texture* terrain_sheet;

    UnifiedRender::OpenGl::Square* map_quad;
    UnifiedRender::OpenGl::Sphere* map_sphere;
    UnifiedRender::OpenGl::Quad2D* map_2d_quad;
    UnifiedRender::OpenGl::Program* map_shader, * obj_shader, * border_sdf_shader, * border_gen_shader, * model_shader;
    const UnifiedRender::Texture* overlay_tex;
    GLuint coastline_gl_list;
    GLuint frame_buffer;
    UnifiedRender::Model* ourModel;
    UnifiedRender::OpenGl::Framebuffer* border_fbuffer;

    void update(const SDL_Event& event, Input& input);
    void update_provinces(const std::vector<Province*>* provinces);
    void draw_flag(const Nation* nation);
    void draw(const GameState& gs, const int width, const int height);
    void handle_click(GameState& gs, SDL_Event event);
    void set_map_mode(std::vector<std::pair<Province::Id, uint32_t>> province_colors);
    void set_view(MapView view);
    void reload_shaders();
private:
    UnifiedRender::Texture* gen_border_sdf();
};

