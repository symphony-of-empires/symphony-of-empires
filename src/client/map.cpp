#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <mutex>
#include <memory>

#include "client/map.hpp"
#include "path.hpp"
#include "unified_render/print.hpp"
#include "client/game_state.hpp"
#include "unified_render/model.hpp"
#include "io_impl.hpp"
#include "client/interface/province_view.hpp"
#include "world.hpp"
#include "client/orbit_camera.hpp"
#include "client/flat_camera.hpp"
#include "client/camera.hpp"
#include "client/interface/lobby.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/primitive.hpp"
#include "unified_render/sphere.hpp"
#include "unified_render/shader.hpp"
#include "unified_render/texture_array.hpp"
#include "unified_render/quad_2d.hpp"
#include "unified_render/framebuffer.hpp"
#include "province.hpp"
#include "unified_render/model.hpp"
#include "unified_render/assimp_model.hpp"

Map::Map(const World& _world, int screen_width, int screen_height)
    : world(_world)
{
    overlay_tex = &g_texture_manager->load_texture(Path::get("ui/map_overlay.png"));
    camera = new FlatCamera(screen_width, screen_height);

    model_shader = UnifiedRender::OpenGl::Program::create("model_loading", "model_loading");
    map_quad = new UnifiedRender::OpenGl::PrimitiveSquare(0.f, 0.f, world.width, world.height);
    map_sphere = new UnifiedRender::OpenGl::Sphere(0.f, 0.f, 0.f, 100.f, 100);

    if(glewIsSupported("GL_VERSION_2_1")) {
        map_2d_quad = new UnifiedRender::OpenGl::Quad2D();
        UnifiedRender::TextureOptions mipmap_options{};
        mipmap_options.wrap_s = GL_REPEAT;
        mipmap_options.wrap_t = GL_REPEAT;
        mipmap_options.min_filter = GL_NEAREST_MIPMAP_LINEAR;
        mipmap_options.mag_filter = GL_LINEAR;
        water_tex = &g_texture_manager->load_texture(Path::get("water_tex.png"), mipmap_options);
        water_tex->gen_mipmaps();
        noise_tex = &g_texture_manager->load_texture(Path::get("noise_tex.png"), mipmap_options);
        noise_tex->gen_mipmaps();
        topo_map = &g_texture_manager->load_texture(Path::get("topo.png"), mipmap_options);
        topo_map->gen_mipmaps();
        landscape_map = &g_texture_manager->load_texture(Path::get("map_col.png"), mipmap_options);
        landscape_map->gen_mipmaps();
        river_tex = &g_texture_manager->load_texture(Path::get("river_smal_smooth.png"), mipmap_options);
        river_tex->gen_mipmaps();

        //terrain_tex = &g_texture_manager->load_texture(Path::get("map_ter_indx.png"));
        //topo_map = new UnifiedRender::Texture(world.width, world.height);

        // terrain_tex = new UnifiedRender::Texture(world.width, world.height);
        // for(size_t i = 0; i < world.width * world.height; i++) {
        //     terrain_tex->buffer[i] = world.tiles[i].terrain_type_id;
        //     topo_map->buffer[i] = world.tiles[i].elevation;
        // }
        // terrain_tex->to_opengl();

        terrain_sheet = new UnifiedRender::TextureArray(Path::get("terrain_sheet.png"), 4, 4);
        terrain_sheet->to_opengl();

        map_shader = UnifiedRender::OpenGl::Program::create("map", "map");
        obj_shader = UnifiedRender::OpenGl::Program::create("simple_model", "simple_model");
        border_gen_shader = UnifiedRender::OpenGl::Program::create("2d_shader", "border_gen");
        border_sdf_shader = UnifiedRender::OpenGl::Program::create("2d_shader", "border_sdf");

        ourModel = new UnifiedRender::Model(Path::get("3d/backpack/backpack.obj"));
        model_shader = UnifiedRender::OpenGl::Program::create("model_loading", "model_loading");
    }

    print_info("Creating topo map");

    // generate the underlying topo map texture, since the topo map
    // dosen't changes too much we can just do a texture
    tile_map = new UnifiedRender::Texture(world.width, world.height);
    terrain_tex = new UnifiedRender::Texture(world.width, world.height);

    // Texture holding the colors of each owner
    tile_sheet = new UnifiedRender::Texture(256, 256);
    for(size_t i = 0; i < 256 * 256; i++) {
        tile_sheet->buffer[i] = 0xffdddddd;
    }
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        Nation* province_owner = world.provinces[i]->owner;
        if(province_owner == nullptr) {
            tile_sheet->buffer[i] = 0xffdddddd;
        }
        else if(province_owner->cached_id == (Nation::Id)-1) {
            tile_sheet->buffer[i] = 0xffdddddd;
        }
        else {
            tile_sheet->buffer[i] = province_owner->get_client_hint().color;
        }
    }
    // Water
    tile_sheet->buffer[(Province::Id)-2] = 0x00000000;
    // Land
    tile_sheet->buffer[(Province::Id)-1] = 0xffdddddd;

    for(size_t i = 0; i < world.width * world.height; i++) {
        const Tile& tile = world.get_tile(i);
        if(tile.province_id >= (Province::Id)-3) {
            tile_map->buffer[i] = (tile.province_id & 0xffff);
        }
        else {
            auto province = world.provinces[tile.province_id];
            if(province->owner == nullptr) {
                tile_map->buffer[i] = province->cached_id & 0xffff;
            }
            else {
                tile_map->buffer[i] = ((world.get_id(province->owner) & 0xffff) << 16) | (province->cached_id & 0xffff);

            }
        }
    }

    print_info("Uploading data to OpenGL");
    UnifiedRender::TextureOptions tile_sheet_options{};
    
    this->update_provinces(&world.provinces);
    //tile_sheet->to_opengl();
    tile_sheet_options.internal_format = GL_RGBA32F;
    tile_map->to_opengl(tile_sheet_options);
    tile_map->gen_mipmaps();

    glDisable(GL_CULL_FACE);

    print_info("Creating border textures");
    border_tex = new UnifiedRender::Texture(world.width, world.height);
    UnifiedRender::TextureOptions border_tex_options{};
    border_tex_options.internal_format = GL_RGBA32F;
    border_tex_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    border_tex_options.mag_filter = GL_LINEAR;
    border_tex->to_opengl(border_tex_options);
    border_tex->gen_mipmaps();

    print_info("Creating border framebuffer");
    border_fbuffer = new UnifiedRender::OpenGl::Framebuffer();
    border_fbuffer->use();
    border_fbuffer->set_texture(0, border_tex);

    print_info("Drawing border with border shader");
    glViewport(0, 0, tile_map->width, tile_map->height);
    border_gen_shader->use();
    border_gen_shader->set_uniform("map_size", (float)tile_map->width, (float)tile_map->height);
    border_gen_shader->set_texture(0, "tile_map", tile_map);
    map_2d_quad->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    border_tex->gen_mipmaps();

    border_sdf = gen_border_sdf();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);

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
        auto flag_texture = &g_texture_manager->load_texture(path, mipmap_options);
        flag_texture->gen_mipmaps();
        nation_flags.push_back(flag_texture);
    }

    for(const auto& building_type : world.building_types) {
        std::string path;
        path = Path::get("3d/building_types/" + building_type->ref_name + ".obj");
        building_type_models.push_back(g_model_manager->load(path));
        path = Path::get("ui/building_types/" + building_type->ref_name + ".png");
        building_type_icons.push_back(&g_texture_manager->load_texture(path));
    }

    for(const auto& unit_type : world.unit_types) {
        std::string path;
        path = Path::get("3d/unit_types/" + unit_type->ref_name + ".obj");
        unit_type_models.push_back(g_model_manager->load(path));
        path = Path::get("ui/unit_types/" + unit_type->ref_name + ".png");
        unit_type_icons.push_back(&g_texture_manager->load_texture(path));
    }
}

