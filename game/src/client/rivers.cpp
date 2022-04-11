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
//      client/rivers.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/rivers.hpp"

#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/state.hpp"
#include "unified_render/curve.hpp"
#include "unified_render/shader.hpp"
#include "client/camera.hpp"

#include "glm/mat4x4.hpp"

Rivers::Rivers() {
    UnifiedRender::TextureOptions mipmap_options{};
    mipmap_options.wrap_s = GL_REPEAT;
    mipmap_options.wrap_t = GL_REPEAT;
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    mipmap_options.internal_format = GL_SRGB;

    water_tex = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/water_tex.png"), mipmap_options);
    line_shader = std::unique_ptr<UnifiedRender::OpenGL::Program>(new UnifiedRender::OpenGL::Program());
    {
        line_shader->attach_shader(UnifiedRender::State::get_instance().builtin_shaders["vs_3d"].get());
        auto fs_shader = UnifiedRender::OpenGL::FragmentShader(Path::cat_strings(Path::get_data("shaders/curve.fs")), true);
        line_shader->attach_shader(&fs_shader);
        line_shader->link();
    }
    build_rivers();
}

class ConnectedNode {
public:
    ConnectedNode* node = nullptr;
    std::vector<glm::vec3>* river;

    ConnectedNode(): river{ new std::vector<glm::vec3> } {}
    ConnectedNode(std::vector<glm::vec3>* _river): river{ _river } {}
};

void get_river(std::vector<glm::vec3>& river, int current_index, int prev_index, uint32_t* pixels, int width, int height) {
    int x = current_index % width;
    int y = current_index / width;
    river.push_back(glm::vec3(x, y, -0.05));

    const auto check_neighbor = [current_index, prev_index, pixels, width, height](std::vector<glm::vec3>& river, int new_x, int new_y) {
        if(new_x < 0 || new_y < 0 || new_x >= width || new_y >= height) {
            return;
        }
        int new_index = new_x + new_y * width;
        if(new_index == prev_index) return;

        uint32_t neighbor_color = pixels[new_index];
        if(neighbor_color == 0xFFFF0000) {
            get_river(river, new_index, current_index, pixels, width, height);
            return;
        }
    };
    check_neighbor(river, x - 1, y + 0);
    check_neighbor(river, x + 1, y + 0);
    check_neighbor(river, x + 0, y + 1);
    check_neighbor(river, x + 0, y - 1);
}


void Rivers::build_rivers() {
    auto tex_man = UnifiedRender::State::get_instance().tex_man;
    UnifiedRender::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    const UnifiedRender::Texture& river_tex = tex_man->load(Path::get("map/river.png"), no_drop_options);

    std::vector<int> rivers_starts;
    int height = river_tex.height;
    int width = river_tex.width;
    auto pixels = river_tex.buffer.get();
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            int curr_index = x + y * river_tex.width;
            uint32_t color = pixels[curr_index];
            if(color == 0xFF0000FF) {
                rivers_starts.push_back(x + y * width);
            }
        }
    }

    // TODO FIX THIS NOT INFINITE LOOP
    for(size_t i = 0; i < rivers_starts.size(); i++) {
        std::vector<glm::vec3> river;
        get_river(river, rivers_starts[i], -1, pixels, width, height);

        size_t length = river.size();
        if(length < 2) continue;

        std::vector<glm::vec3> mid_points(length + 3);
        mid_points[0] = river[0];
        mid_points[1] = river[0];
        for(size_t j = 0; j < length - 1; j++) {
            mid_points[j + 2] = 0.5f * (river[j] + river[j + 1]);
        }
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
        auto* curve = new UnifiedRender::Curve(curve_points, normals, 1.0f);
        this->curves.push_back(curve);
    }
}

Rivers::~Rivers() {

}

void Rivers::draw(Camera* camera) {
    line_shader->use();
    glm::mat4 model(1.f);
    line_shader->set_uniform("model", model);
    line_shader->set_uniform("projection", camera->get_projection());
    line_shader->set_uniform("view", camera->get_view());
    line_shader->set_texture(0, "water_texture", *water_tex);
    for(auto curve : curves) {
        curve->draw();
    }
}