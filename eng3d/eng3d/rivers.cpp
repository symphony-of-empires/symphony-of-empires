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
//      rivers.cpp
//
// Abstract:
//      Functions for creating vertices for maps with rivers.
// ----------------------------------------------------------------------------

#include <glm/mat4x4.hpp>

#include "eng3d/rivers.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/state.hpp"
#include "eng3d/curve.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/camera.hpp"

/// @brief Construct a new Eng3D::Rivers object
/// @param _s Game state
/// @param lazy_init Whetever to postpone creation until later
Eng3D::Rivers::Rivers(Eng3D::State& _s, bool lazy_init)
    : s{ _s }
{
    Eng3D::TextureOptions mipmap_options{};
    mipmap_options.wrap_s = Eng3D::TextureOptions::Wrap::REPEAT;
    mipmap_options.wrap_t = Eng3D::TextureOptions::Wrap::REPEAT;
    mipmap_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
    mipmap_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
    mipmap_options.internal_format = Eng3D::TextureOptions::Format::SRGB;

    water_tex = s.tex_man.load(s.package_man.get_unique("gfx/water_tex.png"), mipmap_options);
    line_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        auto vs_shader = *s.builtin_shaders["vs_3d"].get();
        line_shader->attach_shader(vs_shader);
        auto fs_shader = Eng3D::OpenGL::FragmentShader(s.package_man.get_unique("shaders/curve.fs")->read_all(), true);
        line_shader->attach_shader(fs_shader);
        line_shader->link();
    }

    if(!lazy_init)
        this->build_rivers();
}

struct ConnectedNode {
    ConnectedNode* node = nullptr;
    std::unique_ptr<std::vector<glm::vec3>> river;
    ConnectedNode()
        : river{ new std::vector<glm::vec3> }
    {

    }

    ConnectedNode(const std::vector<glm::vec3>& _river)
        : river{ std::make_unique<std::vector<glm::vec3>>(_river) }
    {

    }

    ~ConnectedNode() = default;
};

void Eng3D::Rivers::get_river(std::vector<glm::vec3>& river, int current_index, int prev_index, uint32_t* pixels, int width, int height) {
    int x = current_index % width;
    int y = current_index / width;
    river.push_back(glm::vec3(x, y, -0.05));

    const auto check_neighbor = [this, current_index, prev_index, pixels, width, height](std::vector<glm::vec3>& _river, int new_x, int new_y) {
        if(new_x < 0 || new_y < 0 || new_x >= width || new_y >= height)
            return;
        int new_index = new_x + new_y * width;
        if(new_index == prev_index) return;

        uint32_t neighbor_color = pixels[new_index];
        if(neighbor_color == 0xFFFF0000) {
            this->get_river(_river, new_index, current_index, pixels, width, height);
            return;
        }
    };
    check_neighbor(river, x - 1, y + 0);
    check_neighbor(river, x + 1, y + 0);
    check_neighbor(river, x + 0, y + 1);
    check_neighbor(river, x + 0, y - 1);
}


void Eng3D::Rivers::build_rivers() {
    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    auto river_tex = s.tex_man.load(s.package_man.get_unique("map/river.png"), no_drop_options);

    std::vector<int> rivers_starts;
    auto pixels = river_tex->buffer.get();
    for(size_t y = 0; y < river_tex->height; y++)
        for(size_t x = 0; x < river_tex->width; x++)
            if(pixels[x + y * river_tex->width] == 0xff0000ff)
                rivers_starts.push_back(x + y * river_tex->width);

    // TODO FIX THIS NOT INFINITE LOOP
    for(size_t i = 0; i < rivers_starts.size(); i++) {
        std::vector<glm::vec3> river;
        this->get_river(river, rivers_starts[i], -1, pixels, river_tex->width, river_tex->height);

        const size_t length = river.size();
        if(length < 2) continue;

        std::vector<glm::vec3> mid_points(length + 3);
        mid_points[0] = river[0];
        mid_points[1] = river[0];
        for(size_t j = 0; j < length - 1; j++)
            mid_points[j + 2] = 0.5f * (river[j] + river[j + 1]);
        mid_points[length + 1] = river[length - 1];
        mid_points[length + 2] = river[length - 1];

        std::vector<glm::vec3> curve_points;

        // p0 = 2.f * river[0] - river[1];
        // p3 = 2.f * river[length - 1] - river[length - 2];
        glm::vec3 p0, p1, p2, p3;
        for(size_t j = 1; j < mid_points.size() - 2; j++) {
            p0 = mid_points[j - 1];
            p1 = mid_points[j];
            p2 = mid_points[j + 1];
            p3 = mid_points[j + 2];

            float step = 1 / 10.;
            for(float t = 1.f; t > 0.f; t -= step) {
                float t0 = t - 2;
                float t1 = t - 1;
                float t2 = t + 0;
                float t3 = t + 1;

                glm::vec3 pt(0, 0, 0);
                pt += p0 * (+1.f / 6.f * glm::pow(t0, 3.f) + 2.f * t0 + 4.f / 3.f + glm::pow(t0, 2.f));
                pt += p3 * (-1.f / 6.f * glm::pow(t3, 3.f) - 2.f * t3 + 4.f / 3.f + glm::pow(t3, 2.f));
                pt += p1 * (-1.f / 2.f * glm::pow(t1, 3.f) - glm::pow(t1, 2.f) + 2.f / 3.f);
                pt += p2 * (+1.f / 2.f * glm::pow(t2, 3.f) - glm::pow(t2, 2.f) + 2.f / 3.f);
                curve_points.push_back(pt);
            }
        }

        std::vector<glm::vec3> normals(curve_points.size() - 1, glm::vec3(0, 0, 1));
        auto curve = std::make_unique<Eng3D::Curve>(curve_points, normals, 1.0f);
        this->curves.push_back(std::move(curve));
    }
}

void Eng3D::Rivers::draw(const Eng3D::Camera& camera) {
    line_shader->use();
    
    glm::mat4 model(1.f);
    line_shader->set_uniform("model", model);
    line_shader->set_uniform("projection", camera.get_projection());
    line_shader->set_uniform("view", camera.get_view());
    line_shader->set_texture(0, "water_texture", *water_tex);
    for(auto& curve : curves)
        curve->draw();
}