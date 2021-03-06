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
//      client/map_render.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <memory>
#include <chrono>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>

#include "eng3d/texture.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/framebuffer.hpp"
#include "eng3d/path.hpp"
#include "eng3d/state.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"
#include "eng3d/orbit_camera.hpp"
#include "eng3d/flat_camera.hpp"
#include "eng3d/camera.hpp"

#include "game_state.hpp"
#include "map.hpp"
#include "world.hpp"
#include "province.hpp"
#include "client/map_render.hpp"
#include "client/map.hpp"

MapRender::MapRender(const World& _world)
    : world(_world)
{
    // Flat surface for drawing flat map 
    for(int x = -1; x <= 1; x++) {
        map_quads.push_back(new Eng3D::Square((int)world.width * x, 0.f, (int)world.width * (x + 1), world.height));
    }

    // Sphere surface for drawing globe map
    map_sphere = new Eng3D::Sphere(0.f, 0.f, 0.f, GLOBE_RADIUS, 100);

    // Simple 2D quad that fills viewport, used for making the border_sdf
    map_2d_quad = new Eng3D::Quad2D();

    auto& gs = (GameState&)Eng3D::State::get_instance();
    auto& tex_man = gs.tex_man;

    // Mipmapped textures
    Eng3D::TextureOptions mipmap_options{};
    mipmap_options.wrap_s = GL_REPEAT;
    mipmap_options.wrap_t = GL_REPEAT;
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    mipmap_options.compressed = false;

    noise_tex = tex_man.load(gs.package_man.get_unique("gfx/noise_tex.png"), mipmap_options);

    mipmap_options.compressed = true;

    wave1 = tex_man.load(gs.package_man.get_unique("gfx/wave1.png"), mipmap_options);
    wave2 = tex_man.load(gs.package_man.get_unique("gfx/wave2.png"), mipmap_options);

    mipmap_options.internal_format = GL_SRGB;
    water_tex = tex_man.load(gs.package_man.get_unique("gfx/water_tex.png"), mipmap_options);
    paper_tex = tex_man.load(gs.package_man.get_unique("gfx/paper.png"), mipmap_options);
    stripes_tex = tex_man.load(gs.package_man.get_unique("gfx/stripes.png"), mipmap_options);

    mipmap_options.internal_format = GL_RED;
    bathymethry = tex_man.load(gs.package_man.get_unique("map/bathymethry.png"), mipmap_options);
    river_tex = tex_man.load(gs.package_man.get_unique("map/river_smooth.png"), mipmap_options);

    terrain_map = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(gs.package_man.get_unique("map/color.png")->get_abs_path()));
    size_t terrain_map_size = terrain_map->width * terrain_map->height;
    for(size_t i = 0; i < terrain_map_size; i++) {
        const uint32_t color = terrain_map->buffer.get()[i];
        uint8_t idx = 0;
        switch(bswap32(color << 8)) {
        case 0x18200b:
            idx = 0 * 16;
            break;
        case 0x4b482a:
            idx = 1 * 16;
            break;
        case 0x273214:
            idx = 3 * 16;
            break;
        case 0x9c8461:
        case 0xbfa178:
            idx = 6 * 16;
            break;
        case 0x969a9a:
        case 0x686963:
        case 0xffffff:
            idx = 8 * 16;
            break;
        case 0x614a2f:
            idx = 9 * 16;
            break;
        case 0x37311b:
        case 0x7a6342:
            idx = 10 * 16;
            break;
        default:
            idx = 0;
            break;
        }

        terrain_map->buffer.get()[i] = idx << 8;
        switch(bswap32(color << 8)) {
        case 0x243089:
            terrain_map->buffer.get()[i] |= 0x00; // Ocean
            break;
        //case 0x243089:
        //    terrain_map->buffer.get()[i] |= 0x01; // Lake
        //    break;
        default:
            terrain_map->buffer.get()[i] |= 0x02; // Land
            break;
        }
    }

    Eng3D::TextureOptions single_color{};
    single_color.internal_format = GL_RGBA;
    single_color.compressed = false;
    terrain_map->upload(single_color);

    auto topo_map = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(gs.package_man.get_unique("map/topo.png")->get_abs_path()));
    normal_topo = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(gs.package_man.get_unique("map/normal.png")->get_abs_path()));
    size_t map_size = topo_map->width * topo_map->height;
    for(unsigned int i = 0; i < map_size; i++) {
        normal_topo->buffer.get()[i] &= (0x00FFFFFF);
        normal_topo->buffer.get()[i] |= (topo_map->buffer.get()[i] & 0xFF) << 24;
    }
    topo_map.reset(nullptr);
    mipmap_options.internal_format = GL_RGBA;
    mipmap_options.compressed = false;
    normal_topo->upload(mipmap_options);

    // Terrain textures to sample from
    terrain_sheet = std::unique_ptr<Eng3D::TextureArray>(new Eng3D::TextureArray(gs.package_man.get_unique("gfx/terrain_sheet.png")->get_abs_path(), 4, 4));
    terrain_sheet->upload();

    Eng3D::Log::debug("game", "Creating tile map & tile sheet");
    // The tile map, used to store per-tile information
    tile_map = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(world.width, world.height));
    for(size_t i = 0; i < world.width * world.height; i++) {
        const auto& tile = world.get_tile(i);
        tile_map->buffer.get()[i] = tile.province_id & 0xffff;
    }
    Eng3D::TextureOptions tile_map_options{};
    tile_map_options.internal_format = GL_RGBA32F;
    tile_map_options.editable = true;
    tile_map_options.compressed = false;
    tile_map->upload(tile_map_options);

    // Texture holding each province color
    // The x & y coords are the province Red & Green color of the tile_map
    tile_sheet = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(256, 256));
    for(size_t i = 0; i < 256 * 256; i++)
        tile_sheet->buffer.get()[i] = 0xffdddddd;

    tile_sheet_nation = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(256, 256));
    for(size_t i = 0; i < 256 * 256; i++)
        tile_sheet_nation->buffer.get()[i] = 0xffdddddd;

    // By default textures will be dropped from the CPU in order to save memory, however we're trying
    // to make a buffer-texture so we have to keep it or we will have trouble
    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
