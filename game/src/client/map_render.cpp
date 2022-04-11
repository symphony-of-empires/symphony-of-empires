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

#include "unified_render/texture.hpp"
#include "unified_render/primitive.hpp"
#include "unified_render/shader.hpp"
#include "unified_render/framebuffer.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/state.hpp"
#include "unified_render/utils.hpp"

#include "map.hpp"
#include "world.hpp"
#include "province.hpp"
#include "client/map_render.hpp"
#include "client/map.hpp"
#include "client/orbit_camera.hpp"
#include "client/flat_camera.hpp"
#include "client/camera.hpp"

MapRender::MapRender(const World& _world)
    : world(_world)
{
    // Flat surface for drawing flat map 
    for(int x = -1; x <= 1; x++) {
        map_quads.push_back(new UnifiedRender::Square((int)world.width * x, 0.f, (int)world.width * (x + 1), world.height));
    }

    // Sphere surface for drawing globe map
    map_sphere = new UnifiedRender::Sphere(0.f, 0.f, 0.f, GLOBE_RADIUS, 100);

    // Simple 2D quad that fills viewport, used for making the border_sdf
    map_2d_quad = new UnifiedRender::Quad2D();

    // Mipmapped textures
    UnifiedRender::TextureOptions mipmap_options{};
    mipmap_options.wrap_s = GL_REPEAT;
    mipmap_options.wrap_t = GL_REPEAT;
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    auto tex_man = UnifiedRender::State::get_instance().tex_man;

    wave1 = &tex_man->load(Path::get("gfx/wave1.png"), mipmap_options);
    wave2 = &tex_man->load(Path::get("gfx/wave2.png"), mipmap_options);
    noise_tex = &tex_man->load(Path::get("gfx/noise_tex.png"), mipmap_options);
    mipmap_options.internal_format = GL_SRGB;


    water_tex = &tex_man->load(Path::get("gfx/water_tex.png"), mipmap_options);
    paper_tex = &tex_man->load(Path::get("gfx/paper.png"), mipmap_options);
    stripes_tex = &tex_man->load(Path::get("gfx/stripes.png"), mipmap_options);
    mipmap_options.internal_format = GL_RED;
    bathymethry = &tex_man->load(Path::get("map/bathymethry.png"), mipmap_options);
    river_tex = &tex_man->load(Path::get("map/river_smooth.png"), mipmap_options);

    terrain_map = new UnifiedRender::Texture(Path::get("map/color.png"));
    size_t terrain_map_size = terrain_map->width * terrain_map->height;
    for(unsigned int i = 0; i < terrain_map_size; i++) {
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
    UnifiedRender::TextureOptions single_color{};
    single_color.internal_format = GL_RGB;
    terrain_map->to_opengl(single_color);
    //terrain_map->gen_mipmaps();

    auto topo_map = std::unique_ptr<UnifiedRender::Texture>(new UnifiedRender::Texture(Path::get("map/topo.png")));
    normal_topo = new UnifiedRender::Texture(Path::get("map/normal.png"));
    size_t map_size = topo_map->width * topo_map->height;
    for(unsigned int i = 0; i < map_size; i++) {
        normal_topo->buffer.get()[i] &= (0x00FFFFFF);
        normal_topo->buffer.get()[i] |= (topo_map->buffer.get()[i] & 0xFF) << 24;
    }
    topo_map.reset(nullptr);
    mipmap_options.internal_format = GL_RGBA;
    normal_topo->to_opengl(mipmap_options);
    normal_topo->gen_mipmaps();

    // Terrain textures to sample from
    terrain_sheet = new UnifiedRender::TextureArray(Path::get("gfx/terrain_sheet.png"), 4, 4);
    terrain_sheet->to_opengl();

    print_info("Creating tile map & tile sheet");
    // The tile map, used to store per-tile information
    tile_map = new UnifiedRender::Texture(world.width, world.height);
    for(size_t i = 0; i < world.width * world.height; i++) {
        const Tile& tile = world.get_tile(i);
        tile_map->buffer.get()[i] = tile.province_id & 0xffff;
    }
    UnifiedRender::TextureOptions tile_map_options{};
    tile_map_options.internal_format = GL_RGBA32F;
    tile_map_options.editable = true;
    tile_map->to_opengl(tile_map_options);
    tile_map->gen_mipmaps();

    // Texture holding each province color
    // The x & y coords are the province Red & Green color of the tile_map
    tile_sheet = new UnifiedRender::Texture(256, 256);
    for(unsigned int i = 0; i < 256 * 256; i++) {
        tile_sheet->buffer.get()[i] = 0xffdddddd;
    }

    tile_sheet_nation = new UnifiedRender::Texture(256, 256);
    for(unsigned int i = 0; i < 256 * 256; i++) {
        tile_sheet_nation->buffer.get()[i] = 0xffdddddd;
    }

    // By default textures will be dropped from the CPU in order to save memory, however we're trying
    // to make a buffer-texture so we have to keep it or we will have trouble
    UnifiedRender::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    no_drop_options.internal_format = GL_SRGB;
    tile_sheet->to_opengl(no_drop_options);

    // Province options
    province_opt = new UnifiedRender::Texture(256, 256);
    for(unsigned int i = 0; i < 256 * 256; i++) {
        province_opt->buffer.get()[i] = 0x000000ff;
    }
    {
        UnifiedRender::TextureOptions no_drop_options{};
        no_drop_options.editable = true;
        province_opt->to_opengl(no_drop_options);
    }

    update_nations(world.provinces);

    // The map shader that draws everything on the map 
    reload_shaders();

    print_info("Creating border textures");
    UnifiedRender::TextureOptions sdf_options{};
    sdf_options.wrap_s = GL_REPEAT;
    sdf_options.wrap_t = GL_REPEAT;
    sdf_options.internal_format = GL_RGB32F;
    sdf_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    sdf_options.mag_filter = GL_LINEAR;
    border_sdf = std::make_unique<UnifiedRender::Texture>(UnifiedRender::Texture(Path::get("map/sdf_map.png")));
    border_sdf->to_opengl(sdf_options);
    border_sdf->gen_mipmaps();
}

void MapRender::reload_shaders() {
    //map_shader = UnifiedRender::OpenGL::Program::create(options.get_options(), "shaders/map.vs", "shaders/map.fs");
    map_shader = std::unique_ptr<UnifiedRender::OpenGL::Program>(new UnifiedRender::OpenGL::Program());
    {
        std::vector<UnifiedRender::OpenGL::GLSL_Define> defined_options;
        for(auto& option : options.get_options()) {
            if(option.used) {
                UnifiedRender::OpenGL::GLSL_Define defined_option;
                defined_option.name = option.get_option();
                defined_options.push_back(defined_option);
            }
        }

        auto vs_shader = UnifiedRender::OpenGL::VertexShader(Path::cat_strings(Path::get_data("shaders/map.vs")));
        map_shader->attach_shader(&vs_shader);
        auto fs_shader = UnifiedRender::OpenGL::FragmentShader(Path::cat_strings(Path::get_data("shaders/map.fs")), true, defined_options);
        map_shader->attach_shader(&fs_shader);
        map_shader->attach_shader(UnifiedRender::State::get_instance().builtin_shaders["fs_lib"].get());
        map_shader->link();
    }
    //border_gen_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_gen.fs");
    border_gen_shader = std::unique_ptr<UnifiedRender::OpenGL::Program>(new UnifiedRender::OpenGL::Program());
    {
        auto vs_shader = UnifiedRender::OpenGL::VertexShader(Path::cat_strings(Path::get_data("shaders/2d_scale.vs")));
        border_gen_shader->attach_shader(&vs_shader);
        auto fs_shader = UnifiedRender::OpenGL::FragmentShader(Path::cat_strings(Path::get_data("shaders/border_gen.fs")));
        border_gen_shader->attach_shader(&fs_shader);
        border_gen_shader->link();
    }
    //sdf_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_sdf.fs");
    sdf_shader = std::unique_ptr<UnifiedRender::OpenGL::Program>(new UnifiedRender::OpenGL::Program());
    {
        auto vs_shader = UnifiedRender::OpenGL::VertexShader(Path::cat_strings(Path::get_data("shaders/2d_scale.vs")));
        sdf_shader->attach_shader(&vs_shader);
        auto fs_shader = UnifiedRender::OpenGL::FragmentShader(Path::cat_strings(Path::get_data("shaders/border_sdf.fs")));
        sdf_shader->attach_shader(&fs_shader);
        sdf_shader->link();
    }
    //output_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_sdf_output.fs");
    output_shader = std::unique_ptr<UnifiedRender::OpenGL::Program>(new UnifiedRender::OpenGL::Program());
    {
        output_shader->attach_shader(UnifiedRender::State::get_instance().builtin_shaders["vs_2d"].get());
        auto fs_shader = UnifiedRender::OpenGL::FragmentShader(Path::cat_strings(Path::get_data("shaders/border_sdf_output.fs")));
        output_shader->attach_shader(&fs_shader);
        output_shader->link();
    }
}

void MapRender::update_options(MapOptions new_options) {
    //map_shader = UnifiedRender::OpenGL::Program::create(options.get_options(), "shaders/map.vs", "shaders/map.fs");
    map_shader = std::unique_ptr<UnifiedRender::OpenGL::Program>(new UnifiedRender::OpenGL::Program());
    {
        std::vector<UnifiedRender::OpenGL::GLSL_Define> defined_options;
        for(auto& option : options.get_options()) {
            if(option.used) {
                UnifiedRender::OpenGL::GLSL_Define defined_option;
                defined_option.name = option.get_option();
                defined_options.push_back(defined_option);
            }
        }

        auto vs_shader = UnifiedRender::OpenGL::VertexShader(Path::cat_strings(Path::get_data("shaders/map.vs")));
        map_shader->attach_shader(&vs_shader);
        auto fs_shader = UnifiedRender::OpenGL::FragmentShader(Path::cat_strings(Path::get_data("shaders/map.fs")), true, defined_options);
        map_shader->attach_shader(&fs_shader);
        map_shader->attach_shader(UnifiedRender::State::get_instance().builtin_shaders["fs_lib"].get());
        map_shader->link();
    }
}

#include "client/game_state.hpp"

// Creates the "waving" border around the continent to give it a 19th century map feel
// Generate a distance field to from each border using the jump flooding algorithm
// Used to create borders thicker than one tile
void MapRender::update_border_sdf(UnifiedRender::Rect update_area) {
    glEnable(GL_SCISSOR_TEST);
    glViewport(update_area.left, update_area.top, update_area.width(), update_area.height());
    glScissor(update_area.left, update_area.top, update_area.width(), update_area.height());
    auto border_tex = UnifiedRender::Texture(world.width, world.height);
    UnifiedRender::TextureOptions border_tex_options{};
    border_tex_options.internal_format = GL_RGBA32F;
    border_tex_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    border_tex_options.mag_filter = GL_LINEAR;
    border_tex_options.editable = true;
    border_tex.to_opengl(border_tex_options);
    border_tex.gen_mipmaps();

    float width = world.width;
    float height = world.height;

    auto tex_coord_scale = update_area;
    tex_coord_scale.scale(1.f / glm::vec2(width, height));

    print_info("Creating border framebuffer");
    auto border_fbuffer = UnifiedRender::OpenGL::Framebuffer();
    border_fbuffer.use();
    border_fbuffer.set_texture(0, border_tex);

    print_info("Drawing border with border shader");
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
    UnifiedRender::TextureOptions fbo_mipmap_options{};
    fbo_mipmap_options.internal_format = GL_RGB32F;
    fbo_mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    fbo_mipmap_options.mag_filter = GL_LINEAR;
    fbo_mipmap_options.editable = true;

    // The Red & Green color channels are the coords on the map
    // The Blue is the distance to a border
    if(border_sdf == nullptr) {
        border_sdf = std::unique_ptr<UnifiedRender::Texture>(new UnifiedRender::Texture(border_tex.width, border_tex.height));
        border_sdf->to_opengl(fbo_mipmap_options);
    }

    auto swap_tex = std::unique_ptr<UnifiedRender::Texture>(new UnifiedRender::Texture(width, height));
    swap_tex->to_opengl(fbo_mipmap_options);

    UnifiedRender::OpenGL::Framebuffer fbo = UnifiedRender::OpenGL::Framebuffer();
    fbo.use();

    // Jump flooding iterations, each step give a distance field 2^steps pixels away from the border
    const float max_steps = 7.f;
    const float max_dist = std::pow(2, max_steps);
    sdf_shader->set_uniform("max_dist", max_dist);

    bool drawOnTex0 = true;
    for(int step = max_dist; step >= 1; step /= 2) {
        // Swap read/write buffers
        drawOnTex0 = !drawOnTex0;
        border_sdf->gen_mipmaps();
        swap_tex->gen_mipmaps();
        sdf_shader->set_uniform("jump", (float)step);

        fbo.set_texture(0, drawOnTex0 ? *border_sdf : *swap_tex);
        if(step == max_dist) {
            sdf_shader->set_texture(0, "tex", border_tex);
        } else {
            sdf_shader->set_texture(0, "tex", drawOnTex0 ? *swap_tex : *border_sdf);
        }
        // Draw a plane over the entire screen to invoke shaders
        map_2d_quad->draw();
    }

    // Delete the textures no used from memory
    if(drawOnTex0) {
        swap_tex.reset();
    } else {
        border_sdf.reset();
        border_sdf = std::move(swap_tex);
    }
    border_sdf->gen_mipmaps();
    glDisable(GL_SCISSOR_TEST);
    return;
}

// Updates the province color texture with the changed provinces 
void MapRender::update_mapmode(std::vector<ProvinceColor> province_colors) {
    for(auto const& province_color : province_colors) {
        tile_sheet->buffer.get()[province_color.id] = province_color.color.get_value();
    }
    UnifiedRender::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    no_drop_options.internal_format = GL_SRGB;
    tile_sheet->to_opengl(no_drop_options);
}

// Updates nations
void MapRender::update_nations(std::vector<Province*> provinces) {
    for(auto const& province : provinces) {
        if(province->controller == nullptr) {
            continue;
        }
        this->tile_sheet_nation->buffer.get()[province->cached_id] = province->controller->cached_id;
    }

    UnifiedRender::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    this->tile_sheet_nation->to_opengl(no_drop_options);
}

void MapRender::request_update_visibility(void)
{
    this->req_update_vision = true;
}

void MapRender::update_visibility(void)
{
    const auto& gs = (GameState&)GameState::get_instance();
    if(gs.curr_nation == nullptr) {
        return;
    }

    // TODO: Check that unit is allied with us/province owned by an ally

    UnifiedRender::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    for(unsigned int i = 0; i < 256 * 256; i++) {
        province_opt->buffer.get()[i] = 0x00000080;
    }
    for(const auto& province : gs.curr_nation->controlled_provinces) {
        this->province_opt->buffer.get()[gs.world->get_id(*province)] = 0x000000ff;
        for(const auto& neighbour : province->neighbours) {
            this->province_opt->buffer.get()[gs.world->get_id(*neighbour)] = 0x000000ff;
        }
    }

    for(const auto& unit : gs.world->units) {
        // Unit must be ours
        if(unit->owner != gs.curr_nation) {
            continue;
        }

        this->province_opt->buffer.get()[gs.world->get_id(*unit->province)] = 0x000000ff;
        for(const auto& neighbour : unit->province->neighbours) {
            this->province_opt->buffer.get()[gs.world->get_id(*neighbour)] = 0x000000ff;
        }
    }
    this->province_opt->to_opengl(no_drop_options);
}

void MapRender::draw(Camera* camera, MapView view_mode) {
    if(this->req_update_vision) {
        this->update_visibility();
        this->req_update_vision = false;
    }

    glm::mat4 view, projection;

    map_shader->use();
    view = camera->get_view();
    map_shader->set_uniform("view", view);
    glm::vec3 cam_pos = camera->get_world_pos();
    map_shader->set_uniform("view_pos", cam_pos.x, cam_pos.y, cam_pos.z);
    projection = camera->get_projection();
    map_shader->set_uniform("projection", projection);
    glm::vec3 map_pos = camera->get_map_pos();
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
    if(options.noise.used) {
        map_shader->set_texture(3, "noise_texture", *noise_tex);
    }
    map_shader->set_texture(4, "terrain_map", *terrain_map); // 1 col

    if(options.sdf.used) {
        map_shader->set_texture(6, "border_sdf", *(border_sdf.get())); // 1 col
    }

    map_shader->set_texture(7, "bathymethry", *bathymethry); // 1 col

    if(options.rivers.used) {
        map_shader->set_texture(9, "river_texture", *river_tex); // 1 col
    }

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
        for(size_t i = 0; i < map_quads.size(); i++) {
            map_quads[i]->draw();
        }
    } else if(view_mode == MapView::SPHERE_VIEW) {
        map_sphere->draw();
    }
}
