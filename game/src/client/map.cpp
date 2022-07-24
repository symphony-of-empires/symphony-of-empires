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
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <memory>
#include <unordered_set>
#include <glm/gtc/matrix_transform.hpp>

#ifdef E3D_BACKEND_OPENGL
// Required before GL/gl.h
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#endif

#include "eng3d/state.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/font_sdf.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/framebuffer.hpp"
#include "eng3d/model.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/locale.hpp"
#include "eng3d/log.hpp"
#include "eng3d/orbit_camera.hpp"
#include "eng3d/flat_camera.hpp"
#include "eng3d/camera.hpp"

#include "client/map.hpp"
#include "client/map_render.hpp"
#include "client/game_state.hpp"
#include "client/interface/province_view.hpp"
#include "client/interface/unit_widget.hpp"
#include "client/interface/battle_widget.hpp"
#include "client/interface/lobby.hpp"
#include "world.hpp"
#include "province.hpp"
#include "client/client_network.hpp"
#include "io_impl.hpp"
#include "action.hpp"
#include "client/rivers.hpp"
#include "client/borders.hpp"

static inline void get_blob_bounds(std::unordered_set<Province*>& visited_provinces, const Nation& nation, const Province& province, glm::vec2* min_x, glm::vec2* min_y, glm::vec2* max_x, glm::vec2* max_y) {
    // Iterate over all neighbours
    for(const auto neighbour_id : province.neighbours) {
        auto& neighbour = g_world.provinces[neighbour_id];
        // Do not visit again
        if(visited_provinces.find(&neighbour) != visited_provinces.end()) continue;
        // Must control the province
        if(neighbour.controller != &nation) continue;
        // Big provinces not taken in account
        if(abs(neighbour.box_area.left - neighbour.box_area.right) >= g_world.width / 3.f) continue;
        if(abs(neighbour.box_area.top - neighbour.box_area.bottom) >= g_world.height / 3.f) continue;

        if(neighbour.box_area.left < min_x->x) {
            min_x->x = neighbour.box_area.left;
            min_x->y = neighbour.box_area.top;
        }
        if(neighbour.box_area.top < min_y->y) {
            min_y->x = neighbour.box_area.left;
            min_y->y = neighbour.box_area.top;
        }
        if(neighbour.box_area.right > max_x->x) {
            max_x->x = neighbour.box_area.right;
            max_x->y = neighbour.box_area.bottom;
        }
        if(neighbour.box_area.bottom > max_y->y) {
            max_y->x = neighbour.box_area.right;
            max_y->y = neighbour.box_area.bottom;
        }
        visited_provinces.insert(&neighbour);
        get_blob_bounds(visited_provinces, nation, neighbour, min_x, min_y, max_x, max_y);
    }
}