#ifdef E3D_BACKEND_OPENGL
    no_drop_options.internal_format = GL_SRGB_ALPHA;
#elif defined E3D_BACKEND_GLES
    no_drop_options.internal_format = GL_RGBA;
#endif
    no_drop_options.compressed = false;
    tile_sheet->upload(no_drop_options);

    // Province options
    province_opt = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(256, 256));
    for(size_t i = 0; i < 256 * 256; i++)
        province_opt->buffer.get()[i] = 0x000000ff;
    {
        Eng3D::TextureOptions no_drop_options{};
        no_drop_options.editable = true;
        province_opt->upload(no_drop_options);
    }

    std::vector<Province::Id> province_ids;
    province_ids.reserve(world.provinces.size());
    for(auto const& province : world.provinces) {
        province_ids.push_back(province.get_id());
    }
    update_nations(province_ids);

    // The map shader that draws everything on the map 
    this->reload_shaders();

    Eng3D::Log::debug("game", "Creating border textures");
    std::unique_ptr<FILE, int(*)(FILE*)> fp(::fopen("sdf_cache.png", "rb"), ::fclose);
    if(fp.get() == nullptr) {
        this->update_border_sdf(Eng3D::Rect(0, 0, gs.world->width, gs.world->height), glm::vec2(gs.width, gs.height));
        border_sdf->to_file("sdf_cache.png");
    } else {
        Eng3D::TextureOptions sdf_options{};
        sdf_options.wrap_s = GL_REPEAT;
        sdf_options.wrap_t = GL_REPEAT;
        sdf_options.internal_format = GL_RGB32F;
        sdf_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        sdf_options.mag_filter = GL_LINEAR;
        sdf_options.compressed = false;
        border_sdf = std::make_unique<Eng3D::Texture>(Eng3D::Texture("sdf_cache.png"));
        border_sdf->upload(sdf_options);
    }
}

