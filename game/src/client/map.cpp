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
//      client/map.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <mutex>
#include <memory>
// Required before GL/gl.h
#include <GL/glew.h>
#include <GL/gl.h>

#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/primitive.hpp"
#include "unified_render/shader.hpp"
#include "unified_render/framebuffer.hpp"
#include "unified_render/model.hpp"

#include "client/map.hpp"
#include "client/map_render.hpp"
#include "client/game_state.hpp"
#include "unified_render/model.hpp"
#include "client/interface/province_view.hpp"
#include "client/interface/lobby.hpp"
#include "world.hpp"
#include "client/orbit_camera.hpp"
#include "client/flat_camera.hpp"
#include "client/camera.hpp"
#include "province.hpp"
#include "unified_render/state.hpp"

#include "client/client_network.hpp"
#include "unified_render/serializer.hpp"
#include "io_impl.hpp"
#include "action.hpp"

Map::Map(const World& _world, int screen_width, int screen_height)
    : world(_world)
{
    camera = new FlatCamera(glm::vec2(screen_width, screen_height), glm::vec2(world.width, world.height));

    map_render = new MapRender(world);

    // Shader used for drawing the models using custom model render
    obj_shader = UnifiedRender::OpenGL::Program::create("shaders/simple_model.vs", "shaders/simple_model.fs");
    line_tex = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/line_target.png"));

    // Set the mapmode
    set_map_mode(political_map_mode);

    print_info("Preloading-important stuff");
    // Query the initial nation flags
    for(const auto& nation : world.nations) {
        UnifiedRender::TextureOptions mipmap_options{};
        mipmap_options.wrap_s = GL_REPEAT;
        mipmap_options.wrap_t = GL_REPEAT;
        mipmap_options.min_filter = GL_NEAREST_MIPMAP_LINEAR;
        mipmap_options.mag_filter = GL_LINEAR;
        std::string path = Path::get("ui/flags/" + nation->ref_name + "_" +
            (nation->ideology == nullptr ? "none" : nation->ideology->ref_name) + ".png"
        );
        auto flag_texture = &UnifiedRender::State::get_instance().tex_man->load(path, mipmap_options);
        flag_texture->gen_mipmaps();
        nation_flags.push_back(flag_texture);
    }

    for(const auto& building_type : world.building_types) {
        std::string path;
        path = Path::get("3d/building_types/" + building_type->ref_name + ".obj");
        building_type_models.push_back(&UnifiedRender::State::get_instance().model_man->load(path));
        path = Path::get("ui/icons/building_types/" + building_type->ref_name + ".png");
        building_type_icons.push_back(&UnifiedRender::State::get_instance().tex_man->load(path));
    }

    for(const auto& unit_type : world.unit_types) {
        std::string path;
        path = Path::get("3d/unit_types/" + unit_type->ref_name + ".obj");
        unit_type_models.push_back(&UnifiedRender::State::get_instance().model_man->load(path));
        path = Path::get("ui/icons/unit_types/" + unit_type->ref_name + ".png");
        unit_type_icons.push_back(&UnifiedRender::State::get_instance().tex_man->load(path));
    }
}

Map::~Map() {

}

void Map::set_view(MapView view) {
    view_mode = view;

    Camera* old_camera = camera;
    if(view == MapView::PLANE_VIEW) {
        camera = new FlatCamera(old_camera);
    } else if(view == MapView::SPHERE_VIEW) {
        camera = new OrbitCamera(old_camera);
    }
    delete camera;
}

// The standard map mode with each province color = country color
std::vector<ProvinceColor> political_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        Nation* province_owner = world.provinces[i]->owner;
        if(province_owner == nullptr) {
            province_color.push_back(ProvinceColor(i, UnifiedRender::Color::rgba32(0xffdddddd)));
        } else if(province_owner->cached_id == (Nation::Id)-1) {
            province_color.push_back(ProvinceColor(i, UnifiedRender::Color::rgba32(0xffdddddd)));
        } else {
            province_color.push_back(ProvinceColor(i, UnifiedRender::Color::rgba32(province_owner->get_client_hint().color)));
        }
    }

    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UnifiedRender::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UnifiedRender::Color::rgba32(0xffdddddd)));
    return province_color;
}

void Map::reload_shaders() {
    map_render->reload_shaders();
}