Map::Map(const World& _world, UI::Group* _map_ui_layer, int screen_width, int screen_height)
    : world{ _world },
    map_ui_layer{ _map_ui_layer },
    skybox(0.f, 0.f, 0.f, 255.f * 10.f, 40, false)
{
    auto& s = Eng3D::State::get_instance();
    camera = new Eng3D::FlatCamera(glm::vec2(screen_width, screen_height), glm::vec2(world.width, world.height));
    rivers = new Rivers();
    borders = new Borders();
    map_render = new MapRender(world, *this);

    // Shader used for drawing the models using custom model render
    obj_shader = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        obj_shader->attach_shader(*s.builtin_shaders["vs_3d"].get());
        obj_shader->attach_shader(*s.builtin_shaders["fs_3d"].get());
        obj_shader->link();
    }

    tree_shder = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        obj_shader->attach_shader(*s.builtin_shaders["vs_tree"].get());
        obj_shader->attach_shader(*s.builtin_shaders["fs_tree"].get());
        obj_shader->link();
    }

    // Set the mapmode
    set_map_mode(political_map_mode, political_province_tooltip);
    Eng3D::Log::debug("game", "Preloading-important stuff");
    map_font = new Eng3D::FontSDF("fonts/cinzel_sdf/cinzel");

    Eng3D::TextureOptions mipmap_options{};
    mipmap_options.wrap_s = GL_REPEAT;
    mipmap_options.wrap_t = GL_REPEAT;
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    mipmap_options.compressed = false;

    // Query the initial nation flags
    for(const auto& nation : world.nations) {
        std::string path = "gfx/flags/" + nation.ref_name + "_" + (nation.ideology == nullptr ? "none" : nation.ideology->ref_name.get_string()) + ".png";
        nation_flags.push_back(s.tex_man.load(s.package_man.get_unique(path), mipmap_options));
    }
    for(const auto& building_type : world.building_types) {
        std::string path;
        path = "models/building_types/" + building_type.ref_name + ".obj";
        building_type_models.push_back(s.model_man.load(s.package_man.get_unique(path)));
        path = "gfx/buildingtype/" + building_type.ref_name + ".png";
        building_type_icons.push_back(s.tex_man.load(s.package_man.get_unique((path))));
    }
    for(const auto& unit_type : world.unit_types) {
        std::string path;
        path = "models/unit_types/" + unit_type.ref_name + ".obj";
        unit_type_models.push_back(s.model_man.load(s.package_man.get_unique(path)));
        path = "gfx/unittype/" + unit_type.ref_name + ".png";
        unit_type_icons.push_back(s.tex_man.load(s.package_man.get_unique(path)));
    }

    line_tex = s.tex_man.load(s.package_man.get_unique("gfx/line_target.png"), mipmap_options);
    skybox_tex = s.tex_man.load(s.package_man.get_unique("gfx/space.png"), mipmap_options);

    for(const auto& terrain_type : world.terrain_types) {
        std::string path = "models/trees/" + terrain_type.ref_name + ".fbx";
        tree_type_models.push_back(s.model_man.load(s.package_man.get_unique(path)));
    }

    tree_spawn_pos.resize(world.provinces.size());
    for(const auto& province : world.provinces)
        tree_spawn_pos[province.get_id()] = std::make_pair(province.get_pos(), province.terrain_type->get_id());
    this->create_labels();
}

void Map::update_nation_label(const Nation& nation) {
    if(!nation.exists()) return;

    glm::vec2 min_point_x(world.width - 1.f, world.height - 1.f), min_point_y(world.width - 1.f, world.height - 1.f);
    glm::vec2 max_point_x(0.f, 0.f), max_point_y(0.f, 0.f);
    if(nation.owned_provinces.empty()) return;
    Province::Id prov_id = *nation.owned_provinces.begin();
    if(Province::is_valid(nation.capital_id))
        prov_id = nation.capital_id;

    const auto& province = world.provinces[prov_id];
    max_point_x = province.box_area.position() + province.box_area.size();
    max_point_y = province.box_area.position() + province.box_area.size();
    min_point_x = province.box_area.position();
    min_point_y = province.box_area.position();
    std::unordered_set<Province*> visited_provinces;
    get_blob_bounds(visited_provinces, nation, province, &min_point_x, &min_point_y, &max_point_x, &max_point_y);
#if 0
    // Stop super-big labels
    if(glm::abs(min_point_x.x - max_point_x.x) >= world.width / 2.f || glm::abs(min_point_y.y - max_point_y.y) >= world.height / 2.f) {
        auto label = this->map_font->gen_text(nation->get_client_hint().alt_name, glm::vec3(-10.f), glm::vec3(-5.f), 1.f);
        nation_labels.push_back(label);
        Eng3D::Log::debug("game", "Extremely big nation: " + nation->ref_name);
        return;
    }
#endif
    glm::vec2 lab_min = (max_point_x + max_point_y + min_point_y) / 3.f;
    glm::vec2 lab_max = (max_point_y + min_point_x + min_point_y) / 3.f;
    glm::vec2 mid_point = (lab_min + lab_max) / 2.f;
    glm::vec3 center = glm::vec3(mid_point, 0.f);
    glm::vec2 x_step = glm::vec2(lab_max.x - mid_point.x, 0.f);
    glm::vec3 left = glm::vec3(mid_point - x_step, 0.f);
    glm::vec3 right = glm::vec3(mid_point + x_step, 0.f);
    float width = glm::length(left - right);
    glm::vec3 right_dir = glm::vec3(mid_point.x + 1.f, mid_point.y, 0.f) - center;
    glm::vec3 top_dir = glm::vec3(mid_point.x, mid_point.y - 1.f, 0.f) - center;
    glm::vec3 normal = glm::cross(top_dir, right_dir);
    normal = glm::normalize(normal);
    float angle = glm::atan(lab_max.y - lab_min.y, lab_max.x - lab_min.x);
    if(angle > (M_PI / 2.0f)) angle -= M_PI;
    else if(angle < -(M_PI / 2.0f)) angle += M_PI;
    glm::mat4 rot = glm::rotate(glm::mat4(1.), angle, normal);
    top_dir = rot * glm::vec4(top_dir, 1.);
    right_dir = rot * glm::vec4(right_dir, 1.);

    center.z -= 0.1f;
    auto label = this->map_font->gen_text(Eng3D::Locale::translate(nation.get_client_hint().alt_name.get_string()), top_dir, right_dir, width, center);

    // Replace old label
    assert(nation_labels.size() >= nation.get_id());
    nation_labels[nation.get_id()] = std::move(label);
}