void MapRender::reload_shaders() {
    auto& gs = Eng3D::State::get_instance();
    //map_shader = Eng3D::OpenGL::Program::create(options.get_options(), "shaders/map.vs", "shaders/map.fs");
    map_shader = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        std::vector<Eng3D::OpenGL::GLSL_Define> defined_options;
        for(auto& option : options.get_options()) {
            if(option.used) {
                Eng3D::OpenGL::GLSL_Define defined_option;
                defined_option.name = option.get_option();
                defined_options.push_back(defined_option);
            }
        }

        auto vs_shader = Eng3D::OpenGL::VertexShader(gs.package_man.get_unique("shaders/map.vs")->read_all());
        map_shader->attach_shader(vs_shader);
        auto fs_shader = Eng3D::OpenGL::FragmentShader(gs.package_man.get_unique("shaders/map.fs")->read_all(), true, defined_options);
        map_shader->attach_shader(fs_shader);
        map_shader->attach_shader(*gs.builtin_shaders["fs_lib"]);
        map_shader->link();
    }
    //border_gen_shader = Eng3D::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_gen.fs");
    border_gen_shader = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        auto vs_shader = Eng3D::OpenGL::VertexShader(gs.package_man.get_unique("shaders/2d_scale.vs")->read_all());
        border_gen_shader->attach_shader(vs_shader);
        auto fs_shader = Eng3D::OpenGL::FragmentShader(gs.package_man.get_unique("shaders/border_gen.fs")->read_all());
        border_gen_shader->attach_shader(fs_shader);
        border_gen_shader->link();
    }
    //sdf_shader = Eng3D::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_sdf.fs");
    sdf_shader = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        auto vs_shader = Eng3D::OpenGL::VertexShader(gs.package_man.get_unique("shaders/2d_scale.vs")->read_all());
        sdf_shader->attach_shader(vs_shader);
        auto fs_shader = Eng3D::OpenGL::FragmentShader(gs.package_man.get_unique("shaders/border_sdf.fs")->read_all());
        sdf_shader->attach_shader(fs_shader);
        sdf_shader->link();
    }
    //output_shader = Eng3D::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_sdf_output.fs");
    output_shader = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        output_shader->attach_shader(*gs.builtin_shaders["vs_2d"].get());
        auto fs_shader = Eng3D::OpenGL::FragmentShader(gs.package_man.get_unique("shaders/border_sdf_output.fs")->read_all());
        output_shader->attach_shader(fs_shader);
        output_shader->link();
    }
}

void MapRender::update_options(MapOptions new_options) {
    auto& gs = Eng3D::State::get_instance();
    map_shader = std::unique_ptr<Eng3D::OpenGL::Program>(new Eng3D::OpenGL::Program());
    {
        std::vector<Eng3D::OpenGL::GLSL_Define> defined_options;
        for(auto& option : options.get_options()) {
            if(option.used) {
                Eng3D::OpenGL::GLSL_Define defined_option;
                defined_option.name = option.get_option();
                defined_options.push_back(defined_option);
            }
        }

        auto vs_shader = Eng3D::OpenGL::VertexShader(gs.package_man.get_unique("shaders/map.vs")->read_all());
        map_shader->attach_shader(vs_shader);
        auto fs_shader = Eng3D::OpenGL::FragmentShader(gs.package_man.get_unique("shaders/map.fs")->read_all(), true, defined_options);
        map_shader->attach_shader(fs_shader);
        map_shader->attach_shader(*gs.builtin_shaders["fs_lib"]);
        map_shader->link();
    }
}

#include "eng3d/framebuffer.hpp"

