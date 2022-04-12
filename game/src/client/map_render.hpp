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
//      client/map_render.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <atomic>

namespace UnifiedRender {
    class Texture;
    class Model;
    class TextureArray;
    class Model;
    class Square;
    class Sphere;
    class Quad2D;
    namespace OpenGL {
        class Program;
        class Framebuffer;
    }
}

#include "unified_render/color.hpp"
#include "unified_render/shader.hpp"

class World;
class Camera;
class Nation;
class GameState;
enum class MapView;
union SDL_Event;
class Input;
class ProvinceColors;

typedef UnifiedRender::OpenGL::Option Option;
class MapOptions {
public:
    Option noise{ "NOISE", true };
    Option sdf{ "SDF", true };
    Option lighting{ "LIGHTING", true };
    Option parallax{ "PARALLAX", false };
    Option rivers{ "RIVERS", true };
    Option water{ "WATER", true };
    Option grid{ "GRID", true };

    std::vector<Option> get_options() {
        return std::vector<Option>{
            noise, sdf, lighting, parallax, rivers, water, grid
        };
    }
};

class MapRender {
public:
    MapRender(const World& world);
    ~MapRender() {};
    void update_mapmode(std::vector<ProvinceColor> province_colors);
    void update_nations(std::vector<Province*> nations);
    void request_update_visibility(void);
    void update_visibility(void);
    void draw(Camera* camera, MapView view_mode);
    void reload_shaders();
    void update_options(MapOptions options);

    MapOptions options;
private:
    void update_border_sdf(UnifiedRender::Rect update_area);

    const World& world;

    // Map textures
    std::shared_ptr<UnifiedRender::Texture> water_tex;
    std::shared_ptr<UnifiedRender::Texture> paper_tex;
    std::shared_ptr<UnifiedRender::Texture> stripes_tex;
    std::shared_ptr<UnifiedRender::Texture> noise_tex;
    std::shared_ptr<UnifiedRender::Texture> river_tex;
    std::shared_ptr<UnifiedRender::Texture> wave1;
    std::shared_ptr<UnifiedRender::Texture> wave2;
    std::shared_ptr<UnifiedRender::Texture> bathymethry;

    std::unique_ptr<UnifiedRender::Texture> tile_sheet;
    std::unique_ptr<UnifiedRender::Texture> tile_sheet_nation;
    std::unique_ptr<UnifiedRender::TextureArray> terrain_sheet;
    std::unique_ptr<UnifiedRender::Texture> normal_topo;
    std::unique_ptr<UnifiedRender::Texture> province_opt;
    std::unique_ptr<UnifiedRender::Texture> tile_map;
    std::unique_ptr<UnifiedRender::Texture> topo_map;
    std::unique_ptr<UnifiedRender::Texture> terrain_map;
    std::unique_ptr<UnifiedRender::Texture> border_sdf;

    std::vector<UnifiedRender::Square*> map_quads;
    UnifiedRender::Sphere* map_sphere;
    UnifiedRender::Quad2D* map_2d_quad;

    std::unique_ptr<UnifiedRender::OpenGL::Program> map_shader;
    std::unique_ptr<UnifiedRender::OpenGL::Program> sdf_shader;
    std::unique_ptr<UnifiedRender::OpenGL::Program> border_gen_shader;
    std::unique_ptr<UnifiedRender::OpenGL::Program> output_shader;

    std::atomic<bool> req_update_vision = true;
};