void Map::create_labels() {
    // Provinces
    province_labels.clear();
    for(const auto& province : world.provinces) {
        glm::vec2 min_point(province.box_area.left, province.box_area.top);
        glm::vec2 max_point(province.box_area.right, province.box_area.bottom);
        glm::vec2 mid_point = 0.5f * (min_point + max_point);
        glm::vec2 x_step = glm::vec2(std::min(max_point.x - mid_point.x, 250.f), 0);
        glm::vec3 center = glm::vec3(mid_point, 0.f);
        glm::vec3 left = glm::vec3(mid_point - x_step, 0.f);
        glm::vec3 right = glm::vec3(mid_point + x_step, 0.f);
        float width = glm::length(left - right);
        width *= 0.25f;

        glm::vec3 right_dir = glm::vec3(mid_point.x + 1.f, mid_point.y, 0.);
        right_dir = right_dir - center;
        glm::vec3 top_dir = glm::vec3(mid_point.x, mid_point.y - 1.f, 0.);
        top_dir = top_dir - center;
        center.z -= 0.1f;
        auto label = this->map_font->gen_text(Eng3D::Locale::translate(province.name.get_string()), top_dir, right_dir, width, center);
        province_labels.push_back(std::move(label));
    }

    // Nations
    nation_labels.clear();
    for(const auto& nation : world.nations) {
        this->update_nation_label(nation);
    }
}

void Map::set_view(MapView view) {
    view_mode = view;

    Eng3D::Camera* old_camera = camera;
    if(view == MapView::PLANE_VIEW)
        camera = new Eng3D::FlatCamera(*old_camera);
    else if(view == MapView::SPHERE_VIEW)
        camera = new Eng3D::OrbitCamera(*old_camera, GLOBE_RADIUS);
    delete old_camera;
    // create_labels();
}

std::string political_province_tooltip(const World& world, const Province::Id id) {
    if(((GameState&)Eng3D::State::get_instance()).editor) {
        return world.provinces[id].ref_name.get_string();
    } else {
        return "";
    }
}

// The standard map mode with each province color = country color
std::vector<ProvinceColor> political_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(Province::Id i = 0; i < world.provinces.size(); i++) {
        auto* province_owner = world.provinces[i].controller;
        if(province_owner == nullptr)
            province_color.push_back(ProvinceColor(i, Eng3D::Color::rgba32(0xffdddddd)));
        else
            province_color.push_back(ProvinceColor(i, Eng3D::Color::rgba32(province_owner->get_client_hint().color)));
    }

    // Land
    province_color.push_back(ProvinceColor(Province::invalid(), Eng3D::Color::rgba32(0xffdddddd)));
    return province_color;
}

std::string empty_province_tooltip(const World&, const Province::Id) {
    return "";
}

void Map::reload_shaders() {
    map_render->reload_shaders();
}

void Map::set_selection(selector_func _selector) {
    selector = _selector;
}

void Map::set_map_mode(mapmode_generator mapmode_generator, mapmode_tooltip tooltip_generator) {
    mapmode_func = mapmode_generator;
    mapmode_tooltip_func = tooltip_generator;
    update_mapmode();
}

