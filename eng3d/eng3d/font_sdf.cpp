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
//      font_sdf.cpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <codecvt>
#include <locale>
#include <sstream>

#include "eng3d/font_sdf.hpp"
#include "eng3d/state.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/state.hpp"
#include "eng3d/camera.hpp"
#include "eng3d/log.hpp"
#include "eng3d/map.hpp"

Eng3D::Glyph::Glyph(float _advance, Eng3D::Rectangle _atlas_bounds, Eng3D::Rectangle _plane_bounds)
    : advance{ _advance },
    atlas_bounds{ _atlas_bounds },
    plane_bounds{ _plane_bounds }
{

}

Eng3D::FontSDF::FontSDF(const std::string& filename) {
    auto& s = Eng3D::State::get_instance();
    sphere_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        auto vs_shader = Eng3D::OpenGL::VertexShader(s.package_man.get_unique("shaders/sphere_mapping.vs")->read_all());
        sphere_shader->attach_shader(vs_shader);
        sphere_shader->attach_shader(*s.builtin_shaders["fs_font_sdf"].get());
        sphere_shader->link();
    }
    flat_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        flat_shader->attach_shader(*s.builtin_shaders["vs_font_sdf"].get());
        flat_shader->attach_shader(*s.builtin_shaders["fs_font_sdf"].get());
        flat_shader->link();
    }

    Eng3D::TextureOptions mipmap_options;
    mipmap_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR;
    mipmap_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
    mipmap_options.wrap_s = Eng3D::TextureOptions::Wrap::CLAMP_TO_EDGE;
    mipmap_options.wrap_t = Eng3D::TextureOptions::Wrap::CLAMP_TO_EDGE;
    mipmap_options.compressed = false;

    auto asset = s.package_man.get_unique(filename + ".png");
    atlas = s.tex_man.load(asset->abs_path, mipmap_options);
    
    std::string line;
    std::ifstream glyph_data(s.package_man.get_unique(filename + ".csv")->abs_path);
    if(glyph_data.is_open()) {
        while(std::getline(glyph_data, line)) {
			uint32_t unicode;
			float advance, top, bottom, left, right;
			char ch;
			
            std::istringstream data(line);
            data >> unicode >> ch;
            data >> advance >> ch;
            data >> left >> ch >> bottom >> ch >> right >> ch >> top >> ch;
            Eng3D::Rectangle plane_bounds(left, top, right - left, bottom - top);
            data >> left >> ch >> bottom >> ch >> right >> ch >> top;
            left /= atlas->width;
            right /= atlas->width;
            top /= atlas->height;
            bottom /= atlas->height;
            Eng3D::Rectangle atlas_bounds(left, top, right - left, bottom - top);
            Eng3D::Glyph glyph(advance, atlas_bounds, plane_bounds);
            unicode_map.insert({ unicode, glyph });
        }
    }
}

/// @brief Quadratic bezier curve, p1 is control
template<typename T>
constexpr T bezier(float t, const T p0, const T p1, const T p2) {
    return (1 - t) * ((1 - t) * p0 + t * p1) + t * ((1 - t) * p1 + t * p2);
}