void Map::set_map_mode(mapmode_generator _mapmode_func){
    mapmode_func = _mapmode_func;
    update_mapmode();
}

void Map::draw_flag(const UnifiedRender::OpenGL::Program& shader, const Nation& nation) {
    // Draw a flag that "waves" with some cheap wind effects it
    // looks nice and it's super cheap to make - only using sine
    const float n_steps = 8.f; // Resolution of flag in one side (in vertices)
    const float step = 90.f; // Steps per vertice

    auto flag = UnifiedRender::Mesh<glm::vec3, glm::vec2>(UnifiedRender::MeshMode::TRIANGLE_STRIP);
    for(float r = 0.f; r <= (n_steps * step); r += step) {
        float sin_r = (sin(r + wind_osc) / 24.f);

        sin_r = sin(r + wind_osc) / 24.f;
        flag.buffer.push_back(UnifiedRender::MeshData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -2.f),
            glm::vec2((r / step) / n_steps, 0.f)
        ));

        sin_r = sin(r + wind_osc + 160.f) / 24.f;
        flag.buffer.push_back(UnifiedRender::MeshData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -1.f),
            glm::vec2((r / step) / n_steps, 1.f)
        ));
    }
    flag.upload();

    shader.set_texture(0, "diffuse_map", *nation_flags[world.get_id(&nation)]);
    flag.draw();
}

void Map::handle_click(GameState& gs, SDL_Event event) {
    Input& input = gs.input;

    if(input.select_pos.first < 0 || input.select_pos.first >= gs.world->width || input.select_pos.second < 0 || input.select_pos.second >= gs.world->height) {
        return;
    }

    if(event.button.button == SDL_BUTTON_LEFT) {
        std::pair<float, float>& select_pos = input.select_pos;
        const Tile& tile = gs.world->get_tile(select_pos.first, select_pos.second);
        switch(gs.current_mode) {
        case MapMode::COUNTRY_SELECT:
            if(tile.province_id < (Province::Id)-3) {
                auto province = world.provinces[tile.province_id];
                if(province->controller == nullptr) {
                    break;
                }
                gs.select_nation->change_nation(province->controller->cached_id);
            }
            break;
        case MapMode::NORMAL:
            // Check if we selected an unit
            input.selected_units.clear();
            for(const auto& unit : gs.world->units) {
                // We can't control others units
                if(unit->owner != gs.curr_nation) {
                    continue;
                }

                std::pair<float, float> pos = unit->get_pos();

                std::pair<float, float> start_pos = input.drag_coord;
                if(input.select_pos.first < input.drag_coord.first) {
                    start_pos.first = input.select_pos.first;
                } if(input.select_pos.second < input.drag_coord.second) {
                    start_pos.second = input.select_pos.second;
                }

                std::pair<float, float> end_pos = input.drag_coord;
                if(input.select_pos.first > input.drag_coord.first) {
                    end_pos.first = input.select_pos.first;
                } if(input.select_pos.second > input.drag_coord.second) {
                    end_pos.second = input.select_pos.second;
                }

                if(pos.first >= start_pos.first && pos.first <= end_pos.first && pos.second >= start_pos.second && pos.second <= end_pos.second) {
                    input.selected_units.push_back(unit);
                }
            }
            is_drag = false;

            if(input.selected_units.empty()) {
                // Show province information when clicking on a province
                if(tile.province_id < gs.world->provinces.size()) {
                    new Interface::ProvinceView(gs, gs.world->provinces[tile.province_id]);
                    return;
                }
            }
            break;
        default:
            break;
        }
        return;
    } else if(event.button.button == SDL_BUTTON_RIGHT) {
        const Tile& tile = gs.world->get_tile(input.select_pos.first, input.select_pos.second);
        if(tile.province_id == (Province::Id)-1) {
            return;
        }

        Province* province = gs.world->provinces[tile.province_id];

        //if(input.selected_units.empty()) {
        if(gs.editor) {
            gs.curr_nation->give_province(*province);
            province->nuclei.insert(gs.curr_nation);
            update_mapmode();
        }

        for(const auto& unit : input.selected_units) {
            if(!unit->province->is_neighbour(*province)) {
                continue;
            }

            if(unit->province->controller != nullptr && unit->province->controller != gs.curr_nation) {
                // Must either be our ally, have military access with them or be at war
                const NationRelation& relation = gs.curr_nation->relations[gs.world->get_id(unit->province->controller)];
                if(!(relation.has_war || relation.has_alliance || relation.has_military_access)) {
                    continue;
                }
            }

            if(!unit->can_move()) {
                continue;
            }

            UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
            Archive ar = Archive();
            ActionType action = ActionType::UNIT_CHANGE_TARGET;
            ::serialize(ar, &action);
            ::serialize(ar, &unit);
            ::serialize(ar, &province);
            packet.data(ar.get_buffer(), ar.size());
            std::scoped_lock lock(gs.client->pending_packets_mutex);
            gs.client->pending_packets.push_back(packet);

            std::scoped_lock l1(gs.sound_lock);
            auto entries = Path::get_all_recursive("sfx/land_move");
            if(!entries.empty()) {
                gs.sound_queue.push_back(new UnifiedRender::Audio(entries[std::rand() % entries.size()]));
            }
        }
    }
}

