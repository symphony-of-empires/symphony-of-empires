// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/map.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <cstddef>

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

#include "province.hpp"
#include "unified_render/color.hpp"

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
    UnifiedRender::Color color;
    ProvinceColor(Province::Id _id, UnifiedRender::Color _color): id{ _id }, color{ _color } {}
};
typedef std::function<std::string(const World& world, const Province::Id id)> mapmode_tooltip;
typedef std::function<std::vector<ProvinceColor>(const World& world)> mapmode_generator;
std::vector<ProvinceColor> political_map_mode(const World& world);

class Map {
    // Called to get mapmode
    mapmode_generator mapmode_func;
    // Called to get the provinces info to show in tooltip
    mapmode_tooltip mapmode_tooltip_func;
public:
    Map(const World& world, int screen_width, int screen_height);
    ~Map();

    void update(const SDL_Event& event, Input& input);
    void update_mapmode();
    void draw_flag(const UnifiedRender::OpenGL::Program& shader, const Nation& nation);
    void draw(const GameState& gs);
    void handle_click(GameState& gs, SDL_Event event);
    void set_map_mode(mapmode_generator mapmode_func);
    void set_view(MapView view);
    void reload_shaders();

    MapRender* map_render;
    MapView view_mode = MapView::PLANE_VIEW;

    std::vector<const UnifiedRender::Model*> building_type_models, unit_type_models;
    std::vector<const UnifiedRender::Texture*> building_type_icons;
    std::vector<const UnifiedRender::Texture*> unit_type_icons;
    std::vector<const UnifiedRender::Texture*> nation_flags;

    // Wind oscillator (for flags)
    float wind_osc = 0.f;
    // Input states
    bool is_drag = false;
    glm::vec2 last_camera_drag_pos;

    const World& world;
    Camera* camera;

#if !defined TILE_GRANULARITY
    UnifiedRender::Texture* id_map;
    UnifiedRender::Texture* province_color_tex;
    const UnifiedRender::Texture* line_tex;
#endif
    std::unique_ptr<UnifiedRender::OpenGL::Program> obj_shader;
};

