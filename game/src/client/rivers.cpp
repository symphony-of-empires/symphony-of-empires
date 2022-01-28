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

    auto tex_man = UnifiedRender::State::get_instance().tex_man;

    water_tex = &tex_man->load(Path::get("gfx/water_tex.png"), mipmap_options);

    line_shader = UnifiedRender::OpenGL::Program::create("shaders/simple_model.vs", "shaders/curve.fs");
    build_rivers();
}

struct ConnectedNode {
    ConnectedNode* node = nullptr;
    std::vector<glm::vec2>* river;

    ConnectedNode(): river{ new std::vector<glm::vec2> } {}
    ConnectedNode(std::vector<glm::vec2>* _river): river{ _river } {}
};
void Rivers::build_rivers() {
    auto tex_man = UnifiedRender::State::get_instance().tex_man;
    UnifiedRender::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    const UnifiedRender::Texture& river_tex = tex_man->load(Path::get("map/river.png"), no_drop_options);
    std::unordered_map<int, ConnectedNode*> river_map;
    std::vector<ConnectedNode*> nodes;

    auto pixels = river_tex.buffer.get();
    for(int y = 0; y < (int)river_tex.height; y++) {
        for(int x = 0; x < (int)river_tex.width; x++) {

            if(curves.size() > 20)
                continue;

            int curr_index = x + y * river_tex.width;
            uint32_t color = pixels[curr_index];
            if(color != 0xFFFF0000)
                continue;
            for(int yy = 0; yy <= 1; yy++) {
                int start_x = yy == 0? 1 : -1;
                for(int xx = start_x; xx <= 1; xx++) {
                    if(x + xx < 0 || y + yy < 0 || x + xx >= (int)river_tex.width || y + yy >= (int)river_tex.height)
                        continue;

                    int new_index = x + xx + (y + yy) * river_tex.width;
                    uint32_t neighbor_color = pixels[new_index];
                    if(neighbor_color != 0xFFFF0000)
                        continue;

                    glm::vec2 p1(x, y);
                    glm::vec2 p2(x + xx, y + yy);
                    if(!river_map.count(curr_index)) {
                        if(!river_map.count(new_index)) {
                            auto node = new ConnectedNode();
                            nodes.push_back(node);
                            // rivers.push_back(river);
                            river_map.insert({ curr_index, node });
                            river_map.insert({ new_index, node });
                            node->river->push_back(p1);
                            node->river->push_back(p2);
                        }
                        else {
                            auto old_node = river_map[new_index];
                            // while(old_node->node != nullptr) {
                            //     old_node = old_node->node;
                            // }
                            river_map.insert({ curr_index, old_node });
                            old_node->river->push_back(p1);
                        }
                    }
                    else {
                        if(!river_map.count(new_index)) {
                            auto old_node = river_map[curr_index];
                            // while(old_node->node != nullptr) {
                            //     old_node = old_node->node;
                            // }
                            river_map.insert({ new_index, old_node });
                            old_node->river->push_back(p2);
                        }
                        else {
                            auto active_node = river_map[curr_index];
                            while(active_node->node != nullptr) {
                                active_node = active_node->node;
                            }
                            auto old_node = river_map[new_index];
                            while(old_node->node != nullptr) {
                                old_node = old_node->node;
                            }
                            if(active_node != old_node) {
                                // for(auto point : *old_node->river) {
                                //     active_node->river->push_back(point);
                                // }
                                // delete old_node->river;
                                old_node->node = active_node;
                            }
                        }
                    }
                }
            }

        }
    }
    for(auto node : nodes) {
        // if (node->node != nullptr) {
        //     continue;
        // }
        if(node->river->size() == 0)
            continue;
        auto* river = node->river;
        auto* curve = new UnifiedRender::Curve(*river, 2.5f);
        curves.push_back(curve);
        delete river;
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