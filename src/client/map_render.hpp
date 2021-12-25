#pragma once

#include <cstddef>

// Required before GL/gl.h
#include <GL/glew.h>
#include <GL/gl.h>

namespace UnifiedRender {
    class Texture;
    class Model;
    class TextureArray;
    class Model;
    namespace OpenGl {
        class Square;
        class Sphere;
        class Program;
        class Quad2D;
        class Framebuffer;
    }
}

#include "unified_render/color.hpp"
#include "map.hpp"

#include <vector>
#include <utility>
#include <functional>

class World;
class Camera;
class Nation;
class GameState;
enum class MapView;
union SDL_Event;
struct Input;
struct ProvinceColors;

class MapRender {
public:
    MapRender(const World& world);
    ~MapRender() {

    };

    std::vector<const UnifiedRender::Model*> building_type_models, unit_type_models;
    std::vector<const UnifiedRender::Texture*> building_type_icons;
    std::vector<const UnifiedRender::Texture*> unit_type_icons;
    std::vector<const UnifiedRender::Texture*> nation_flags;

    // Wind oscillator (for flags)
    float wind_osc = 0.f;

    const World& world;

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
    const UnifiedRender::Texture* terrain_map;
    const UnifiedRender::Texture* landscape_map;
    const UnifiedRender::Texture* wave1;
    const UnifiedRender::Texture* wave2;
    const UnifiedRender::Texture* normal10;
    UnifiedRender::TextureArray* terrain_sheet;

    UnifiedRender::OpenGl::Square* map_quad;
    UnifiedRender::OpenGl::Sphere* map_sphere;
    UnifiedRender::OpenGl::Quad2D* map_2d_quad;

    UnifiedRender::OpenGl::Program* map_shader, * border_sdf_shader, * border_gen_shader;

    void update_mapmode(std::vector<ProvinceColor> province_colors);
    void draw(Camera* camera, MapView view_mode);
    void reload_shaders();
private:
    UnifiedRender::Texture* gen_border_sdf();
};