void Map::set_view(MapView view) {
    view_mode = view;
    int old_width = camera->screen_size.x;
    int old_height = camera->screen_size.y;
    delete camera;
    if(view == MapView::PLANE_VIEW) {
        camera = new FlatCamera(old_width, old_height);
    }
    else if(view == MapView::SPHERE_VIEW) {
        camera = new OrbitCamera(old_width, old_height, 100.f);
    }
}

void Map::reload_shaders() {
    delete map_shader;
    map_shader = UnifiedRender::OpenGl::Program::create("map", "map");

    delete border_sdf_shader;
    border_sdf_shader = UnifiedRender::OpenGl::Program::create("2d_shader", "border_sdf");

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
    uint8_t max_r = 255, max_g = 229, max_b = 217;
    uint8_t min_r = 220, min_g = 46, min_b = 35;
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
    std::memset(tile_sheet->buffer, 0, sizeof(tile_sheet->buffer[0]) * (tile_sheet->width * tile_sheet->height));

    for(auto const& province_color : province_colors) {
        tile_map->buffer[province_color.first] = (0xff << 24) | province_color.second;
    }
    tile_sheet->to_opengl();
}

/** Creates the "waving" border around the continent to give it a 19th century map feel */
UnifiedRender::Texture* Map::gen_border_sdf() {
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, border_tex->width, border_tex->height);
    border_sdf_shader->use();
    border_sdf_shader->set_uniform("map_size", (float)border_tex->width, (float)border_tex->height);
    UnifiedRender::TextureOptions fbo_mipmap_options{};
    fbo_mipmap_options.internal_format = GL_RGBA32F;
    fbo_mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    fbo_mipmap_options.mag_filter = GL_LINEAR;

    UnifiedRender::Texture* tex0 = new UnifiedRender::Texture(border_tex->width, border_tex->height);
    tex0->to_opengl(fbo_mipmap_options);
    UnifiedRender::Texture* tex1 = new UnifiedRender::Texture(border_tex->width, border_tex->height);
    tex1->to_opengl(fbo_mipmap_options);

    UnifiedRender::OpenGl::Framebuffer* fbo = new UnifiedRender::OpenGl::Framebuffer();
    fbo->use();

    // Jump flooding iterations
    const int max_steps = 4.f;

    bool drawOnTex0 = true;
    for(int step = max_steps; step >= 1; step /= 2) {
        // Swap read/write buffers
        drawOnTex0 = !drawOnTex0;
        tex0->gen_mipmaps();
        tex1->gen_mipmaps();
        border_sdf_shader->set_uniform("jump", (float)step);

        fbo->set_texture(0, drawOnTex0 ? tex0 : tex1);
        if(step == max_steps){
            border_sdf_shader->set_texture(0, "tex", border_tex);
        }
        else {
            border_sdf_shader->set_texture(0, "tex", drawOnTex0 ? tex1 : tex0);
        }

        // Draw a plane over the entire screen to invoke shaders
        map_2d_quad->draw();
    }

    glFinish();
    delete fbo;
    if(drawOnTex0) {
        delete tex1;
        tex0->gen_mipmaps();
        glEnable(GL_CULL_FACE);
        return tex0;
    }
    else {
        delete tex0;
        tex1->gen_mipmaps();
        glEnable(GL_CULL_FACE);
        return tex1;
    }
}

