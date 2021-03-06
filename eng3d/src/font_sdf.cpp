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
#include "eng3d/path.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/state.hpp"
#include "eng3d/camera.hpp"

using namespace Eng3D;

Eng3D::Glyph::Glyph(float _advance, Eng3D::Rectangle _atlas_bounds, Eng3D::Rectangle _plane_bounds)
    : advance(_advance), atlas_bounds(_atlas_bounds), plane_bounds(_plane_bounds)
{

}

Eng3D::FontSDF::FontSDF(const std::string& filename) {
    auto& s = Eng3D::State::get_instance();
    sphere_shader = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        auto vs_shader = Eng3D::OpenGL::VertexShader(s.package_man.get_unique("shaders/sphere_mapping.vs")->read_all());
        sphere_shader->attach_shader(vs_shader);
        sphere_shader->attach_shader(*s.builtin_shaders["fs_font_sdf"].get());
        sphere_shader->link();
    }
    flat_shader = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        flat_shader->attach_shader(*s.builtin_shaders["vs_font_sdf"].get());
        flat_shader->attach_shader(*s.builtin_shaders["fs_font_sdf"].get());
        flat_shader->link();
    }

    Eng3D::TextureOptions mipmap_options;
    mipmap_options.min_filter = GL_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    mipmap_options.wrap_s = GL_CLAMP_TO_EDGE;
    mipmap_options.wrap_t = GL_CLAMP_TO_EDGE;
    mipmap_options.compressed = false;

    auto asset = s.package_man.get_unique(filename + ".png");
    atlas = s.tex_man.load(asset->abs_path, mipmap_options);

    char buff;
    uint32_t unicode;
    float advance, top, bottom, left, right;
    std::string line;
    std::ifstream glyph_data(s.package_man.get_unique(filename + ".csv")->abs_path);
    if(glyph_data.is_open()) {
        while(std::getline(glyph_data, line)) {
            std::istringstream data(line);
            data >> unicode >> buff;
            data >> advance >> buff;
            data >> left >> buff >> bottom >> buff >> right >> buff >> top >> buff;
            Rectangle plane_bounds(left, top, right - left, bottom - top);
            data >> left >> buff >> bottom >> buff >> right >> buff >> top;
            left /= atlas->width;
            right /= atlas->width;
            top /= atlas->height;
            bottom /= atlas->height;
            Rectangle atlas_bounds(left, top, right - left, bottom - top);
            Glyph glyph(advance, atlas_bounds, plane_bounds);
            unicode_map.insert({ unicode, glyph });
        }
    }
}

Label3D* Eng3D::FontSDF::gen_text(const std::string& text, glm::vec3 top, glm::vec3 right, float width, glm::vec3 center) {
    Eng3D::Color color(0.f, 0.f, 0.f);

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv_utf8_utf32;
    std::u32string unicode_text = conv_utf8_utf32.from_bytes(text);

    float text_width = 0.f;
    for(char32_t& character : unicode_text) {
        if(!unicode_map.count(character)) continue;
        Eng3D::Glyph& glyph = unicode_map.at(character);
        text_width += glyph.advance;
    }

    if(text_width == 0.f)
        CXX_THROW(std::runtime_error, "Empty text label");

    float scale = width / text_width;
    top = glm::normalize(top);
    right = glm::normalize(right);
    glm::vec3 start = center - right * width * 0.5f;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> tex_coords;

    int idx = 1;
    for(char32_t& character : unicode_text) {
        if(!unicode_map.count(character)) continue;

        const auto& glyph = unicode_map.at(character);

        glm::vec2 atlas_tl(glyph.atlas_bounds.left, glyph.atlas_bounds.top);
        glm::vec2 atlas_bl(glyph.atlas_bounds.left, glyph.atlas_bounds.bottom);
        glm::vec2 atlas_tr(glyph.atlas_bounds.right, glyph.atlas_bounds.top);
        glm::vec2 atlas_br(glyph.atlas_bounds.right, glyph.atlas_bounds.bottom);

        glm::vec2 plane_tl(glyph.plane_bounds.left, glyph.plane_bounds.top);
        glm::vec2 plane_bl(glyph.plane_bounds.left, glyph.plane_bounds.bottom);
        glm::vec2 plane_tr(glyph.plane_bounds.right, glyph.plane_bounds.top);
        glm::vec2 plane_br(glyph.plane_bounds.right, glyph.plane_bounds.bottom);

        const auto base = glm::mat2x3(right, top) * scale;
        glm::vec3 char_tl = start + base * plane_tl;
        glm::vec3 char_bl = start + base * plane_bl;
        glm::vec3 char_tr = start + base * plane_tr;
        glm::vec3 char_br = start + base * plane_br;

        tex_coords.push_back(atlas_tl);
        positions.push_back(char_tl);
        tex_coords.push_back(atlas_br);
        positions.push_back(char_br);
        tex_coords.push_back(atlas_bl);
        positions.push_back(char_bl);
        tex_coords.push_back(atlas_tr);
        positions.push_back(char_tr);
        tex_coords.push_back(atlas_br);
        positions.push_back(char_br);
        tex_coords.push_back(atlas_tl);
        positions.push_back(char_tl);

        start.x += right.x * glyph.advance * scale;
        start.y += right.y * glyph.advance * scale;
        idx++;
    }

    TriangleList* triangles = new TriangleList(positions, tex_coords);
    return new Label3D(triangles, scale, center);
}

void Eng3D::FontSDF::draw(const std::vector<Label3D*>& labels, Camera* camera, bool sphere) {
    auto* shader = sphere ? sphere_shader.get() : flat_shader.get();
    shader->use();
    shader->set_uniform("projection", camera->get_projection());
    shader->set_uniform("view", camera->get_view());
    shader->set_texture(0, "atlas", *atlas);
    for(auto& label : labels) {
        shader->set_uniform("map_size", camera->get_map_size());
        shader->set_uniform("center", label->center.x, label->center.y);
        shader->set_uniform("radius", 100.f + 0.01f * label->size);
        shader->set_uniform("model", glm::mat4(1));
        shader->set_uniform("px_range", label->size * 0.5f);
        label->draw();
    }
}

Eng3D::Label3D::Label3D(TriangleList* _triangles, float _size, glm::vec3 _center)
    : size{ _size },
    center{ _center },
    triangles{ _triangles }
{

}

Eng3D::Label3D::~Label3D() {
    delete triangles;
}

void Eng3D::Label3D::draw() {
    triangles->draw();
}