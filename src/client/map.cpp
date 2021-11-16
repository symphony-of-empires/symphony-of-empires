#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <mutex>

#include "client/map.hpp"
#include "path.hpp"
#include "print.hpp"
#include "client/game_state.hpp"
#include "client/render/model.hpp"
#include "io_impl.hpp"
#include "client/interface/province_view.hpp"
#include "world.hpp"

Map::Map(const World& _world): world(_world) {
    overlay_tex = &g_texture_manager->load_texture(Path::get("ui/map_overlay.png"));
    if(glewIsSupported("GL_VERSION_2_1")) {
        map_quad = new UnifiedRender::OpenGl::PrimitiveSquare(0.f, 0.f, world.width, world.height);
        map_sphere = new UnifiedRender::OpenGl::Sphere(0.f, 0.f, 0.f, 100.f, 100);
        map_2d_quad = new UnifiedRender::OpenGl::Quad2D();
        water_tex = &g_texture_manager->load_texture(Path::get("water_tex.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        water_tex->gen_mipmaps();
        noise_tex = &g_texture_manager->load_texture(Path::get("noise_tex.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        noise_tex->gen_mipmaps();
        topo_tex = &g_texture_manager->load_texture(Path::get("topo.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        topo_tex->gen_mipmaps();
        map_color = &g_texture_manager->load_texture(Path::get("map_col.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        map_color->gen_mipmaps();

        //terrain_tex = &g_texture_manager->load_texture(Path::get("map_ter_indx.png"));
        // topo_tex = new UnifiedRender::Texture(world.width, world.height);

        terrain_tex = new UnifiedRender::Texture(world.width, world.height);
        for(size_t i = 0; i < world.width * world.height; i++) {
            terrain_tex->buffer[i] = world.tiles[i].terrain_type_id;
            topo_tex->buffer[i] = world.tiles[i].elevation;
        }

        terrain_sheet = new UnifiedRender::TextureArray(Path::get("terrain_sheet.png"), 4, 4);
        terrain_sheet->to_opengl();
        {
            auto vs = UnifiedRender::OpenGl::VertexShader("shaders/map.vs");
            auto fs = UnifiedRender::OpenGl::FragmentShader("shaders/map.fs");
            map_shader = new UnifiedRender::OpenGl::Program(&vs, &fs);
        }

        {
            auto vs = UnifiedRender::OpenGl::VertexShader("shaders/simple_model.vs");
            auto fs = UnifiedRender::OpenGl::FragmentShader("shaders/simple_model.fs");
            obj_shader = new UnifiedRender::OpenGl::Program(&vs, &fs);
        }

        {
            auto vs = UnifiedRender::OpenGl::VertexShader("shaders/2d_shader.vs");
            auto fs = UnifiedRender::OpenGl::FragmentShader("shaders/border_gen.fs");
            border_gen_shader = new UnifiedRender::OpenGl::Program(&vs, &fs);
        }

        {
            auto vs = UnifiedRender::OpenGl::VertexShader("shaders/2d_shader.vs");
            auto fs = UnifiedRender::OpenGl::FragmentShader("shaders/border_sdf.fs");
            border_sdf_shader = new UnifiedRender::OpenGl::Program(&vs, &fs);
        }
    }

    print_info("Creating topo map");

    // generate the underlying topo map texture, since the topo map
    // dosen't changes too much we can just do a texture
    tile_map = new UnifiedRender::Texture(world.width, world.height);
    tile_sheet = new UnifiedRender::Texture(256, 256);
    for(size_t i = 0; i < 256 * 256; i++) {
        tile_sheet->buffer[i] = 0xffdddddd;
    }
    for(size_t i = 0; i < world.width * world.height; i++) {
        uint8_t r, g, b, a;
        const Tile& tile = world.get_tile(i);
        r = tile.province_id % 256;
        g = (tile.province_id / 256) % 256;
        b = tile.owner_id % 256;
        a = (tile.owner_id / 256) % 256;
        tile_map->buffer[i] = (a << 24) | (b << 16) | (g << 8) | (r);

        uint32_t color;
        if(tile.owner_id == (Nation::Id)-2) {
            // Water
            color = 0x00000000;
        }
        else if (tile.owner_id == (Nation::Id)-1) {
            // Land
            color = 0xffdddddd;
        }
        else {
            const Nation* owner = world.nations.at(tile.owner_id);
            color = owner->get_client_hint().colour;
        }

        tile_sheet->buffer[r + g * 256] = (0xff << 24) | color;
    }
    tile_sheet->to_opengl();
    tile_map->to_opengl();

    // This can be put into unified render
    // I leave it for now since I havn't been able to test the code
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, topo_tex->gl_tex_num, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        print_info("Frame buffer error");

    border_tex = new UnifiedRender::Texture(world.width * 2, world.height * 2);
    border_tex->to_opengl_test(GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    border_fbuffer = new UnifiedRender::OpenGl::Framebuffer();
    border_fbuffer->use();
    border_fbuffer->set_texture(0, border_tex);

    glViewport(0, 0, tile_map->width * 2, tile_map->height * 2);
    border_gen_shader->use();
    border_gen_shader->set_uniform("map_size", (float)tile_map->width * 2, (float)tile_map->height * 2);
    border_gen_shader->set_texture(0, "tile_map", tile_map);
    map_2d_quad->draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    border_tex->gen_mipmaps();

    border_sdf = gen_border_sdf();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Map::reload_shaders() {
    delete map_shader;
    auto vs = UnifiedRender::OpenGl::VertexShader("shaders/map.vs");
    auto fs = UnifiedRender::OpenGl::FragmentShader("shaders/map.fs");
    map_shader = new UnifiedRender::OpenGl::Program(&vs, &fs);

    delete border_sdf_shader;
    {
        auto vs = UnifiedRender::OpenGl::VertexShader("shaders/2d_shader.vs");
        auto fs = UnifiedRender::OpenGl::FragmentShader("shaders/border_sdf.fs");
        border_sdf_shader = new UnifiedRender::OpenGl::Program(&vs, &fs);
    }

    delete border_sdf;
    border_sdf = gen_border_sdf();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::vector<std::pair<Province::Id, uint32_t>> population_map_mode(std::vector<Province*> provinces, World* world) {
    std::vector<std::pair<Province::Id, uint32_t>> province_amounts;
    uint32_t max_amount = 0;
    for(auto const& province : provinces) {
        uint32_t color;
        uint32_t amount = 0;
        for(auto const& pop : province->pops) {
            amount += pop.size;
        }
        max_amount = std::max<size_t>(amount, max_amount);
        province_amounts.push_back(std::make_pair(world->get_id(province), amount));
    }
    max_amount = std::max<uint32_t>(1, max_amount);
    uint8_t max_r = 255;
    uint8_t max_g = 229;
    uint8_t max_b = 217;
    uint8_t min_r = 220;
    uint8_t min_g = 46;
    uint8_t min_b = 35;
    std::vector<std::pair<Province::Id, uint32_t>> province_color;
    for(auto const& prov_amount : province_amounts) {
        Province::Id prov_id = prov_amount.first;
        uint32_t amount = prov_amount.first;
        float ratio = ((float)amount) / max_amount;
        uint8_t prov_r = (uint8_t)(min_r * (1 - ratio) + max_r * ratio);
        uint8_t prov_g = (uint8_t)(min_g * (1 - ratio) + max_g * ratio);
        uint8_t prov_b = (uint8_t)(min_b * (1 - ratio) + max_b * ratio);
        uint32_t color = (0xff << 24) | (prov_b << 16) | (prov_g << 8) | (prov_r);
        province_color.push_back(std::make_pair(prov_id, color));
    }
    return province_color;
}

void Map::set_map_mode(std::vector<std::pair<Province::Id, uint32_t>> province_colors) {
    // Max amout of provinces are limited to 256 * 256
    tile_sheet->delete_opengl();
    tile_sheet = new UnifiedRender::Texture(256, 256);
    for(size_t i = 0; i < 256 * 256; i++) {
        tile_sheet->buffer[i] = 0x00000000;
    }
    for(auto const& province_color : province_colors) {
        Province::Id prov_id = province_color.first;
        u_int32_t color = province_color.second;
        uint8_t r = prov_id % 256;
        uint8_t g = (prov_id / 256) % 256;
        size_t id = r + g * 256;

        tile_map->buffer[id] = (0xff << 24) | color;
    }
    tile_sheet->to_opengl();
}

UnifiedRender::Texture* Map::gen_border_sdf() {
    glViewport(0, 0, border_tex->width, border_tex->height);
    border_sdf_shader->use();
    border_sdf_shader->set_uniform("map_size", (float)border_tex->width, (float)border_tex->height);
    UnifiedRender::Texture* tex0 = new UnifiedRender::Texture(border_tex->width, border_tex->height);
    tex0->to_opengl_test(GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
    UnifiedRender::Texture* tex1 = new UnifiedRender::Texture(border_tex->width, border_tex->height);
    tex1->to_opengl_test(GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
    UnifiedRender::OpenGl::Framebuffer* fbo = new UnifiedRender::OpenGl::Framebuffer();
    fbo->use();

    // Jump flooding iterations
    int max_steps = 128.;
    int step = max_steps;
    bool drawOnTex0 = true;
    while(step >= 1) {
        // Swap read/write buffers
        drawOnTex0 = !drawOnTex0;
        tex0->gen_mipmaps();
        tex1->gen_mipmaps();
        border_sdf_shader->set_uniform("jump", (float)step);

        if(step == max_steps){
            fbo->set_texture(0, drawOnTex0 ? tex0 : tex1);
            border_sdf_shader->set_texture(0, "tex", border_tex);
        }
        else {
            fbo->set_texture(0, drawOnTex0 ? tex0 : tex1);
            border_sdf_shader->set_texture(0, "tex", drawOnTex0 ? tex1 : tex0);
        }


        // Draw a plane over the entire screen to invoke shaders
        map_2d_quad->draw();

        step /= 2;
    }
    glFinish();
    delete fbo;
    if(drawOnTex0) {
        delete tex1;
        tex0->gen_mipmaps();
        return tex0;
    }
    else {
        delete tex0;
        tex1->gen_mipmaps();
        return tex1;
    }
}

void Map::draw_flag(const Nation* nation) {
    // Draw a flag that "waves" with some cheap wind effects it
    // looks nice and it's super cheap to make - only using sine
    const float n_steps = 8.f;  // Resolution of flag in one side (in vertices)
    const float step = 90.f;    // Steps per vertice

    auto flag = UnifiedRender::OpenGl::PackedModel<glm::vec3, glm::vec2>(GL_TRIANGLE_STRIP);
    for(float r = 0.f; r <= (n_steps * step); r += step) {
        float sin_r = (sin(r + wind_osc) / 24.f);

        sin_r = sin(r + wind_osc) / 24.f;
        flag.buffer.push_back(UnifiedRender::OpenGl::PackedData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -2.f),
            glm::vec2((r / step) / n_steps, 0.f)
            ));

        sin_r = sin(r + wind_osc + 90.f) / 24.f;
        flag.buffer.push_back(UnifiedRender::OpenGl::PackedData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -1.f),
            glm::vec2((r / step) / n_steps, 1.f)
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
                if((int)select_pos.first > (int)unit->x - size
                    && (int)select_pos.first < (int)unit->x + size
                    && (int)select_pos.second >(int)unit->y - size
                    && (int)select_pos.second < (int)unit->y + size) {
                    selected_unit = unit;
                    return;
                }
            }

            // Check if we selected a building
            for(const auto& building : gs.world->buildings) {
                const float size = 2.f;
                if((int)select_pos.first > (int)building->x - size
                    && (int)select_pos.first < (int)building->x + size
                    && (int)select_pos.second >(int)building->y - size
                    && (int)select_pos.second < (int)building->y + size) {
                    selected_building = building;
                    return;
                }
            }

            // Show province information when clicking on a province
            if(tile.province_id < (Province::Id)-2) {
                new Interface::ProvinceView(gs, gs.world->provinces.at(tile.province_id));
                return;
            }
            break;
        default:
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

        building.type = gs.world->building_types[0];

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
        glViewport(0, 0, tile_map->width, tile_map->height);

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

void Map::draw(OrbitCamera& cam, const int width, const int height) {
    glm::mat4 view, projection;

    // Map should have no "model" matrix since it's always static
    map_shader->use();
    view = cam.get_view();
    map_shader->set_uniform("view", view);
    map_shader->set_uniform("view_pos", cam.position.x, cam.position.y, cam.position.z);
    projection = cam.get_projection();

    map_shader->set_uniform("projection", projection);

    map_shader->set_uniform("map_size", (float)world.width, (float)world.height);

    map_shader->set_texture(0, "tile_map", tile_map);
    map_shader->set_texture(1, "tile_sheet", tile_sheet);
    map_shader->set_texture(2, "water_texture", water_tex);
    map_shader->set_texture(3, "noise_texture", noise_tex);
    map_shader->set_texture(4, "terrain_texture", terrain_tex);
    map_shader->set_texture(5, "terrain_sheet", terrain_sheet);
    map_shader->set_texture(6, "topo_texture", topo_tex);
    map_shader->set_texture(7, "border_tex", border_tex);
    map_shader->set_texture(8, "border_sdf", border_sdf);
    map_shader->set_texture(9, "map_color", map_color);

    // map_quad->draw();
    map_sphere->draw();

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
