// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      map.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <vector>
#include <glm/vec2.hpp>

#include "eng3d/borders.hpp"
#include "eng3d/rivers.hpp"
#include "eng3d/primitive.hpp"

class MapRender;
namespace Eng3D {
    constexpr static auto GLOBE_RADIUS = 100.f;

    class Texture;
    struct TextureArray;
    struct Square;
    class State;

    class BaseMap {
        Eng3D::State& s;

        std::shared_ptr<Eng3D::Texture> water_tex;
        std::shared_ptr<Eng3D::Texture> wave1;
        std::shared_ptr<Eng3D::Texture> wave2;
        std::shared_ptr<Eng3D::Texture> bathymethry;
        std::unique_ptr<Eng3D::TextureArray> terrain_sheet;
        std::unique_ptr<Eng3D::Texture> normal_topo;
        std::unique_ptr<Eng3D::Texture> terrain_map;
        std::shared_ptr<Eng3D::Texture> noise_tex;
        std::shared_ptr<Eng3D::Texture> paper_tex;
        std::shared_ptr<Eng3D::Texture> stripes_tex;
        Eng3D::Sphere map_sphere;
        Eng3D::Quad2D map_2d_quad; // Simple 2D quad that fills viewport, used for making the border_sdf
        std::vector<std::unique_ptr<Eng3D::Square>> map_quads;
        Eng3D::Rivers rivers;
        Eng3D::Borders borders;
    public:
        BaseMap(Eng3D::State& s, glm::ivec2 size);
        ~BaseMap() = default;
        friend MapRender;
    };
}