void Map::draw_flag(const Nation* nation) {
    // Draw a flag that "waves" with some cheap wind effects it
    // looks nice and it's super cheap to make - only using sine
    const float n_steps = 8.f; // Resolution of flag in one side (in vertices)
    const float step = 90.f; // Steps per vertice

    auto flag = UnifiedRender::OpenGl::PackedModel<glm::vec3, glm::vec2>(GL_TRIANGLE_STRIP);
    for(float r = 0.f; r <= (n_steps * step); r += step) {
        float sin_r = (sin(r + wind_osc) / 24.f);

        sin_r = sin(r + wind_osc) / 24.f;
        flag.buffer.push_back(UnifiedRender::OpenGl::PackedData<glm::vec3, glm::vec2>(
            glm::vec3(((r / step) / n_steps) * 1.5f, sin_r, -2.f),
            glm::vec2((r / step) / n_steps, 0.f)
            ));

        sin_r = sin(r + wind_osc + 160.f) / 24.f;
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

    nation_flags[world.get_id(nation)]->bind();
    flag.draw();
}

#include "client/client_network.hpp"
#include "unified_render/serializer.hpp"
#include "io_impl.hpp"
#include "action.hpp"
#ifdef windows
#	define bswap_32(x) _byteswap_ulong(x)
#	define bswap_64(x) _byteswap_uint64(x)
#else
#	include <byteswap.h>
#endif

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
                gs.select_nation->change_nation(province->owner->cached_id);
            }
            break;
        case MapMode::NORMAL:
            input.selected_units.clear();
            // Check if we selected an unit
            for(const auto& unit : gs.world->units) {
                const float size = 2.f;
                std::pair<float, float> pos = unit->get_pos();
                if((int)select_pos.first > (int)pos.first - size && (int)select_pos.first < (int)pos.second + size && (int)select_pos.second >(int)pos.first - size && (int)select_pos.second < (int)pos.second + size) {
                    input.selected_units.push_back(unit);
                    return;
                }
            }
            // Show province information when clicking on a province
            if(tile.province_id < gs.world->provinces.size()) {
                new Interface::ProvinceView(gs, gs.world->provinces[tile.province_id]);
                return;
            }
            break;
        default:
            break;
        }
        return;
    }
    else if(event.button.button == SDL_BUTTON_RIGHT) {
        std::pair<float, float>& select_pos = input.select_pos;

        if(1) {
            const Tile& tile = gs.world->get_tile(input.select_pos.first, input.select_pos.second);
            if(tile.province_id == (Province::Id)-1) return;
            Province* province = gs.world->provinces[tile.province_id];

            FILE* fp = fopen("test.lua", "a+t");
            if(!fp) return;
            fprintf(fp, "{ ref_name = \"%s\", name = _(\"%s\"), color = 0x%06x },\r\n", province->ref_name.c_str(), province->name.c_str(), bswap_32((province->color & 0x00ffffff) << 8));
            fclose(fp);
        }

        for(const auto& unit : input.selected_units) {
            const Tile& tile = gs.world->get_tile(input.select_pos.first, input.select_pos.second);
            if(tile.province_id == (Province::Id)-1) return;
            unit->target = gs.world->provinces[tile.province_id];

            Packet packet = Packet();
            Archive ar = Archive();
            ActionType action = ActionType::UNIT_CHANGE_TARGET;
            ::serialize(ar, &action);
            ::serialize(ar, &unit);
            ::serialize(ar, &unit->target);
            packet.data(ar.get_buffer(), ar.size());
            std::scoped_lock lock(gs.client->pending_packets_mutex);
            gs.client->pending_packets.push_back(packet);
        }
    }
}

