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
    namespace OpenGL {
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
    std::unique_ptr<UnifiedRender::Texture> gen_border_sdf();
public:
    MapRender(const World& world);
    ~MapRender() {};
    void update_mapmode(std::vector<ProvinceColor> province_colors);
    void draw(Camera* camera, MapView view_mode);
    void reload_shaders();

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
    const UnifiedRender::Texture* water_tex;
    const UnifiedRender::Texture* noise_tex;
    const UnifiedRender::Texture* topo_map;
    const UnifiedRender::Texture* river_tex;
    UnifiedRender::Texture* terrain_map;
    const UnifiedRender::Texture* landscape_map;
    const UnifiedRender::Texture* wave1;
    const UnifiedRender::Texture* wave2;
    UnifiedRender::Texture* normal_topo;
    UnifiedRender::TextureArray* terrain_sheet;

    UnifiedRender::Square* map_quad;
    UnifiedRender::Sphere* map_sphere;
    UnifiedRender::Quad2D* map_2d_quad;

    std::unique_ptr<UnifiedRender::OpenGL::Program> map_shader;
    std::unique_ptr<UnifiedRender::OpenGL::Program> border_sdf_shader;
    std::unique_ptr<UnifiedRender::OpenGL::Program> border_gen_shader;
    std::unique_ptr<UnifiedRender::Texture> border_sdf;
};