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
//      borders.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <unordered_set>
#include <stack>
#include <glm/mat4x4.hpp>

#include "eng3d/borders.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/state.hpp"
#include "eng3d/curve.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/camera.hpp"

/// @brief Construct a new Eng 3D::Borders object
/// @param _s Game state
/// @param lazy_init Whetever to postpone creation until later
Eng3D::Borders::Borders(Eng3D::State& _s, bool lazy_init)
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
        auto vs_shader = *s.builtin_shaders["vs_3d"];
        line_shader->attach_shader(vs_shader);
        auto fs_shader = Eng3D::OpenGL::FragmentShader(s.package_man.get_unique("shaders/curve.fs")->read_all(), true);
        line_shader->attach_shader(fs_shader);
        line_shader->link();
    }
    
    if(!lazy_init)
        this->build_borders();
}

class BorderGenerator {
    std::unordered_set<int> walked_positions;
    std::unordered_set<int> walked_paths;
    std::stack<int> unexplored_paths;
    std::stack<int> current_paths;
    std::vector<std::vector<glm::vec3>>& borders;
    const uint32_t* pixels;
    int width;
    int height;
    BorderGenerator(std::vector<std::vector<glm::vec3>>& _borders, const uint32_t* _pixels, int _width, int _height)
        : borders{ _borders },
        pixels{ _pixels },
        width{ _width },
        height{ _height }
    {

    }

    bool check_neighbor(int new_x, int new_y) {
        if(new_x < 0 || new_y < 0 || new_x >= width - 1 || new_y >= height - 1)
            return false;
        int new_index = new_x + new_y * width;
        const auto color_ul = pixels[new_index];
        const auto color_dl = pixels[new_index + width];
        const auto color_ur = pixels[new_index + 1];
        const auto color_dr = pixels[new_index + width + 1];
        // Different neighbor, ie its a border
        if(color_ul != color_ur || color_ur != color_dr || color_dr != color_dl || color_dl != color_ul)
            return true;
        return false;
    }

    void add_neighbor(int prev_x, int prev_y, int new_x, int new_y, int& connections) {
        if(check_neighbor(new_x, new_y)) {
            int old_index = prev_x + prev_y * width;
            int new_index = new_x + new_y * width;
            int index = 2 * glm::min<int>(old_index, new_index) + std::abs(prev_x - new_x);
            if(walked_paths.count(index)) return;
            walked_positions.insert(new_index);
            if(connections++ > 1) {
                unexplored_paths.push(old_index);
            } else {
                current_paths.push(new_index);
                walked_paths.insert(index);
            }
        }
    }

    void get_border(int current_index, int connections) {
        int x = current_index % width;
        int y = current_index / width;
        auto& current_river = borders.back();
        current_river.push_back(glm::vec3(x + 1.f, y + 1.f, -0.05));

        add_neighbor(x, y, x - 1, y + 0, connections);
        add_neighbor(x, y, x + 1, y + 0, connections);
        add_neighbor(x, y, x + 0, y + 1, connections);
        add_neighbor(x, y, x + 0, y - 1, connections);
    }

    void clear_stack() {
        while(!current_paths.empty() || !unexplored_paths.empty()) {
            while(!current_paths.empty()) {
                auto current_path = current_paths.top();
                current_paths.pop();
                get_border(current_path, 1);
            }

            if(!unexplored_paths.empty()) {
                current_paths.push(unexplored_paths.top());
                unexplored_paths.pop();
                borders.push_back(std::vector<glm::vec3>());
            }
        }
        borders.push_back(std::vector<glm::vec3>());
    }


public:
    static void build_borders(std::vector<std::vector<glm::vec3>>& borders, const uint32_t* pixels, int width, int height) {
        BorderGenerator generator(borders, pixels, width, height);
        borders.push_back(std::vector<glm::vec3>());
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                int curr_index = x + y * width;
                if(generator.check_neighbor(x, y) && !generator.walked_positions.count(curr_index)) {
                    generator.get_border(curr_index, 1);
                    generator.clear_stack();
                }
            }
        }
    }
};

void Eng3D::Borders::build_borders() {
    auto border_tex = std::make_unique<Eng3D::BinaryImage>(s.package_man.get_unique("map/provinces.png")->get_abs_path());
    int height = border_tex->height;
    int width = border_tex->width;
    auto pixels = border_tex->buffer.get();
    std::vector<std::vector<glm::vec3>> borders;
    BorderGenerator::build_borders(borders, pixels, width, height);

    // TODO FIX THIS NOT INFINITE LOOP
    auto curve = std::make_unique<Eng3D::Curve>();
    for(size_t i = 0; i < borders.size(); i++) {
        std::vector<glm::vec3> river = borders[i];
        auto length = river.size();
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
            float step = 1 / 1.;
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
        curve->add_line(curve_points, normals, 1.0f);
    }
    curve->upload();
    this->curves.push_back(std::move(curve));
}

void Eng3D::Borders::draw(const Eng3D::Camera& camera) {
    line_shader->use();
    glm::mat4 model(1.f);
    line_shader->set_uniform("model", model);
    line_shader->set_uniform("projection", camera.get_projection());
    line_shader->set_uniform("view", camera.get_view());
    line_shader->set_texture(0, "water_texture", *water_tex);
    for(auto& curve : curves)
        curve->draw();
}
