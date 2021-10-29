#include "map.hpp"

#include <GL/glu.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <mutex>

#include "../path.hpp"
#include "../print.hpp"
#include "game_state.hpp"
#include "render/model.hpp"
#include "../io_impl.hpp"

#include "interface/province_view.hpp"

Map::Map(const World& _world): world(_world) {
    overlay_tex = &g_texture_manager->load_texture(Path::get("ui/map_overlay.png"));
    if(glewIsSupported("GL_VERSION_2_1")) {
        map_quad = new UnifiedRender::OpenGl::PrimitiveSquare(0.f, 0.f, world.width, world.height);
        water_tex = &g_texture_manager->load_texture(Path::get("water_tex.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        noise_tex = &g_texture_manager->load_texture(Path::get("noise_tex.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        topo_tex = &g_texture_manager->load_texture(Path::get("map_topo.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        
        //terrain_tex = &g_texture_manager->load_texture(Path::get("map_ter_indx.png"));
        topo_tex = new UnifiedRender::Texture(world.width, world.height);

        terrain_tex = new UnifiedRender::Texture(world.width, world.height);
        for(size_t i = 0; i < world.width * world.height; i++) {
            terrain_tex->buffer[i] = world.tiles[i].terrain_type_id;
            topo_tex->buffer[i] = world.tiles[i].terrain_type_id;
        }
        
        terrain_sheet = new UnifiedRender::TextureArray(Path::get("terrain_sheet.png"), 4, 4);
        terrain_sheet->to_opengl();
        {
            auto vs = new UnifiedRender::OpenGl::VertexShader("shaders/map.vs");
            auto fs = new UnifiedRender::OpenGl::FragmentShader("shaders/map.fs");
            map_shader = new UnifiedRender::OpenGl::Program(vs, fs);
        }

        {
            auto vs = new UnifiedRender::OpenGl::VertexShader("shaders/simple_model.vs");
            auto fs = new UnifiedRender::OpenGl::FragmentShader("shaders/simple_model.fs");
            obj_shader = new UnifiedRender::OpenGl::Program(vs, fs);
        }
    }

    print_info("Creating topo map");

    // generate the underlying topo map texture, since the topo map
    // dosen't changes too much we can just do a texture
    div_topo_tex = new UnifiedRender::Texture(world.width, world.height);
    if(glewIsSupported("GL_VERSION_2_1")) {
        div_sheet_tex = new UnifiedRender::Texture(256, 256);
        for(size_t i = 0; i < 256 * 256; i++) {
            div_sheet_tex->buffer[i] = 0x00000000;
        }
        for(size_t i = 0; i < world.width * world.height; i++) {
            uint8_t r, g, b, a;
            const Tile& tile = world.get_tile(i);
            r = tile.province_id % 256;
            g = (tile.province_id / 256) % 256;
            b = tile.owner_id % 256;
            a = (tile.owner_id / 256) % 256;
            div_topo_tex->buffer[i] = (a << 24) | (b << 16) | (g << 8) | (r);

            // Only "paint" valid owners
            if(tile.owner_id == (Nation::Id)-1) continue;
            
            const Nation* owner = world.nations.at(tile.owner_id);
            uint32_t color = owner->get_client_hint().colour;
            div_sheet_tex->buffer[r + g * 256] = (0xff << 24) | color;
        }
        div_sheet_tex->to_opengl();
    }
    else {
        for(size_t i = 0; i < world.width * world.height; i++) {
            uint8_t r, g, b;
            const Tile& tile = world.get_tile(i);

            if(tile.terrain_type_id <= 1) {
                r = 8;
                g = 8;
                b = 128;
            } else {
                r = 8;
                g = 255;
                b = 8;
            }
            div_topo_tex->buffer[i] = (0xff << 24) | (b << 16) | (g << 8) | (r);
        }
    }
    div_topo_tex->to_opengl();

    // This can be put into unified render
    // I leave it for now since I havn't been able to test the code
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, div_topo_tex->gl_tex_num, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        print_info("Frame buffer error");
}

void Map::draw_flag(const Nation* nation) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_LINE);
    glColor3f(1.f, 1.f, 1.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, -2.f);
    glEnd();

    // Draw a flag that "waves" with some cheap wind effects it
    // looks nice and it's super cheap to make - only using sine
    const float n_steps = 8.f;  // Resolution of flag in one side (in vertices)
    const float step = 90.f;    // Steps per vertice
    
    auto flag = UnifiedRender::OpenGl::PackedModel<glm::vec3, glm::vec2>(GL_TRIANGLE_STRIP);
    for(float r = 0.f; r <= (n_steps * step); r += step) {
        float sin_r = (sin(r + wind_osc) / 24.f);
    
        sin_r = sin(r + wind_osc) / 24.f;
        flag.buffer.push_back(UnifiedRender::OpenGl::PackedData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -2.f),  // Vert
            glm::vec2((r / step) / n_steps, 0.f)                    // Texcoord
        ));

        sin_r = sin(r + wind_osc + 90.f) / 24.f;
        flag.buffer.push_back(UnifiedRender::OpenGl::PackedData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -1.f),  // Vert
            glm::vec2((r / step) / n_steps, 1.f)                    // Texcoord
        ));
    }

    flag.vao.bind();
    flag.vbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, flag.buffer.size() * sizeof(flag.buffer[0]), &flag.buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(flag.buffer[0]), (void*)0);  // Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(flag.buffer[0]), (void*)(3 * sizeof(float)));  // Texcoords
    glEnableVertexAttribArray(1);

    nation_flags.at(world.get_id(nation))->bind();
    flag.draw();

    // sin_r - Sin'ed iterator along with the wind oscillator
    /*glBegin(GL_TRIANGLE_STRIP);
    for(float r = 0.f; r <= (n_steps * step); r += step) {
        float sin_r;

        sin_r = sin(r + wind_osc) / 24.f;
        glColor3f((sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f);
        glTexCoord2f((r / step) / n_steps, 0.f);
        glVertex3f(((r / step) / n_steps) * 1.5f, sin_r, -2.f);

        sin_r = sin(r + wind_osc + 90.f) / 24.f;
        glColor3f((sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f);
        glTexCoord2f((r / step) / n_steps, 1.f);
        glVertex3f(((r / step) / n_steps) * 1.5f, sin_r, -1.f);
    }
    glEnd();*/
}

