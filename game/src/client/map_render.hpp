// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

namespace Eng3D {
    class Texture;
    class Model;
    class TextureArray;
    class Model;
    class Square;
    class Sphere;
    class Quad2D;
    class Camera;
    namespace OpenGL {
        class Program;
        class Framebuffer;
    }
}

#include "eng3d/texture.hpp"
#include "eng3d/color.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/map.hpp"

class World;
class Nation;
class GameState;
enum class MapView;
class Input;
class ProvinceColors;
class Map;

typedef Eng3D::OpenGL::Option Option;
class MapOptions {
public:
    Option noise{ "NOISE", true }; // Randomization for more variety
    Option sdf{ "SDF", false }; // Pretty shadow borders
    Option lighting{ "LIGHTING", true }; // Lights and reflections
    Option city_lights{ "CITY_LIGHTS", false }; // Lights and reflections
    Option parallax{ "PARALLAX", false }; // Parallax (3D) topography
    Option rivers{ "RIVERS", true }; // Overlaid rivers
    Option water{ "WATER", true }; // Textured water
    Option grid{ "GRID", true }; // Square grid for the map
    Option units{ "UNITS", false }; // 3D units
    Option buildings{ "BUILDINGS", false }; // 3D buildings
    Option trees{ "TREES", false }; // 3D hyper realistic trees
    Option compress{ "COMPRESS", false }; // Use compression on textures

    std::vector<Option> get_options() {
        return std::vector<Option>{
            noise, sdf, lighting, city_lights, parallax, rivers, water, grid, units, trees, compress
        };
    }
};

class MapRender : public Eng3D::BaseMap {
    void update_visibility(GameState& gs);
    void update_city_lights();

    GameState& gs;
    Map& map;

    // Map textures
    std::shared_ptr<Eng3D::Texture> river_tex;

    std::unique_ptr<Eng3D::Texture> tile_sheet;
    std::unique_ptr<Eng3D::Texture> tile_sheet_nation;
    std::unique_ptr<Eng3D::Texture> province_opt;
    std::unique_ptr<Eng3D::Texture> border_sdf;

    std::unique_ptr<Eng3D::OpenGL::Program> map_shader;
    std::unique_ptr<Eng3D::OpenGL::Program> sdf_shader;
    std::unique_ptr<Eng3D::OpenGL::Program> border_gen_shader;
    std::unique_ptr<Eng3D::OpenGL::Program> output_shader;

    std::atomic<bool> req_update_vision = true;
public:
    MapRender(GameState& gs, Map& map);
    ~MapRender() = default;
    void update_mapmode(std::vector<ProvinceColor>& province_colors);
    void update_nations(std::vector<ProvinceId>& nations);
    void request_update_visibility();
    void draw(Eng3D::Camera* camera, MapView view_mode);
    void reload_shaders();
    void update_options(MapOptions options);
    void update(GameState& gs);

    MapOptions options;
    void update_border_sdf(Eng3D::Rect update_area, glm::ivec2 window_size);
    inline uint32_t get_province_opt(const ProvinceId id) {
        return this->province_opt->buffer[id];
    }

    inline ProvinceId get_tile_province_id(size_t x, size_t y) {
        return ProvinceId(this->terrain_map->buffer.get()[x + y * this->terrain_map->width] & 0xffff);
    }
};