void Map::set_selected_province(bool selected, Province::Id id) {
    this->province_selected = selected;
    this->selected_province_id = id;
    map_render->request_update_visibility();
}

void Map::draw_flag(const Eng3D::OpenGL::Program& shader, const Nation& nation) {
    // Draw a flag that "waves" with some cheap wind effects it
    // looks nice and it's super cheap to make - only using sine
    const float n_steps = 8.f; // Resolution of flag in one side (in vertices)
    const float step = 90.f; // Steps per vertice
    auto flag = Eng3D::Mesh<glm::vec3, glm::vec2>(Eng3D::MeshMode::TRIANGLE_STRIP);
    for(float r = 0.f; r <= (n_steps * step); r += step) {
        float sin_r = (sin(r + wind_osc) / 24.f);
        sin_r = sin(r + wind_osc) / 24.f;
        flag.buffer.push_back(Eng3D::MeshData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -2.f),
            glm::vec2((r / step) / n_steps, 0.f)
            ));
        sin_r = sin(r + wind_osc + 160.f) / 24.f;
        flag.buffer.push_back(Eng3D::MeshData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -1.f),
            glm::vec2((r / step) / n_steps, 1.f)
            ));
    }
    flag.upload();
    shader.set_texture(0, "diffuse_map", *nation_flags[world.get_id(nation)]);
    flag.draw();
}