void Map::handle_click(GameState& gs, SDL_Event event) {
    Input& input = gs.input;
    if(input.select_pos.first < 0 ||
        input.select_pos.first >= gs.world->width ||
        input.select_pos.second < 0 ||
        input.select_pos.second >= gs.world->height) {
        return;
    }
    Unit* selected_unit = input.selected_unit;
    Building* selected_building = input.selected_building;

    if(event.button.button == SDL_BUTTON_LEFT) {
        std::pair<float, float>& select_pos = input.select_pos;

        selected_unit = nullptr;
        selected_building = nullptr;
        
        const Tile& tile = gs.world->get_tile(select_pos.first, select_pos.second);
        switch(gs.current_mode) {
        case MapMode::COUNTRY_SELECT:
            // TODO add call to functions from here
            gs.select_nation->change_nation(tile.owner_id);
            break;
        case MapMode::NORMAL:
            // Check if we selected an unit
            for(const auto& unit : gs.world->units) {
                const float size = 2.f;
                if((int)select_pos.first > (int)unit->x - size &&
                    (int)select_pos.first < (int)unit->x + size &&
                    (int)select_pos.second >(int)unit->y - size &&
                    (int)select_pos.second < (int)unit->y + size) {
                    selected_unit = unit;
                    return;
                }
            }

            // Check if we selected an building
            for(const auto& building : gs.world->buildings) {
                const float size = 2.f;
                if((int)select_pos.first > (int)building->x - size &&
                    (int)select_pos.first < (int)building->x + size &&
                    (int)select_pos.second >(int)building->y - size &&
                    (int)select_pos.second < (int)building->y + size) {
                    selected_building = building;
                    return;
                }
            }

            // Show province information when clicking on a province
            if(tile.province_id != (Province::Id)-1) {
                new Interface::ProvinceView(gs, gs.world->provinces.at(tile.province_id));
                return;
            }
            break;
        }
        
        // TODO: We should instead make it so you can build stuff in a "building" mode
        /*

        // Server will reject building build request if it's not in our territory (and it's not being built on water either)
        if(tile.owner_id != gs.world->get_id(gs.curr_nation) && tile.owner_id != (Nation::Id)-1) {
            return;
        }

        // Tell the server about an action for building an building
        gs.client->packet_mutex.lock();
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::BUILDING_ADD;
        ::serialize(ar, &action);
        Building building = Building();

        if(gs.world->get_tile(select_pos.first + 0, select_pos.second + 0).elevation <= gs.world->sea_level) {
            // Seaport if on bordering water
            building.type = gs.world->building_types[2];
        }
        else {
            // Barracks if on land
            building.type = gs.world->building_types[0];
        }

        building.x = select_pos.first;
        building.y = select_pos.second;
        building.working_unit_type = nullptr;
        building.req_goods = building.type->req_goods;
        // TODO FIX
        building.owner = gs.world->nations[gs.select_nation->curr_selected_nation];
        ::serialize(ar, &building);  // BuildingObj
        packet.data(ar.get_buffer(), ar.size());
        gs.client->packet_queue.push_back(packet);
        gs.client->packet_mutex.unlock();
        */
        return;
    }
    else if(event.button.button == SDL_BUTTON_RIGHT) {
        std::pair<float, float>& select_pos = input.select_pos;

        /*
        if(selected_unit != nullptr) {
            selected_unit->tx = select_pos.first;
            selected_unit->ty = select_pos.second;

            gs.client->packet_mutex.lock();
            Packet packet = Packet();
            Archive ar = Archive();
            ActionType action = ActionType::UNIT_CHANGE_TARGET;
            ::serialize(ar, &action);
            ::serialize(ar, &selected_unit);
            ::serialize(ar, &selected_unit->tx);
            ::serialize(ar, &selected_unit->ty);
            packet.data(ar.get_buffer(), ar.size());
            gs.client->packet_queue.push_back(packet);
            gs.client->packet_mutex.unlock();
            return;
        }
        if(selected_building != nullptr) {
            //new BuildUnitWindow(gs, selected_building, gs.top_win->top_win);
            return;
        }
        */
    }
}

