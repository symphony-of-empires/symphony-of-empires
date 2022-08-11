// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      map.cpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/map.hpp"
#include "eng3d/state.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/texture.hpp"

Eng3D::BaseMap::BaseMap(Eng3D::State& _s, glm::ivec2 size)
    : s{ _s }
{
    // Flat surface for drawing flat map 
    for(int x = -1; x <= 1; x++)
        map_quads.push_back(new Eng3D::Square(size.x * x, 0.f, size.x * (x + 1), size.y));

    // Sphere surface for drawing globe map
    map_sphere = new Eng3D::Sphere(0.f, 0.f, 0.f, Eng3D::GLOBE_RADIUS, 100);

    // Simple 2D quad that fills viewport, used for making the border_sdf
    map_2d_quad = new Eng3D::Quad2D();

    // Mipmapped textures
    Eng3D::TextureOptions mipmap_options{};
    mipmap_options.wrap_s = GL_REPEAT;
    mipmap_options.wrap_t = GL_REPEAT;
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    mipmap_options.compressed = true;

    this->noise_tex = this->s.tex_man.load(this->s.package_man.get_unique("gfx/noise_tex.png"), mipmap_options);
    this->wave1 = this->s.tex_man.load(this->s.package_man.get_unique("gfx/wave1.png"), mipmap_options);
    this->wave2 = this->s.tex_man.load(this->s.package_man.get_unique("gfx/wave2.png"), mipmap_options);

    mipmap_options.internal_format = GL_SRGB;
    this->water_tex = this->s.tex_man.load(this->s.package_man.get_unique("gfx/water_tex.png"), mipmap_options);
    this->paper_tex = this->s.tex_man.load(this->s.package_man.get_unique("gfx/paper.png"), mipmap_options);
    this->stripes_tex = this->s.tex_man.load(this->s.package_man.get_unique("gfx/stripes.png"), mipmap_options);

    this->terrain_map = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(this->s.package_man.get_unique("map/color.png")->get_abs_path()));
    size_t terrain_map_size = this->terrain_map->width * this->terrain_map->height;
    for(size_t i = 0; i < terrain_map_size; i++) {
        const uint32_t color = bswap32(this->terrain_map->buffer.get()[i] << 8);
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

        this->terrain_map->buffer.get()[i] = idx << 8;
        this->terrain_map->buffer.get()[i] |= (color == 0x243089) ? 0x00 : 0x02; // Ocean, or ocean
    }

    Eng3D::TextureOptions single_color{};
    single_color.internal_format = GL_RGBA;
    single_color.compressed = false;
    this->terrain_map->upload(single_color);

    // Terrain textures to sample from
    this->terrain_sheet = std::unique_ptr<Eng3D::TextureArray>(new Eng3D::TextureArray(this->s.package_man.get_unique("gfx/terrain_sheet.png")->get_abs_path(), 4, 4));
    this->terrain_sheet->upload();
}

Eng3D::BaseMap::~BaseMap() {
    delete map_sphere;
    delete map_2d_quad;
    for(auto quad : map_quads)
        delete quad;
    map_quads.clear();
}