void Map::update(const SDL_Event& event, Input& input) {
    std::pair<int, int>& mouse_pos = input.mouse_pos;
    std::pair<float, float>& select_pos = input.select_pos;
    switch(event.type) {
    case SDL_MOUSEBUTTONDOWN:
        SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
        if(event.button.button == SDL_BUTTON_MIDDLE) {
            input.last_camera_drag_pos = camera->get_map_pos(mouse_pos);
            input.last_camera_mouse_pos = mouse_pos;

            input.drag_coord = camera->get_map_pos(mouse_pos);
            input.is_drag = true;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        input.is_drag = false;
        break;
    case SDL_MOUSEMOTION:
        SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
        if(view_mode == MapView::SPHERE_VIEW) {
            if(input.middle_mouse_down) {  // Drag the map with middlemouse
                float scale = glm::length(camera->position) / map_sphere->radius;
                float x_pos = input.last_camera_drag_pos.first - (mouse_pos.first - input.last_camera_mouse_pos.first) * 0.001 * scale;
                float y_pos = input.last_camera_drag_pos.second - (mouse_pos.second - input.last_camera_mouse_pos.second) * 0.001 * scale;
                camera->set_pos(x_pos, y_pos);
            }
            input.select_pos = camera->get_map_pos(input.mouse_pos);
            input.select_pos.first = (int)(tile_map->width * input.select_pos.first / (2. * M_PI));
            input.select_pos.second = (int)(tile_map->height * input.select_pos.second / M_PI);
        }
        else {
            if(input.middle_mouse_down) {  // Drag the map with middlemouse
                std::pair<float, float> map_pos = camera->get_map_pos(mouse_pos);
                float x_pos = camera->position.x + input.last_camera_drag_pos.first - map_pos.first;
                float y_pos = camera->position.y + input.last_camera_drag_pos.second - map_pos.second;
                camera->set_pos(x_pos, y_pos);
            }
            input.select_pos = camera->get_map_pos(input.mouse_pos);
            input.select_pos.first = (int)input.select_pos.first;
            input.select_pos.second = (int)input.select_pos.second;
        }
        break;
    case SDL_MOUSEWHEEL:
        SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
        camera->move(0.f, 0.f, event.wheel.y * 2.f);
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
    case SDL_WINDOWEVENT:
        if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int width, height;
            SDL_GetWindowSize(SDL_GetWindowFromID(event.window.windowID), &width, &height);
            camera->set_screen(width, height);
        }
        break;

    }
}