void Map::handle_click(GameState& gs, SDL_Event event) {
    auto& input = gs.input;
    if(input.select_pos.x < 0 || input.select_pos.x >= this->world.width || input.select_pos.y < 0 || input.select_pos.y >= this->world.height)
        return;

    if(event.button.button == SDL_BUTTON_LEFT) {
        const Tile& tile = gs.world->get_tile(input.select_pos.x, input.select_pos.y);
        switch(gs.current_mode) {
        case MapMode::COUNTRY_SELECT:
            if(Province::is_valid(tile.province_id)) {
                auto& province = world.provinces[tile.province_id];
                if(province.controller != nullptr)
                    gs.select_nation->change_nation(province.controller->cached_id);
            }
            break;
        case MapMode::NORMAL:
            if(this->selector) {
                /// @todo Good selector function
                //this->selector(this->world, *this, world.provinces[tile.province_id]);
                break;
            }

            // Check if we selected an unit
            is_drag = false;
            if(input.get_selected_units().empty()) {
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
        const Tile& tile = gs.world->get_tile(input.select_pos.x, input.select_pos.y);
        if(Province::is_invalid(tile.province_id)) return;
        Province& province = gs.world->provinces[tile.province_id];
        if(gs.editor) {
            switch(gs.current_mode) {
            case MapMode::NORMAL:
                gs.curr_nation->control_province(province);
                gs.curr_nation->give_province(province);
                province.nuclei.insert(gs.world->get_id(*gs.curr_nation));
                update_mapmode();
                break;
            default:
                break;
            }
        }

        /// @todo Handle the case where an unit is deleted
        for(const auto unit_id : gs.input.get_selected_units()) {
            auto& unit = gs.world->unit_manager.units[unit_id];
            auto province_id = gs.world->unit_manager.unit_province[unit_id];
            if(!gs.world->provinces[province_id].is_neighbour(province) || !unit.can_move()) continue;
            // Don't change target if ID is the same...
            if(province_id == gs.world->get_id(province) || unit.get_target_province_id() == gs.world->get_id(province))
                continue;
            if(province.controller != nullptr && province.controller != gs.curr_nation) {
                // Must either be our ally, have military access with them or be at war
                const NationRelation& relation = gs.world->get_relation(gs.world->get_id(*gs.curr_nation), gs.world->get_id(*province.controller));
                if(!(relation.has_war || relation.has_alliance || relation.has_military_access)) continue;
            }

            Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
            Archive ar = Archive();
            ActionType action = ActionType::UNIT_CHANGE_TARGET;
            ::serialize(ar, &action);
            ::serialize(ar, &unit_id);
            Province* tmp_province_ref = &province;
            ::serialize(ar, &tmp_province_ref);
            packet.data(ar.get_buffer(), ar.size());
            g_client->send(packet);

            const std::scoped_lock lock2(gs.audio_man.sound_lock);
            auto entries = gs.package_man.get_multiple_prefix("sfx/land_move");
            if(!entries.empty()) {
                auto audio = gs.audio_man.load(entries[std::rand() % entries.size()]->get_abs_path());
                gs.audio_man.sound_queue.push_back(audio);
            }
        }
        input.clear_selected_units();
    }
}

void Map::update(const SDL_Event& event, Input& input, UI::Context* ui_ctx, GameState& gs) {
    auto& mouse_pos = input.mouse_pos;
    switch(event.type) {
    case SDL_JOYBUTTONDOWN:
    case SDL_MOUSEBUTTONDOWN:
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        is_drag = false;
        if(event.button.button == SDL_BUTTON_MIDDLE) {
            glm::ivec2 map_pos;
            if(camera->get_cursor_map_pos(input.mouse_pos, map_pos)) {
                last_camera_drag_pos = map_pos;
            }
        } else if(event.button.button == SDL_BUTTON_LEFT) {
            input.drag_coord = input.select_pos;
            is_drag = true;
        }
        break;
    case SDL_JOYBUTTONUP:
    case SDL_MOUSEBUTTONUP:
        is_drag = false;
        break;
    case SDL_JOYAXISMOTION: {
        int xrel = SDL_JoystickGetAxis(input.joy, 0);
        int yrel = SDL_JoystickGetAxis(input.joy, 1);
        const float sensivity = Eng3D::State::get_instance().joy_sensivity;
        float x_force = xrel / sensivity;
        float y_force = yrel / sensivity;
        input.mouse_pos.x += x_force;
        input.mouse_pos.y += y_force;
        if(input.middle_mouse_down) {  // Drag the map with middlemouse
            glm::ivec2 map_pos;
            if(camera->get_cursor_map_pos(input.mouse_pos, map_pos)) {
                glm::vec2 current_pos = glm::make_vec2(camera->get_map_pos());
                const glm::vec2 pos = current_pos + last_camera_drag_pos - glm::vec2(map_pos);
                camera->set_pos(pos.x, pos.y);
            }
        }
        glm::ivec2 map_pos;
        if(camera->get_cursor_map_pos(input.mouse_pos, map_pos)) {
            input.select_pos = map_pos;
        }
    } break;
    case SDL_MOUSEMOTION:
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        glm::ivec2 map_pos;

        if(input.middle_mouse_down) {  // Drag the map with middlemouse
            if(camera->get_cursor_map_pos(mouse_pos, map_pos)) {
                glm::vec2 current_pos = glm::make_vec2(camera->get_map_pos());
                const glm::vec2 pos = current_pos + last_camera_drag_pos - glm::vec2(map_pos);
                camera->set_pos(pos.x, pos.y);
            }
        }

        if(camera->get_cursor_map_pos(mouse_pos, map_pos)) {
            if(map_pos.x < 0 || map_pos.x >(int)world.width || map_pos.y < 0 || map_pos.y >(int)world.height) break;
            input.select_pos = map_pos;
            auto prov_id = world.get_tile(map_pos.x, map_pos.y).province_id;
            if(mapmode_tooltip_func != nullptr) {
                auto* tooltip = new UI::Tooltip();
                tooltip->text(mapmode_tooltip_func(world, prov_id));
                ui_ctx->use_tooltip(tooltip, mouse_pos);
            }
        }
        break;
    case SDL_MOUSEWHEEL:
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        camera->move(0.f, 0.f, -event.wheel.y * gs.delta_time * 120.f);
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

void Map::draw(GameState& gs) {
    const auto projection = camera->get_projection();
    const auto view = camera->get_view();
    glm::mat4 base_model = glm::mat4(1.f);

    map_render->draw(camera, view_mode);
    // rivers->draw(camera);
    // borders->draw(camera);

    /// @todo We need to better this
    obj_shader->use();
    obj_shader->set_uniform("projection", projection);
    obj_shader->set_uniform("view", view);

    auto preproc_quad = Eng3D::Quad2D(); // Reused a bunch of times

    // Properly display textures :)
    std::vector<float> province_units_y(world.provinces.size(), 0.f);

    // Display units that aren't on battles
    Unit::Id unit_index = 0, battle_index = 0;
    for(auto& province : const_cast<World&>(world).provinces) {
        // Units
        if(!gs.world->unit_manager.get_province_units(province.get_id()).empty()) {
            size_t total_stack_size = 0; // Calculate the total size of our stack
            for(const auto unit_id : gs.world->unit_manager.get_province_units(province.get_id())) {
                const auto& unit = gs.world->unit_manager.units[unit_id];
                total_stack_size += unit.size;
            }

            // Get first/topmost unit
            const auto unit_id = gs.world->unit_manager.get_province_units(province.get_id())[0];
            auto& unit = gs.world->unit_manager.units[unit_id];
            bool has_widget = false;
            if((this->map_render->get_province_opt(province.get_id()) & 0x000000ff) == 0x000000ff) {
                auto& camera = this->camera;
                const glm::vec2 prov_pos = province.get_pos();
                // And display units
                if(unit.on_battle) return;
                bool unit_visible = true;
                if(this->view_mode == MapView::SPHERE_VIEW) {
                    const auto* orbit_camera = static_cast<const Eng3D::OrbitCamera*>(camera);
                    const auto cam_pos = camera->get_world_pos();
                    const auto world_pos = camera->get_tile_world_pos(prov_pos);
                    const auto world_to_camera = glm::normalize(cam_pos - world_pos) * orbit_camera->radius * 0.001f;
                    if(glm::length(world_pos + world_to_camera) < orbit_camera->radius)
                        unit_visible = false;
                }

                if(unit_visible) {
                    if(unit_index < unit_widgets.size()) {
                        this->unit_widgets[unit_index]->set_unit(unit);
                    } else {
                        this->unit_widgets.push_back(new Interface::UnitWidget(unit, *this, gs, this->map_ui_layer));
                    }
                    unit_index++;
                    has_widget = true;
                }
            }
            if(has_widget) this->unit_widgets[unit_index - 1]->set_size(total_stack_size);
        }

        // Battles
        if((this->map_render->get_province_opt(province.get_id()) & 0x000000ff) == 0x000000ff) {
            const auto prov_pos = province.get_pos();
            size_t war_battle_idx = 0;
            for(size_t i = 0; i < province.battles.size(); i++) {
                bool battle_visible = true;
                if(view_mode == MapView::SPHERE_VIEW) {
                    const auto* orbit_camera = static_cast<const Eng3D::OrbitCamera*>(camera);
                    const auto cam_pos = camera->get_world_pos();
                    const auto world_pos = camera->get_tile_world_pos(prov_pos);
                    const auto world_to_camera = glm::normalize(cam_pos - world_pos) * orbit_camera->radius * 0.001f;
                    if(glm::length(world_pos + world_to_camera) < orbit_camera->radius)
                        battle_visible = false;
                }

                if(battle_visible) {
                    if(battle_index < battle_widgets.size())
                        battle_widgets[battle_index]->set_battle(province, war_battle_idx);
                    else
                        battle_widgets.push_back(new Interface::BattleWidget(province, war_battle_idx, *this, map_ui_layer));
                    battle_index++;
                }
                war_battle_idx++;
            }

            const auto& province_units = world.unit_manager.get_province_units(world.get_id(province));
            if(!province_units.empty()) {
                const auto& unit = world.unit_manager.units[province_units[0]];
                auto model = glm::translate(base_model, glm::vec3(prov_pos.x, prov_pos.y, 0.f));
                if(Province::is_valid(unit.get_target_province_id())) {
                    const auto& unit_target = world.provinces[unit.get_target_province_id()];
                    const auto target_pos = unit_target.get_pos();
                    const auto dist = glm::sqrt(glm::pow(glm::abs(prov_pos.x - target_pos.x), 2.f) + glm::pow(glm::abs(prov_pos.y - target_pos.y), 2.f));
                    auto line_square = Eng3D::Square(0.f, 0.f, dist, 0.5f);
                    const auto line_model = glm::rotate(model, glm::atan(target_pos.y - prov_pos.y, target_pos.x - prov_pos.x), glm::vec3(0.f, 0.f, 1.f));
                    obj_shader->set_texture(0, "diffuse_map", *line_tex);
                    obj_shader->set_uniform("model", line_model);
                    line_square.draw();
                }
                model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
                obj_shader->set_uniform("model", model);
                unit_type_models[world.get_id(*unit.type)]->draw(*obj_shader);
            }
        }
    }
    for(Unit::Id i = unit_index; i < unit_widgets.size(); i++)
        unit_widgets[i]->kill();
    unit_widgets.resize(unit_index);
    for(Unit::Id i = battle_index; i < battle_widgets.size(); i++)
        battle_widgets[i]->kill();
    battle_widgets.resize(battle_index);

    // Unit movement lines
    gs.world->unit_manager.for_each_unit([&gs] (Unit& unit) {
        if (gs.curr_nation && unit.owner_id != gs.curr_nation->get_id()) 
            return;
        
        const auto& path = unit.get_path();

    });


    // Buildings
    for(const auto& province : world.provinces) {
        province_units_y[world.get_id(province)] += 2.5f;
        const auto prov_pos = province.get_pos();

        for(const auto& building_type : world.building_types) {
            if(!province.buildings[building_type.get_id()].level) continue;
            glm::mat4 model = glm::translate(base_model, glm::vec3(prov_pos.x, prov_pos.y, 0.f));
            model = glm::rotate(model, -90.f, glm::vec3(1.f, 0.f, 0.f));
            obj_shader->set_uniform("model", model);
            building_type_models[building_type.get_id()]->draw(*obj_shader);
            break;
        }
    }
    //*/

    // Draw the "drag area" box
    if(is_drag) {
        auto model = base_model;
        obj_shader->set_uniform("model", model);
        obj_shader->set_texture(0, "diffuse_map", *line_tex);
        obj_shader->set_texture(1, "ambient_map", *gs.tex_man.get_white());
        obj_shader->set_texture(2, "occlussion_map", *gs.tex_man.get_white());
        obj_shader->set_texture(3, "height_map", *gs.tex_man.get_white());
        obj_shader->set_texture(4, "specular_map", *gs.tex_man.get_white());
        obj_shader->set_texture(5, "normal_map", *gs.tex_man.get_white());
        auto dragbox_square = Eng3D::Square(gs.input.drag_coord.x, gs.input.drag_coord.y, gs.input.select_pos.x, gs.input.select_pos.y);
        dragbox_square.draw();
    }

    const auto map_pos = camera->get_map_pos();
    const auto distance_to_map = map_pos.z / world.width;
    if(distance_to_map < 0.070) map_font->draw(province_labels, camera, view_mode == MapView::SPHERE_VIEW);
    else map_font->draw(nation_labels, camera, view_mode == MapView::SPHERE_VIEW);

    if(view_mode == MapView::SPHERE_VIEW) {
        // Universe skybox
        const auto model = base_model;
        obj_shader->set_uniform("model", model);
        obj_shader->set_texture(0, "diffuse_map", *skybox_tex);
        obj_shader->set_texture(1, "ambient_map", *gs.tex_man.get_white());
        obj_shader->set_texture(2, "occlussion_map", *gs.tex_man.get_white());
        obj_shader->set_texture(3, "height_map", *gs.tex_man.get_white());
        obj_shader->set_texture(4, "specular_map", *gs.tex_man.get_white());
        obj_shader->set_texture(5, "normal_map", *gs.tex_man.get_white());
        skybox.draw();
    }

    // Drawing trees
    /*tree_shder->use();
    tree_shder->set_uniform("projection", projection);
    tree_shder->set_uniform("view", view);
    for(auto& province : const_cast<World&>(world).provinces) {
        const auto& tree = tree_spawn_pos[province.get_id()];
        glm::mat4 model = glm::translate(base_model, glm::vec3(tree.first, 0.f));
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        tree_shder->set_uniform("model", model);
        for(const auto& simple_model : tree_type_models[tree.second]->simple_models)
            simple_model.draw(*tree_shder, 10);
    }*/

    wind_osc += 0.1f;
}
