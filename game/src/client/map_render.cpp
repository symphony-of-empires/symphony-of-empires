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
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <mutex>
#include <memory>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>

#include "unified_render/texture.hpp"
#include "unified_render/primitive.hpp"
#include "unified_render/shader.hpp"
#include "unified_render/framebuffer.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/state.hpp"

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
    map_quad = new UnifiedRender::Square(0.f, 0.f, world.width, world.height);
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
    mipmap_options.internal_format = GL_RGB;
    water_tex = &tex_man->load(Path::get("water_tex.png"), mipmap_options);
    landscape_map = &tex_man->load(Path::get("map_col.png"), mipmap_options);
    wave1 = &tex_man->load(Path::get("wave1.png"), mipmap_options);
    wave2 = &tex_man->load(Path::get("wave2.png"), mipmap_options);
    // normal = &g_texture_manager->load_texture(Path::get("normal.png"), mipmap_options);
    // topo_map = &g_texture_manager->load_texture(Path::get("topo.png"), mipmap_options);
    mipmap_options.internal_format = GL_RED;
    bathymethry = &tex_man->load(Path::get("bathymethry.png"), mipmap_options);
    noise_tex = &tex_man->load(Path::get("noise_tex.png"), mipmap_options);
    river_tex = &tex_man->load(Path::get("river_smal_smooth.png"), mipmap_options);
    // terrain_map = &g_texture_manager->load_texture(Path::get("terrain_map.png"), single_color);
    terrain_map = new UnifiedRender::Texture(Path::get("terrain_map.png"));
    size_t terrain_map_size = terrain_map->width * terrain_map->height;
    for(unsigned int i = 0; i < terrain_map_size; i++) {
        uint32_t color = terrain_map->buffer.get()[i];
        uint32_t base_index = 0xFF000000;
        if(color == 0xFF000000) {
            terrain_map->buffer.get()[i] = base_index + 0; // Ocean
        }
        else if(color == 0xFFFF00FF) {
            terrain_map->buffer.get()[i] = base_index + 1; // Lake 
        }
        else if(color == 0xFFFFFFFF) {
            terrain_map->buffer.get()[i] = base_index + 2; // Land
        }
    }
    UnifiedRender::TextureOptions single_color{};
    single_color.internal_format = GL_RED;
    terrain_map->to_opengl(single_color);
    terrain_map->gen_mipmaps();

    auto topo_map = std::unique_ptr<UnifiedRender::Texture>(new UnifiedRender::Texture(Path::get("topo.png")));
    normal_topo = new UnifiedRender::Texture(Path::get("normal.png"));
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
    // terrain_sheet = new UnifiedRender::TextureArray(Path::get("terrain_sheet.png"), 4, 4);
    // terrain_sheet->to_opengl();

    // The map shader that draws everything on the map 
    map_shader = UnifiedRender::OpenGL::Program::create(options.get_options(), "shaders/map.vs", "shaders/map.fs");
    border_gen_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_gen.fs");
    border_sdf_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_sdf.fs");
    output_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_sdf_output.fs");

    print_info("Creating tile map & tile sheet");

    // The province & nation mapping
    // Alpha & Blue is for nation id
    // Red & Green is for province id
    tile_map = new UnifiedRender::Texture(world.width, world.height);
    for(size_t i = 0; i < world.width * world.height; i++) {
        const Tile& tile = world.get_tile(i);
        if(tile.province_id >= (Province::Id)-3) {
            tile_map->buffer.get()[i] = (tile.province_id & 0xffff);
        }
        else {
            auto province = world.provinces[tile.province_id];
            if(province->owner == nullptr) {
                tile_map->buffer.get()[i] = province->cached_id & 0xffff;
            }
            else {
                tile_map->buffer.get()[i] = ((world.get_id(province->owner) & 0xffff) << 16) | (province->cached_id & 0xffff);
            }
        }
    }

    UnifiedRender::TextureOptions tile_map_options{};
    tile_map_options.internal_format = GL_RGBA32F;
    tile_map->to_opengl(tile_map_options);
    tile_map->gen_mipmaps();

    // Texture holding each province color
    // The x & y coords are the province Red & Green color of the tile_map
    tile_sheet = new UnifiedRender::Texture(256, 256);
    for(unsigned int i = 0; i < 256 * 256; i++) {
        tile_sheet->buffer.get()[i] = 0xffdddddd;
    }
    tile_sheet->to_opengl();

    print_info("Creating border textures");

    if(options.sdf.used) {
        border_sdf = gen_border_sdf();
    }
}


void MapRender::reload_shaders() {
    map_shader = UnifiedRender::OpenGL::Program::create(options.get_options(), "shaders/map.vs", "shaders/map.fs");
    border_sdf_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_sdf.fs");
    border_gen_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_gen.fs");
    output_shader = UnifiedRender::OpenGL::Program::create("shaders/2d_shader.vs", "shaders/border_sdf_output.fs");

    border_sdf.reset(nullptr);
    if(options.sdf.used) {
        border_sdf = gen_border_sdf();
    }
}

#include "client/game_state.hpp"