// Updates the tiles texture with the changed tiles
void Map::update(World& world) {
    std::lock_guard lock(g_world->changed_tiles_coords_mutex);
    if(world.changed_tile_coords.size() > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glViewport(0, 0, div_topo_tex->width, div_topo_tex->height);

        glBegin(GL_POINTS);
        for(const auto& coords : world.changed_tile_coords) {
            uint8_t r, g, b, a;
            Tile tile = world.get_tile(coords.first, coords.second);
            r = tile.province_id % 256;
            g = (tile.province_id / 256) % 256;
            b = tile.owner_id % 256;
            a = (tile.owner_id / 256) % 256;
            // a = tile.elevation == 0 ? 255 : a;
            glColor4ui(r, g, b, a);
            glVertex2i(coords.first, coords.second);
        }
        glEnd();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        world.changed_tile_coords.clear();
    }
}

void Map::draw(Camera& cam, const int width, const int height) {
    glm::mat4 view, projection;

    // Map should have no "model" matrix since it's always static
    map_shader->use();
    view = cam.get_view();
    map_shader->set_uniform("view", view);
    projection = cam.get_projection();
    
    map_shader->set_uniform("projection", projection);
    
    map_shader->set_uniform("map_size", (float)world.width, (float)world.height);
    
    map_shader->set_uniform("tile_map", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, div_topo_tex->gl_tex_num);

    map_shader->set_uniform("tile_sheet", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, div_sheet_tex->gl_tex_num);

    map_shader->set_uniform("water_texture", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, water_tex->gl_tex_num);

    map_shader->set_uniform("noise_texture", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, noise_tex->gl_tex_num);

    map_shader->set_uniform("terrain_texture", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, terrain_tex->gl_tex_num);

    map_shader->set_uniform("terrain_sheet", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D_ARRAY, terrain_sheet->gl_tex_num);

    map_quad->draw();

    // TODO: We need to better this
    obj_shader->use();
    view = cam.get_view();
    obj_shader->set_uniform("view", view);
    projection = cam.get_projection();
    obj_shader->set_uniform("projection", projection);
    obj_shader->set_uniform("map_diffusion", 0);

    glActiveTexture(GL_TEXTURE0);
    obj_shader->set_uniform("tex", 0);
    
    world.world_mutex.lock();
    for(const auto& building : world.buildings) {
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(building->x, building->y, 0.f));
        
        model = glm::rotate(model, glm::radians(270.f), glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        outpost_type_icons.at(world.get_id(building->type))->draw(obj_shader);

        // Reverse rotation
        model = glm::rotate(model, glm::radians(-270.f), glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        draw_flag(building->get_owner());
    }

    for(const auto& unit : world.units) {
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(unit->x, unit->y, 0.f));

        model = glm::rotate(model, glm::radians(270.f), glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        unit_type_icons[world.get_id(unit->type)]->draw(obj_shader);

        // Reverse rotation
        model = glm::rotate(model, glm::radians(-270.f), glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        draw_flag(unit->owner);
    }
    world.world_mutex.unlock();

    // Resets the shader and texture
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);

    wind_osc += 1.f;
    if(wind_osc >= 180.f)
        wind_osc = 0.f;
}
