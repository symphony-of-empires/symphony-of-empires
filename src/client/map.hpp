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

#include "province.hpp"
#include "client/color.hpp"

#include <vector>
#include <utility>
#include <functional>

enum class MapView {
    SPHERE_VIEW,
    PLANE_VIEW,
};

class World;
class Camera;
class Nation;
class MapRender;
class GameState;
union SDL_Event;
struct Input;

static const float GLOBE_RADIUS = 100.f;
struct ProvinceColor {
    Province::Id id;
    UI::Color color;
    ProvinceColor(Province::Id _id, UI::Color _color): id{ _id }, color{ _color } {}
};
typedef std::function<std::vector<ProvinceColor>(const World& world)> mapmode_generator;
std::vector<ProvinceColor> political_map_mode(const World& world);

class Map {
public:
    Map(const World& world, int screen_width, int screen_height);
    ~Map() {
        // delete map_render;
    };

    std::vector<const UnifiedRender::Model*> building_type_models, unit_type_models;
    std::vector<const UnifiedRender::Texture*> building_type_icons;
    std::vector<const UnifiedRender::Texture*> unit_type_icons;
    std::vector<const UnifiedRender::Texture*> nation_flags;

    // Wind oscillator (for flags)
    float wind_osc = 0.f;

    const World& world;
    Camera* camera;

#if !defined TILE_GRANULARITY
    UnifiedRender::Texture* id_map;
    UnifiedRender::Texture* province_color_tex;
#endif

    UnifiedRender::OpenGl::Program* obj_shader, * model_shader;

    void update(const SDL_Event& event, Input& input);
    void update_mapmode();
    void draw_flag(const Nation* nation);
    void draw(const GameState& gs, const int width, const int height);
    void handle_click(GameState& gs, SDL_Event event);
    void set_map_mode(mapmode_generator mapmode_func);
    void set_view(MapView view);
    void reload_shaders();
private:
    MapView view_mode = MapView::PLANE_VIEW;
    MapRender* map_render;
    // Called to get mapmode
    mapmode_generator mapmode_func;
};