// Creates the "waving" border around the continent to give it a 19th century map feel
// Generate a distance field to from each border using the jump flooding algorithm
// Used to create borders thicker than one tile
std::unique_ptr<UnifiedRender::Texture> MapRender::gen_border_sdf() {
    glDisable(GL_CULL_FACE);
    auto border_tex = new UnifiedRender::Texture(world.width, world.height);
    UnifiedRender::TextureOptions border_tex_options{};
    border_tex_options.internal_format = GL_RGBA32F;
    border_tex_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    border_tex_options.mag_filter = GL_LINEAR;
    border_tex->to_opengl(border_tex_options);
    border_tex->gen_mipmaps();

    print_info("Creating border framebuffer");
    auto border_fbuffer = UnifiedRender::OpenGL::Framebuffer();
    border_fbuffer.use();
    border_fbuffer.set_texture(0, *border_tex);

    print_info("Drawing border with border shader");
    glViewport(0, 0, tile_map->width, tile_map->height);
    border_gen_shader->use();
    border_gen_shader->set_uniform("map_size", (float)tile_map->width, (float)tile_map->height);
    border_gen_shader->set_texture(0, "tile_map", *tile_map);
    border_gen_shader->set_texture(1, "terrain_map", *terrain_map);
    map_2d_quad->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    border_tex->gen_mipmaps();

    glViewport(0, 0, border_tex->width, border_tex->height);
    border_sdf_shader->use();
    border_sdf_shader->set_uniform("map_size", (float)border_tex->width, (float)border_tex->height);
    UnifiedRender::TextureOptions fbo_mipmap_options{};
    fbo_mipmap_options.internal_format = GL_RGB32F;
    fbo_mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    fbo_mipmap_options.mag_filter = GL_LINEAR;

    // The Red & Green color channels are the coords on the map
    // The Blue is the distance to a border
    auto tex0 = std::unique_ptr<UnifiedRender::Texture>(new UnifiedRender::Texture(border_tex->width, border_tex->height));
    tex0->to_opengl(fbo_mipmap_options);

    auto tex1 = std::unique_ptr<UnifiedRender::Texture>(new UnifiedRender::Texture(border_tex->width, border_tex->height));
    tex1->to_opengl(fbo_mipmap_options);

    UnifiedRender::OpenGL::Framebuffer fbo = UnifiedRender::OpenGL::Framebuffer();
    fbo.use();

    // Jump flooding iterations, each step give a distance field 2^steps pixels away from the border
    const int max_steps = 4.f;

    bool drawOnTex0 = true;
    for(int step = max_steps; step >= 1; step /= 2) {
        // Swap read/write buffers
        drawOnTex0 = !drawOnTex0;
        tex0->gen_mipmaps();
        tex1->gen_mipmaps();
        border_sdf_shader->set_uniform("jump", (float)step);

        fbo.set_texture(0, drawOnTex0 ? *tex0 : *tex1);
        if(step == max_steps){
            border_sdf_shader->set_texture(0, "tex", *border_tex);
        }
        else {
            border_sdf_shader->set_texture(0, "tex", drawOnTex0 ? *tex1 : *tex0);
        }
        // Draw a plane over the entire screen to invoke shaders
        map_2d_quad->draw();
    }

    // Delete the textures no used from memory
    delete border_tex;
    if(drawOnTex0) {
        tex1.reset(nullptr);
        tex1 = std::move(tex0);
    }
    else {
        tex0.reset(nullptr);
    }
    tex1->gen_mipmaps();

    UnifiedRender::OpenGL::Framebuffer output_fbo = UnifiedRender::OpenGL::Framebuffer();
    output_fbo.use();
    output_shader->use();

    UnifiedRender::TextureOptions output_options{};
    output_options.format = GL_RED;
    output_options.internal_format = GL_RED;
    output_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    output_options.mag_filter = GL_LINEAR;
    tex0.reset(new UnifiedRender::Texture(tex1->width, tex1->height));
    tex0->to_opengl(output_options);

    output_fbo.set_texture(0, *tex0);
    output_shader->set_texture(0, "tex", *tex1);
    map_2d_quad->draw();
    glFinish();
    tex0->gen_mipmaps();
    tex1.reset(nullptr);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Copy the object
    return tex0;
}

// Updates the province color texture with the changed provinces 
void MapRender::update_mapmode(std::vector<ProvinceColor> province_colors) {
    for(auto const& province_color : province_colors) {
        tile_sheet->buffer.get()[province_color.id] = province_color.color.get_value();
    }
    tile_sheet->to_opengl();
}

void MapRender::draw(Camera* camera, MapView view_mode) {
    glm::mat4 view, projection;

    map_shader->use();
    view = camera->get_view();
    map_shader->set_uniform("view", view);
    glm::vec3 cam_pos = camera->get_world_pos();
    map_shader->set_uniform("view_pos", cam_pos.x, cam_pos.y, cam_pos.z);
    projection = camera->get_projection();
    map_shader->set_uniform("projection", projection);
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

    // Temporary not in use
    // map_shader->set_texture(5, "terrain_sheet", terrain_sheet);
    if(options.sdf.used) {
        map_shader->set_texture(6, "border_sdf", *(border_sdf.get())); // 1 col
    }

    map_shader->set_texture(6, "bathymethry", *bathymethry); // 1 col

    if(options.landscape.used) {
        map_shader->set_texture(8, "landscape_map", *landscape_map); // 3 col
    }

    if(options.rivers.used) {
        map_shader->set_texture(9, "river_texture", *river_tex); // 1 col
    }

    if(options.lighting.used) {
        map_shader->set_texture(10, "wave1", *wave1);
        map_shader->set_texture(11, "wave2", *wave2);
        map_shader->set_texture(12, "normal", *normal_topo); // 3 col
    }

    if(view_mode == MapView::PLANE_VIEW) {
        map_quad->draw();
    }
    else if(view_mode == MapView::SPHERE_VIEW) {
        map_sphere->draw();
    }
}