// Creates the "waving" border around the continent to give it a 19th century map feel
// Generate a distance field to from each border using the jump flooding algorithm
// Used to create borders thicker than one tile
void MapRender::update_border_sdf(Eng3D::Rect update_area, glm::ivec2 window_size) {
    glEnable(GL_SCISSOR_TEST);
    glViewport(update_area.left, update_area.top, update_area.width(), update_area.height());
    glScissor(update_area.left, update_area.top, update_area.width(), update_area.height());

    Eng3D::TextureOptions border_tex_options{};
    border_tex_options.internal_format = GL_RGBA32F;
    border_tex_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    border_tex_options.mag_filter = GL_LINEAR;
    border_tex_options.editable = true;
    Eng3D::Texture border_tex(world.width, world.height);
    border_tex.upload(border_tex_options);
    border_tex.gen_mipmaps();

    float width = world.width;
    float height = world.height;

    auto tex_coord_scale = update_area;
    tex_coord_scale.scale(1.f / glm::vec2(width, height));

    Eng3D::Log::debug("game", "Creating border framebuffer");
    auto border_fbuffer = Eng3D::OpenGL::Framebuffer();
    border_fbuffer.use();
    border_fbuffer.set_texture(0, border_tex);

    Eng3D::Log::debug("game", "Drawing border with border shader");
    border_gen_shader->use();
    border_gen_shader->set_uniform("map_size", width, height);
    border_gen_shader->set_uniform("tex_coord_scale", tex_coord_scale.left, tex_coord_scale.top, tex_coord_scale.right, tex_coord_scale.bottom);
    border_gen_shader->set_texture(0, "tile_map", *tile_map);
    border_gen_shader->set_texture(1, "terrain_map", *terrain_map);
    border_gen_shader->set_texture(2, "tile_sheet_nation", *tile_sheet_nation);
    map_2d_quad->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    border_tex.gen_mipmaps();

    sdf_shader->use();
    sdf_shader->set_uniform("map_size", width, height);
    sdf_shader->set_uniform("tex_coord_scale", tex_coord_scale.left, tex_coord_scale.top, tex_coord_scale.right, tex_coord_scale.bottom);
    
    Eng3D::TextureOptions fbo_mipmap_options{};
    fbo_mipmap_options.internal_format = GL_RGB32F;
    fbo_mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    fbo_mipmap_options.mag_filter = GL_LINEAR;
    fbo_mipmap_options.editable = true;
    // The Red & Green color channels are the coords on the map
    // The Blue is the distance to a border
    if(border_sdf.get() == nullptr) {
        border_sdf = std::unique_ptr<Eng3D::Texture>(new Eng3D::Texture(border_tex.width, border_tex.height));
        border_sdf->upload(fbo_mipmap_options);
    }

    std::unique_ptr<Eng3D::Texture> swap_tex(new Eng3D::Texture(width, height));
    swap_tex->upload(fbo_mipmap_options);

    auto fbo = Eng3D::OpenGL::Framebuffer();
    fbo.use();

    // Jump flooding iterations, each step give a distance field 2^steps pixels away from the border
    const float max_steps = 7.f;
    const float max_dist = std::pow(2, max_steps);
    sdf_shader->set_uniform("max_dist", max_dist);

    bool draw_on_tex0 = true;
    for(int step = max_dist; step >= 1; step /= 2) {
        // Swap read/write buffers
        draw_on_tex0 = !draw_on_tex0;
        border_sdf->gen_mipmaps();
        swap_tex->gen_mipmaps();
        sdf_shader->set_uniform("jump", (float)step);

        fbo.set_texture(0, draw_on_tex0 ? *border_sdf : *swap_tex);
        if(step == max_dist)
            sdf_shader->set_texture(0, "tex", border_tex);
        else
            sdf_shader->set_texture(0, "tex", draw_on_tex0 ? *swap_tex : *border_sdf);
        // Draw a plane over the entire screen to invoke shaders
        map_2d_quad->draw();
    }

    // Delete the textures no used from memory
    if(draw_on_tex0) {
        swap_tex.reset();
    } else {
        border_sdf.reset();
        border_sdf = std::move(swap_tex);
    }
    border_sdf->gen_mipmaps();
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, window_size.x, window_size.y);
}

// Updates the province color texture with the changed provinces 
void MapRender::update_mapmode(std::vector<ProvinceColor> province_colors) {
    // Water
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        auto* terrain_type = world.provinces[i].terrain_type;
        if(terrain_type->is_water_body)
            province_colors[i] = ProvinceColor(i, Eng3D::Color::rgba32(0x00000000));
    }
    for(auto const& province_color : province_colors)
        tile_sheet->buffer.get()[province_color.id] = province_color.color.get_value();
    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    no_drop_options.internal_format = GL_SRGB;
    tile_sheet->upload(no_drop_options);
}

// Updates nations
void MapRender::update_nations(std::vector<Province::Id> province_ids) {
    for(auto const& id : province_ids) {
        auto& province = world.provinces[id]; 
        if(province.controller == nullptr) continue;
        this->tile_sheet_nation->buffer.get()[province.cached_id] = province.controller->cached_id;
    }

    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    this->tile_sheet_nation->upload(no_drop_options);
}

void MapRender::request_update_visibility()
{
    this->req_update_vision = true;
}

