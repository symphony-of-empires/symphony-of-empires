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
//      map.cpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>
#include "eng3d/map.hpp"
#include "eng3d/state.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/utils.hpp"

Eng3D::BaseMap::BaseMap(Eng3D::State& _s, glm::ivec2 size)
    : s{ _s },
    map_sphere(0.f, 0.f, 0.f, Eng3D::GLOBE_RADIUS, 100),
    map_2d_quad{},
    rivers(_s),
    borders(_s)
{
    for(int x = -1; x <= 1; x++) { // Flat surface for drawing flat map 
        auto square = std::make_unique<Eng3D::Square>(size.x * x, 0.f, size.x * (x + 1), size.y);
        map_quads.push_back(std::move(square));
    }

    // Mipmapped textures
    Eng3D::TextureOptions mipmap_options{};
    mipmap_options.wrap_s = Eng3D::TextureOptions::Wrap::REPEAT;
    mipmap_options.wrap_t = Eng3D::TextureOptions::Wrap::REPEAT;
    mipmap_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
    mipmap_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
    mipmap_options.compressed = true;

    this->noise_tex = this->s.tex_man.load(this->s.package_man.get_unique("gfx/noise_tex.png"), mipmap_options);
    this->wave1 = this->s.tex_man.load(this->s.package_man.get_unique("gfx/wave1.png"), mipmap_options);
    this->wave2 = this->s.tex_man.load(this->s.package_man.get_unique("gfx/wave2.png"), mipmap_options);

    mipmap_options.internal_format = Eng3D::TextureOptions::Format::SRGB;
    this->water_tex = this->s.tex_man.load(this->s.package_man.get_unique("gfx/water_tex.png"), mipmap_options);
    this->paper_tex = this->s.tex_man.load(this->s.package_man.get_unique("gfx/paper.png"), mipmap_options);
    this->stripes_tex = this->s.tex_man.load(this->s.package_man.get_unique("gfx/stripes.png"), mipmap_options);

    this->terrain_map = std::make_unique<Eng3D::Texture>(this->s.package_man.get_unique("map/color.png")->abs_path);

    tbb::parallel_for(static_cast<size_t>(0), this->terrain_map->width * this->terrain_map->height, [this](const auto i) {
        auto* data = &(this->terrain_map->buffer.get()[i]);
        const auto color = std::byteswap<std::uint32_t>((*data) << 8);
        uint8_t idx = 0;
        switch(color) {
        case 0x18200b:
            idx = 0 * 16;
            break;
        case 0x4b482a:
            idx = 1 * 16;
            break;
        case 0x273214:
            idx = 3 * 16;
            break;
        case 0x9c8461:
        case 0xbfa178:
            idx = 6 * 16;
            break;
        case 0x969a9a:
        case 0x686963:
        case 0xffffff:
            idx = 8 * 16;
            break;
        case 0x614a2f:
            idx = 9 * 16;
            break;
        case 0x37311b:
        case 0x7a6342:
            idx = 10 * 16;
            break;
        default:
            idx = 0;
            break;
        }
        *data = idx << 24;
        *data |= (color == 0x243089 ? 0x00 : 0x02) << 16; // Ocean, or ocean
    });
    //this->terrain_map->to_file("new_terrain.png");

    // Terrain textures to sample from
    this->terrain_sheet = std::make_unique<Eng3D::TextureArray>(this->s.package_man.get_unique("gfx/terrain_sheet.png")->abs_path, 4, 4);
    this->terrain_sheet->upload();
}