void Map::update(const SDL_Event& event, Input& input) {
    std::pair<int, int>& mouse_pos = input.mouse_pos;
    // std::pair<float, float>& select_pos = input.select_pos;
    switch(event.type) {
    case SDL_JOYBUTTONDOWN:
    case SDL_MOUSEBUTTONDOWN:
        SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);

        is_drag = false;
        if(event.button.button == SDL_BUTTON_MIDDLE) {
            glm::ivec2 map_pos;
            if(camera->get_cursor_map_pos(input.mouse_pos, map_pos)) {
                last_camera_drag_pos = map_pos;
                input.last_camera_mouse_pos = mouse_pos;
            }
        }
        else if(event.button.button == SDL_BUTTON_LEFT) {
            input.drag_coord = input.select_pos;
            input.drag_coord.first = (int)input.drag_coord.first;
            input.drag_coord.second = (int)input.drag_coord.second;
            is_drag = true;
        }
        break;
    case SDL_JOYBUTTONUP:
    case SDL_MOUSEBUTTONUP:
        is_drag = false;
        break;
    case SDL_JOYAXISMOTION:{
        int xrel = SDL_JoystickGetAxis(input.joy, 0);
        int yrel = SDL_JoystickGetAxis(input.joy, 1);

        const float sensivity = UnifiedRender::State::get_instance().joy_sensivity;

        float x_force = xrel / sensivity;
        float y_force = yrel / sensivity;

        input.mouse_pos.first += x_force;
        input.mouse_pos.second += y_force;

        if(input.middle_mouse_down) {  // Drag the map with middlemouse
            glm::ivec2 map_pos;
            if(camera->get_cursor_map_pos(input.mouse_pos, map_pos)) {
                glm::vec2 current_pos = glm::make_vec2(camera->get_map_pos());
                const glm::vec2 pos =  current_pos + last_camera_drag_pos - glm::vec2(map_pos);
                camera->set_pos(pos.x, pos.y);
            }
        }
        glm::ivec2 map_pos;
        if(camera->get_cursor_map_pos(input.mouse_pos, map_pos)) {
            input.select_pos.first = map_pos.x;
            input.select_pos.second = map_pos.y;
        }
    } break;
    case SDL_MOUSEMOTION:
        SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
        glm::ivec2 map_pos;
        if(input.middle_mouse_down) {  // Drag the map with middlemouse
            if(camera->get_cursor_map_pos(mouse_pos, map_pos)) {
                glm::vec2 current_pos = glm::make_vec2(camera->get_map_pos());
                const glm::vec2 pos =  current_pos + last_camera_drag_pos - glm::vec2(map_pos);
                camera->set_pos(pos.x, pos.y);
            }
        }
        if(camera->get_cursor_map_pos(mouse_pos, map_pos)) {
            input.select_pos.first = map_pos.x;
            input.select_pos.second = map_pos.y;
        }
        break;
    case SDL_MOUSEWHEEL:
        SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
        camera->move(0.f, 0.f, event.wheel.y * -2.f);
        break;
    case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {
        case SDLK_UP:
            camera->move(0.f, -1.f, 0.f);
            break;
        case SDLK_DOWN:
            camera->move(0.f, 1.f, 0.f);
            break;
        case SDLK_LEFT:
            camera->move(-1.f, 0.f, 0.f);
            break;
        case SDLK_RIGHT:
            camera->move(1.f, 0.f, 0.f);
            break;
        }
        break;
    }
}