std::unique_ptr<Eng3D::Label3D> Eng3D::FontSDF::gen_text(const std::string& text, glm::vec2 pmin, glm::vec2 pmax, glm::vec2 p0, float width) {
    assert(width > 0.f && width < 1.f);
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv_utf8_utf32;
    std::u32string unicode_text = conv_utf8_utf32.from_bytes(text);

    auto text_width = 0.f;
    for(const auto& character : unicode_text) {
        if(!unicode_map.count(character)) continue;
        const auto& glyph = unicode_map[character];
        text_width += glyph.advance;
    }
    
    if(text_width == 0.f) {
		std::vector<glm::vec3> positions;
		positions.emplace_back(0.f, 0.f, 0.f);
		positions.emplace_back(0.f, 0.f, 0.f);
		positions.emplace_back(0.f, 0.f, 0.f);
		std::vector<glm::vec2> tex_coords;
		tex_coords.emplace_back(0.f, 0.f);
		tex_coords.emplace_back(0.f, 0.f);
		tex_coords.emplace_back(0.f, 0.f);
		return std::make_unique<Eng3D::Label3D>(new Eng3D::TriangleList(positions, tex_coords), 0.f, glm::vec3(p0, 0.f));
	}

    glm::vec2 diff = (pmax - pmin) * (1.f - width);
    pmin += diff;
    pmax -= diff;
    float scale = glm::length(pmax - pmin) / text_width;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> tex_coords;
    float advance = 0.f;
    for(const auto& character : unicode_text) {
        if(!unicode_map.count(character)) continue;
        const auto& glyph = unicode_map.at(character);
        //glm::vec2 atlas_tl(glyph.atlas_bounds.left, glyph.atlas_bounds.top);
        //glm::vec2 atlas_bl(glyph.atlas_bounds.left, glyph.atlas_bounds.bottom);
        //glm::vec2 atlas_tr(glyph.atlas_bounds.right, glyph.atlas_bounds.top);
        //glm::vec2 atlas_br(glyph.atlas_bounds.right, glyph.atlas_bounds.bottom);
        glm::vec2 atlas_tl(glyph.atlas_bounds.left, glyph.atlas_bounds.bottom);
        glm::vec2 atlas_bl(glyph.atlas_bounds.left, glyph.atlas_bounds.top);
        glm::vec2 atlas_tr(glyph.atlas_bounds.right, glyph.atlas_bounds.bottom);
        glm::vec2 atlas_br(glyph.atlas_bounds.right, glyph.atlas_bounds.top);
        tex_coords.push_back(atlas_tl);
        tex_coords.push_back(atlas_br);
        tex_coords.push_back(atlas_bl);
        tex_coords.push_back(atlas_tr);
        tex_coords.push_back(atlas_br);
        tex_coords.push_back(atlas_tl);

        glm::vec2 plane_tl(glyph.plane_bounds.left, glyph.plane_bounds.top);
        glm::vec2 plane_bl(glyph.plane_bounds.left, glyph.plane_bounds.bottom);
        glm::vec2 plane_tr(glyph.plane_bounds.right, glyph.plane_bounds.top);
        glm::vec2 plane_br(glyph.plane_bounds.right, glyph.plane_bounds.bottom);

        const auto t0 = advance / text_width;
        advance += glyph.advance;
        glm::vec2 char_tl = bezier(t0, pmin, p0, pmax) + plane_tl * scale;
        glm::vec2 char_bl = bezier(t0, pmin, p0, pmax) + plane_bl * scale;
        glm::vec2 char_tr = bezier(t0, pmin, p0, pmax) + plane_tr * scale;
        glm::vec2 char_br = bezier(t0, pmin, p0, pmax) + plane_br * scale;

        positions.emplace_back(char_tl, 0.f);
        positions.emplace_back(char_br, 0.f);
        positions.emplace_back(char_bl, 0.f);
        positions.emplace_back(char_tr, 0.f);
        positions.emplace_back(char_br, 0.f);
        positions.emplace_back(char_tl, 0.f);
    }
    return std::make_unique<Eng3D::Label3D>(new Eng3D::TriangleList(positions, tex_coords), scale, glm::vec3(p0, 0.f));
}

void Eng3D::FontSDF::draw(const std::vector<std::unique_ptr<Label3D>>& labels, const Eng3D::Camera& camera, bool sphere) {
    auto& shader = sphere ? *sphere_shader : *flat_shader;
    shader.use();
    shader.set_uniform("projection", camera.get_projection());
    shader.set_uniform("view", camera.get_view());
    shader.set_uniform("map_size", camera.get_map_size());
    shader.set_uniform("model", glm::mat4(1));
    shader.set_texture(0, "atlas", *atlas);
    for(auto& label : labels) {
        if(label.get() == nullptr) continue;
        const auto size = label->size * (sphere ? 0.1f : 1.f);
        shader.set_uniform("center", label->center.x, label->center.y);
        shader.set_uniform("radius", Eng3D::GLOBE_RADIUS + 0.01f * size);
        shader.set_uniform("px_range", size * 0.3f);
        label->draw();
    }
}

Eng3D::Label3D::Label3D(Eng3D::TriangleList* _triangles, float _size, glm::vec3 _center)
    : size{ _size },
    center{ _center },
    triangles{ _triangles }
{

}

void Eng3D::Label3D::draw() {
    triangles->draw();
}