// Updates the province color texture with the changed provinces
void Map::update_provinces(const std::vector<Province*>* provinces) {
    for(unsigned int i = 0; i < 0xffff; i++) {
        tile_sheet->buffer[i] = 0x00808080;
    }

    for(const auto& province : *provinces) {
        if(province->owner == nullptr) continue;
        tile_sheet->buffer[world.get_id(province)] = province->owner->get_client_hint().color;
    }
    tile_sheet->to_opengl();
}

void Map::draw(const GameState& gs, const int width, const int height) {
    glm::mat4 view, projection;

    map_shader->use();
    view = camera->get_view();
    map_shader->set_uniform("view", view);
    map_shader->set_uniform("view_pos", camera->position.x, camera->position.y, camera->position.z);
    projection = camera->get_projection();
    map_shader->set_uniform("projection", projection);
    map_shader->set_uniform("map_size", (float)world.width, (float)world.height);

    // Map should have no "model" matrix since it's always static
    map_shader->set_texture(0, "tile_map", tile_map);
    map_shader->set_texture(1, "tile_sheet", tile_sheet);
    map_shader->set_texture(2, "water_texture", water_tex);
    map_shader->set_texture(3, "noise_texture", noise_tex);
    map_shader->set_texture(4, "terrain_texture", terrain_tex);
    map_shader->set_texture(5, "terrain_sheet", terrain_sheet);
    map_shader->set_texture(6, "topo_mapture", topo_map);
    map_shader->set_texture(7, "border_tex", border_tex);
    map_shader->set_texture(8, "border_sdf", border_sdf);
    map_shader->set_texture(9, "landscape_map", landscape_map);
    map_shader->set_texture(10, "river_texture", river_tex);

    if(view_mode == MapView::PLANE_VIEW) {
        map_quad->draw();
    }
    else if(view_mode == MapView::SPHERE_VIEW) {
        map_sphere->draw();
    }

    // TODO: We need to better this
    view = camera->get_view();
    projection = camera->get_projection();
    // obj_shader->set_uniform("map_diffusion", 0);
    model_shader->use();
    model_shader->set_uniform("projection", projection);
    model_shader->set_uniform("view", view);

    // glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);

    for(const auto& building : world.buildings) {
        glm::mat4 model(1.f);
#if defined TILE_GRANULARITY
        model = glm::translate(model, glm::vec3(building->x, building->y, 0.f));
#else
        std::pair<float, float> pos = building->get_pos();
        model = glm::translate(model, glm::vec3(pos.first, pos.second, 0.f));
#endif
        model = glm::rotate(model, 180.f, glm::vec3(1.f, 0.f, 0.f));
        model_shader->set_uniform("model", model);
        draw_flag(building->get_owner());
        building_type_models.at(world.get_id(building->type))->draw(*model_shader);
    }
    for(const auto& unit : world.units) {
        glm::mat4 model(1.f);
#if defined TILE_GRANULARITY
        model = glm::translate(model, glm::vec3(unit->x, unit->y, 0.f));
#else
        std::pair<float, float> pos = unit->get_pos();
        model = glm::translate(model, glm::vec3(pos.first, pos.second, 0.f));
#endif
        model = glm::rotate(model, 180.f, glm::vec3(1.f, 0.f, 0.f));
        model_shader->set_uniform("model", model);
        draw_flag(unit->owner);
#if defined TILE_GRANULARITY
        model = glm::rotate(model, std::atan2(unit->tx - unit->x, unit->ty - unit->y), glm::vec3(0.f, 1.f, 0.f));
#endif
        model_shader->set_uniform("model", model);
        unit_type_models[world.get_id(unit->type)]->draw(*model_shader);
    }

    glEnable(GL_CULL_FACE);

    // Resets the shader and texture
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);

    /*for(const auto& building : world.buildings) {
        glPushMatrix();
        glTranslatef(building->x, building->y, -1.f);
        const int _w = 2, _h = 2;
        nation_flags[world.get_id(building->get_owner())]->bind();
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(_w, 0.f);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_w, _h);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_w, _h);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(0.f, _h);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glEnd();
        glPopMatrix();
    }*/

    // Draw the "drag area" box
    if(gs.input.is_drag) {
        glPushMatrix();
        glTranslatef(gs.input.mouse_pos.first, gs.input.mouse_pos.second, -0.1f);
        glColor3f(1.f, 1.f, 1.f);
        glBegin(GL_LINES);
        glVertex2f(0.f, 0.f);
        glVertex2f(gs.input.drag_coord.first, 0.f);
        glVertex2f(gs.input.drag_coord.first, gs.input.drag_coord.second);
        glVertex2f(0.f, gs.input.drag_coord.second);
        glVertex2f(0.f, 0.f);
        glEnd();
        glPopMatrix();
    }

    for(const auto& unit : world.units) {
        glPushMatrix();
        std::pair<float, float> pos = unit->get_pos();
        glTranslatef(pos.first, pos.second, -0.1f);
        float _w = 2, _h = 2;
        nation_flags[world.get_id(unit->owner)]->bind();

        if(!unit->size) {
            glColor3f(0.5f, 0.5f, 0.5f);
        }
        else {
            glColor3f(1.f, 1.f, 1.f);
        }
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(_w, 0.f);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_w, _h);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_w, _h);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(0.f, _h);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glEnd();
        glPopMatrix();

        if(!unit->size) continue;

        glBindTexture(GL_TEXTURE_2D, 0);

        glPushMatrix();
