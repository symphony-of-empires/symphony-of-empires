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

#include <vector>
#include <memory>
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

typedef UnifiedRender::OpenGL::Option Option;
struct MapOptions {
    Option noise{ "NOISE", false };
    Option sdf{ "SDF", false };
    Option lighting{ "LIGHTING", false };
    Option diag_borders{ "DIAG_BORDER", false };
    Option parallax{ "PARALLAX", false };
    Option landscape{ "LANDSCAPE", false };
    Option rivers{ "RIVERS", false };
    Option water{ "WATER", false };
    Option grid{ "GRID", false };

    std::vector<Option> get_options() {
        return std::vector<Option>{
            noise, sdf, lighting, diag_borders, parallax, landscape, rivers, water, grid
        };
    }
};
class MapRender {
public:
    MapRender(const World& world);
    ~MapRender() {};
    void update_mapmode(std::vector<ProvinceColor> province_colors);
    void draw(Camera* camera, MapView view_mode);
    void reload_shaders();

    MapOptions options;
private:
    std::unique_ptr<UnifiedRender::Texture> gen_border_sdf();

    const World& world;

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
    const UnifiedRender::Texture* bathymethry;
    UnifiedRender::Texture* normal_topo;
    UnifiedRender::TextureArray* terrain_sheet;

    UnifiedRender::Square* map_quad;
    UnifiedRender::Sphere* map_sphere;
    UnifiedRender::Quad2D* map_2d_quad;

    std::unique_ptr<UnifiedRender::OpenGL::Program> map_shader;
    std::unique_ptr<UnifiedRender::OpenGL::Program> border_sdf_shader;
    std::unique_ptr<UnifiedRender::OpenGL::Program> border_gen_shader;
    std::unique_ptr<UnifiedRender::OpenGL::Program> output_shader;
    std::unique_ptr<UnifiedRender::Texture> border_sdf;
};