// Updates the province color texture with the changed provinces
void Map::update_mapmode() {
    std::vector<ProvinceColor> province_colors = mapmode_func(world);
    map_render->update_mapmode(province_colors);
}

void Map::draw(const GameState& gs) {
    glm::mat4 view, projection;
    map_render->draw(camera, view_mode);

    // TODO: We need to better this
    view = camera->get_view();
    projection = camera->get_projection();

    obj_shader->use();
    obj_shader->set_uniform("projection", projection);
    obj_shader->set_uniform("view", view);

    for(const auto& unit_type : world.unit_types) {
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(world.get_id(unit_type) * 8.f, -4.f, 0.f));
        model = glm::rotate(model, -90.f, glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        unit_type_models[world.get_id(unit_type)]->draw(*obj_shader);
    }

    for(const auto& building_type : world.building_types) {
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(world.get_id(building_type) * 8.f, 0.f, 0.f));
        model = glm::rotate(model, -90.f, glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        building_type_models[world.get_id(building_type)]->draw(*obj_shader);
    }

    glm::mat4 base_model(1.f);
    base_model = glm::translate(base_model, glm::vec3(0.f, 0.f, -1.f));
    for(const auto& province : world.provinces) {
        for(const auto& unit : province->get_units()) {
            glm::mat4 model = base_model;
            
            const std::pair<float, float> pos = unit->get_pos();
            model = glm::translate(model, glm::vec3(pos.first, pos.second, 0.f));
            if(unit->target != nullptr) {
                UnifiedRender::Line target_line = UnifiedRender::Line(pos.first, pos.second, unit->target->min_x + ((unit->target->max_x - unit->target->min_x) / 2.f), unit->target->min_y + ((unit->target->max_y - unit->target->min_y) / 2.f));
                obj_shader->set_texture(0, "diffuse_map", *line_tex);
                obj_shader->set_uniform("model", model);
                target_line.draw();

                const std::pair<float, float> tpos = unit->target->get_pos();
                model = glm::rotate(model, std::atan2(tpos.first - pos.first, tpos.second - pos.second), glm::vec3(0.f, 1.f, 0.f));
            }
            model = glm::rotate(model, -90.f, glm::vec3(1.f, 0.f, 0.f));
            obj_shader->set_uniform("model", model);
            draw_flag(*obj_shader, *unit->owner);

            // Model
            obj_shader->set_uniform("model", model);
            unit_type_models[world.get_id(unit->type)]->draw(*obj_shader);
        }
    }

    // Highlight for units
    for(const auto& unit : gs.input.selected_units) {
        glm::mat4 model = base_model;
        const std::pair<float, float> pos = unit->get_pos();
        model = glm::translate(model, glm::vec3(pos.first, pos.second, 0.f));
        UnifiedRender::Square select_highlight = UnifiedRender::Square(0.f, 0.f, 1.f, 1.f);
        obj_shader->set_texture(0, "diffuse_map", gs.tex_man->load(Path::get("ui/select_border.png")));
        select_highlight.draw();
    }

    // Draw the "drag area" box
    if(is_drag) {
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(0.f, 0.f, -1.f));
        obj_shader->set_texture(0, "diffuse_map", *line_tex);
        obj_shader->set_uniform("model", model);

        UnifiedRender::Square dragbox_square = UnifiedRender::Square(gs.input.drag_coord.first, gs.input.drag_coord.second, gs.input.select_pos.first, gs.input.select_pos.second);
        dragbox_square.draw();
    }

    if(view_mode == MapView::SPHERE_VIEW) {
        // Universe skybox
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(0.f, 0.f, -1.f));
        UnifiedRender::TextureOptions mipmap_options{};
        mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        mipmap_options.mag_filter = GL_LINEAR;
        auto& skybox_texture = gs.tex_man->load(Path::get("space.png"), mipmap_options);
        obj_shader->set_texture(0, "diffuse_map", skybox_texture);
        obj_shader->set_uniform("model", model);

        UnifiedRender::Sphere skybox = UnifiedRender::Sphere(0.f, 0.f, 0.f, 8000.f, 40);
        skybox.draw();
    }

    wind_osc += 0.1f;
    if(wind_osc >= 180.f) {
        wind_osc = 0.f;
    }
}