#if !defined TILE_GRANULARITY
        glTranslatef(pos.first, pos.second, -0.1f);
#else
        glTranslatef(unit->x, unit->y, -0.1f);
#endif
        _w = unit->size / unit->type->max_health;
        _h = 0.5f;
        glColor3f(0.f, 1.f, 0.f);
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(_w, 0.f);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_w, _h);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_w, _h);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(0.f, _h);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glEnd();
        glPopMatrix();

        glPushMatrix();
#if !defined TILE_GRANULARITY
        glTranslatef(pos.first + (unit->size / unit->type->max_health), pos.second - 2.f, -1.f);
#else
        glTranslatef(unit->x + (unit->size / unit->type->max_health), unit->y - 2.f, -1.f);
#endif
        _w = (unit->type->max_health - unit->size) / unit->type->max_health;
        _h = 0.5f;
        glColor3f(1.f, 0.f, 0.f);
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(_w, 0.f);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_w, _h);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_w, _h);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(0.f, _h);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glEnd();
        glPopMatrix();

#if defined TILE_GRANULARITY
        glBegin(GL_LINES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex2f(unit->x, unit->y);
        glColor3f(0.f, 1.f, 0.f);
        glVertex2f(unit->tx, unit->ty);
        glEnd();
#else
        if(unit->target != nullptr) {
            std::pair<float, float> pos = unit->get_pos();
            glBegin(GL_LINES);
            glColor3f(1.f, 0.f, 0.f);
            glVertex2f(pos.first, pos.second);
            glColor3f(0.f, 1.f, 0.f);
            glVertex2f(unit->target->min_x + ((unit->target->max_x - unit->target->min_x) / 2.f), unit->target->min_y + ((unit->target->max_y - unit->target->min_y) / 2.f));
            glEnd();
        }
#endif
    }

    wind_osc += 1.f;
    if(wind_osc >= 180.f) wind_osc = 0.f;
}