void MapRender::update_visibility(GameState& gs)
{
    if(gs.curr_nation == nullptr) return;

    /// @todo Check that unit is allied with us/province owned by an ally

    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    for(Province::Id i = 0; i < gs.world->provinces.size(); i++)
        province_opt->buffer[i] = 0x00000080;
    
    for(const auto& nation : gs.world->nations) {
        const auto nation_id = nation.get_id();
        // If it's our own nation or an ally of ours we can see them
        if(nation_id == gs.curr_nation->get_id() || gs.world->get_relation(nation_id, gs.curr_nation->get_id()).has_alliance) {
            for(const auto province_id : gs.world->nations[nation_id].controlled_provinces) {
                const auto& province = gs.world->provinces[province_id];
                this->province_opt->buffer[province_id] = 0x000000ff;
                for(const auto neighbour_id : province.neighbours)
                    this->province_opt->buffer[neighbour_id] = 0x000000ff;
            }
        }
    }

    gs.world->unit_manager.for_each_unit([this, &gs](Unit& unit) {
        // Unit must be ours
        if(unit.owner_id != gs.curr_nation->get_id()) {
            // Or be owned by our ally
            if(gs.world->get_relation(unit.owner_id, gs.curr_nation->get_id()).has_alliance) {
                // ...
            } else {
                return;
            }
        }
        auto prov_id = gs.world->unit_manager.get_unit_current_province(unit.cached_id);
        this->province_opt->buffer[prov_id] = 0x000000ff;
        for(const auto neighbour_id : gs.world->provinces[prov_id].neighbours)
            this->province_opt->buffer[neighbour_id] = 0x000000ff;
    });
    if(gs.map->province_selected)
        this->province_opt->buffer.get()[gs.map->selected_province_id] = 0x400000ff;
    this->province_opt->upload(no_drop_options);
}

void MapRender::update(GameState& gs) {
    if(gs.world->province_manager.is_provinces_changed())
        this->req_update_vision = true;
    
    if(this->req_update_vision) {
        this->update_visibility(gs);
        this->req_update_vision = false;
    }

    auto& changed_owner_provinces = gs.world->province_manager.get_changed_owner_provinces();
    if(!changed_owner_provinces.empty()) {
        update_nations(changed_owner_provinces);
        Eng3D::Rect update_area{ 0, 0, 0, 0 };
        for(Province::Id i = 0; i < changed_owner_provinces.size(); i++) {
            auto& province = gs.world->provinces[changed_owner_provinces[i]];
            update_area = update_area.join(province.box_area);
        }
        glm::ivec2 screen_size(gs.width, gs.height);
        this->update_border_sdf(update_area, screen_size);
    }
}

void MapRender::draw(Eng3D::Camera* camera, MapView view_mode) {
    map_shader->use();
    const glm::mat4 view = camera->get_view();
    map_shader->set_uniform("view", view);
    const glm::vec3 cam_pos = camera->get_world_pos();
    map_shader->set_uniform("view_pos", cam_pos.x, cam_pos.y, cam_pos.z);
    const glm::mat4 projection = camera->get_projection();
    map_shader->set_uniform("projection", projection);
    const glm::vec3 map_pos = camera->get_map_pos();
    float distance_to_map = map_pos.z / world.width;
    map_shader->set_uniform("dist_to_map", distance_to_map);
    map_shader->set_uniform("map_size", (float)world.width, (float)world.height);
    // A time uniform to send to the shader
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millisec_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    float time = (float)(millisec_since_epoch % 1000000) / 1000.f;
    map_shader->set_uniform("time", time);
    // Map should have no "model" matrix since it's always static
    map_shader->set_texture(0, "tile_map", *tile_map); // 4 col
    map_shader->set_texture(1, "tile_sheet", *tile_sheet);
    map_shader->set_texture(2, "water_texture", *water_tex);
    if(options.noise.used)
        map_shader->set_texture(3, "noise_texture", *noise_tex);
    map_shader->set_texture(4, "terrain_map", *terrain_map); // 1 col
    if(options.sdf.used)
        map_shader->set_texture(6, "border_sdf", *(border_sdf.get())); // 1 col
    map_shader->set_texture(7, "bathymethry", *bathymethry); // 1 col
    if(options.rivers.used)
        map_shader->set_texture(9, "river_texture", *river_tex); // 1 col
    if(options.lighting.used) {
        map_shader->set_texture(10, "wave1", *wave1);
        map_shader->set_texture(11, "wave2", *wave2);
        map_shader->set_texture(12, "normal", *normal_topo); // 3 col
    }
    map_shader->set_texture(13, "paper_tex", *paper_tex);
    // map_shader->set_texture(14, "stripes", *stripes_tex);
    map_shader->set_texture(14, "tile_sheet_nation", *tile_sheet_nation);
    map_shader->set_texture(15, "province_opt", *province_opt);
    map_shader->set_texture(16, "terrain_sheet", *terrain_sheet);

    if(view_mode == MapView::PLANE_VIEW) {
        for(const auto& map_quad : map_quads)
            map_quad->draw();
    } else if(view_mode == MapView::SPHERE_VIEW) {
        map_sphere->draw();
    }
}
