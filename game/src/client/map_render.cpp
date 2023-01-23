// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
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
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#endif

#include "eng3d/texture.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/framebuffer.hpp"
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

MapRender::MapRender(GameState& _gs, Map& _map)
    : Eng3D::BaseMap(_gs, glm::ivec2{ _gs.world->width, _gs.world->height }),
    gs{ _gs },
    map{ _map }
{
    if(this->gs.world->width != this->terrain_map->width || this->gs.world->height != this->terrain_map->height)
        CXX_THROW(std::runtime_error, "Color map differs from province map!");

    // The terrain_map has 16 bits available for usage, so naturally we use them
    // ------------------------------------------------------------
    // | 16 bit Province id | 8 bit terrain index | 8 bit "flags" |
    // ------------------------------------------------------------
    Eng3D::Log::debug("game", "Creating tile map & tile sheet");
    assert((this->terrain_map->width* this->terrain_map->height) % 5400 == 0);

    tbb::parallel_for(static_cast<size_t>(0), (this->terrain_map->width* this->terrain_map->height) / 5400, [this](const auto y) {
        const auto i = y * 5400;
        for(size_t x = 0; x < 5400; x++)
            this->terrain_map->buffer.get()[i + x] |= static_cast<size_t>(this->gs.world->tiles[i + x]) & 0xffff;
    });

    Eng3D::TextureOptions terrain_map_options{};
    terrain_map_options.internal_format = Eng3D::TextureOptions::Format::RGBA;
    terrain_map_options.editable = true;
    terrain_map_options.compressed = false;
    this->terrain_map->upload(terrain_map_options);
    // After this snippet of code we won't ever need tiles ever again
    this->gs.world->tiles.reset();

    // Texture holding each province color
    // The x & y coords are the province Red & Green color of the tile_map
    tile_sheet = std::make_unique<Eng3D::Texture>(256, 256);
    std::fill_n(tile_sheet->buffer.get(), 256 * 256, 0xffdddddd);
    tile_sheet_nation = std::make_unique<Eng3D::Texture>(256, 256);
    std::fill_n(tile_sheet_nation->buffer.get(), 256 * 256, 0xffdddddd);

    // By default textures will be dropped from the CPU in order to save memory, however we're trying
    // to make a buffer-texture so we have to keep it or we will have trouble
    {
        Eng3D::TextureOptions no_drop_options{};
        no_drop_options.editable = true;
#ifdef E3D_BACKEND_OPENGL
        no_drop_options.internal_format = Eng3D::TextureOptions::Format::SRGB_ALPHA;
#elif defined E3D_BACKEND_GLES
        no_drop_options.internal_format = Eng3D::TextureOptions::Format::RGBA;
#endif
        no_drop_options.compressed = false;
        tile_sheet->upload(no_drop_options);
    }

    // Province options
    province_opt = std::make_unique<Eng3D::Texture>(256, 256);
    for(size_t i = 0; i < 256 * 256; i++)
        province_opt->buffer.get()[i] = 0x000000ff;
    {
        Eng3D::TextureOptions no_drop_options{};
        no_drop_options.editable = true;
        province_opt->upload(no_drop_options);
    }

    std::vector<ProvinceId> province_ids;
    province_ids.reserve(this->gs.world->provinces.size());
    for(auto const& province : this->gs.world->provinces)
        province_ids.push_back(province);
    this->update_nations(province_ids);
}

void MapRender::reload_shaders() {
    border_gen_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        auto vs_shader = Eng3D::OpenGL::VertexShader(gs.package_man.get_unique("shaders/2d_scale.vs")->read_all());
        border_gen_shader->attach_shader(vs_shader);
        auto fs_shader = Eng3D::OpenGL::FragmentShader(gs.package_man.get_unique("shaders/border_gen.fs")->read_all());
        border_gen_shader->attach_shader(fs_shader);
        border_gen_shader->attach_shader(*gs.builtin_shaders["fs_lib"].get());
        border_gen_shader->link();
    }

    sdf_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        auto vs_shader = Eng3D::OpenGL::VertexShader(gs.package_man.get_unique("shaders/2d_scale.vs")->read_all());
        sdf_shader->attach_shader(vs_shader);
        auto fs_shader = Eng3D::OpenGL::FragmentShader(gs.package_man.get_unique("shaders/border_sdf.fs")->read_all());
        sdf_shader->attach_shader(fs_shader);
        sdf_shader->attach_shader(*gs.builtin_shaders["fs_lib"].get());
        sdf_shader->link();
    }

    output_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        output_shader->attach_shader(*gs.builtin_shaders["vs_2d"].get());
        auto fs_shader = Eng3D::OpenGL::FragmentShader(gs.package_man.get_unique("shaders/border_sdf_output.fs")->read_all());
        output_shader->attach_shader(fs_shader);
        output_shader->attach_shader(*gs.builtin_shaders["fs_lib"].get());
        output_shader->link();
    }
    this->update_options(this->options);
}

void MapRender::update_options(MapOptions) {
    map_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        std::vector<Eng3D::GLSL::Define> defined_options;
        for(auto& option : options.get_options()) {
            if(option.used) {
                Eng3D::GLSL::Define defined_option;
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

    if(this->options.rivers.used) {
        Eng3D::TextureOptions mipmap_options{};
        mipmap_options.wrap_s = Eng3D::TextureOptions::Wrap::REPEAT;
        mipmap_options.wrap_t = Eng3D::TextureOptions::Wrap::REPEAT;
        mipmap_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
        mipmap_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
        mipmap_options.compressed = this->options.compress.used;
        mipmap_options.internal_format = Eng3D::TextureOptions::Format::RED;
        if(this->bathymethry.get() == nullptr)
            this->bathymethry = gs.tex_man.load(gs.package_man.get_unique("map/bathymethry.png"), mipmap_options);
        if(this->river_tex.get() == nullptr)
            this->river_tex = gs.tex_man.load(gs.package_man.get_unique("map/river_smooth.png"), mipmap_options);
    } else {
        this->bathymethry.reset(new Eng3D::Texture(1, 1));
        this->river_tex.reset(new Eng3D::Texture(1, 1));
    }

    // Load normal and topographic maps only w lighting
    if(this->options.lighting.used) {
        // If reload is required
        if(this->normal_topo.get() == nullptr) {
            auto topo_map = std::make_unique<Eng3D::Texture>(gs.package_man.get_unique("map/topo.png")->abs_path);
            this->normal_topo = std::make_unique<Eng3D::Texture>(gs.package_man.get_unique("map/normal.png")->abs_path);
            size_t map_size = topo_map->width * topo_map->height;
            for(size_t i = 0; i < map_size; i++) {
                this->normal_topo->buffer.get()[i] &= (0x00FFFFFF);
                this->normal_topo->buffer.get()[i] |= (topo_map->buffer.get()[i] & 0xFF) << 24;
            }
            Eng3D::TextureOptions mipmap_options{};
            mipmap_options.internal_format = Eng3D::TextureOptions::Format::RGBA;
            mipmap_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
            mipmap_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
            mipmap_options.compressed = this->options.compress.used;
            this->normal_topo->upload(mipmap_options);
        }
    }

    // Only perform the updates/generation on the SDF map when SDF is actually used
    if(this->options.sdf.used) {
        // If reload is required
        if(this->border_sdf.get() == nullptr) {
            Eng3D::Log::debug("game", "Creating border textures");
            std::unique_ptr<FILE, int(*)(FILE*)> fp(::fopen("sdf_cache.png", "rb"), ::fclose);
            if(fp.get() == nullptr) {
                this->update_border_sdf(Eng3D::Rect(0, 0, this->gs.world->width, this->gs.world->height), glm::vec2(gs.width, gs.height));
                this->border_sdf->to_file("sdf_cache.png");
            } else {
                Eng3D::TextureOptions sdf_options{};
                sdf_options.wrap_s = Eng3D::TextureOptions::Wrap::REPEAT;
                sdf_options.wrap_t = Eng3D::TextureOptions::Wrap::REPEAT;
                sdf_options.internal_format = Eng3D::TextureOptions::Format::RGB32F;
                sdf_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
                sdf_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
                sdf_options.compressed = this->options.compress.used;
                this->border_sdf = std::make_unique<Eng3D::Texture>("sdf_cache.png");
                this->border_sdf->upload(sdf_options);
            }
            // We are already updating the whole map, don't do it twice
            this->gs.world->province_manager.clear();
        }
    }
}

/// @brief Creates the "waving" border around the continent to give it a 19th century map feel
/// Generate a distance field to from each border using the jump flooding algorithm
/// Used to create borders thicker than one tile
void MapRender::update_border_sdf(Eng3D::Rect update_area, glm::ivec2 window_size) {
    glEnable(GL_SCISSOR_TEST);
    
    float width = glm::min(this->gs.max_texture_size, this->gs.world->width);
    float height = glm::min(this->gs.max_texture_size, this->gs.world->height);
    
    float w_ratio = width / this->gs.world->width;
    float h_ratio = height / this->gs.world->height;
    
    glViewport(update_area.left / w_ratio, update_area.top / h_ratio, update_area.width() / w_ratio, update_area.height() / h_ratio);
    glScissor(update_area.left / w_ratio, update_area.top / h_ratio, update_area.width() / w_ratio, update_area.height() / h_ratio);

    Eng3D::TextureOptions border_tex_options{};
    border_tex_options.internal_format = Eng3D::TextureOptions::Format::RGB32F;
    border_tex_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
    border_tex_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
    border_tex_options.editable = true;
    Eng3D::Texture border_tex(width, height);
    border_tex.upload(border_tex_options);
    border_tex.gen_mipmaps();

    auto tex_coord_scale = update_area;
    tex_coord_scale.scale(1.f / glm::vec2(width, height));

    Eng3D::Log::debug("game", "Creating border framebuffer");
    Eng3D::OpenGL::Framebuffer border_fbuffer{};
    border_fbuffer.use();
    border_fbuffer.set_texture(0, border_tex);

    Eng3D::Log::debug("game", "Drawing border with border shader");
    border_gen_shader->use();
    border_gen_shader->set_uniform("map_size", width, height);
    border_gen_shader->set_uniform("tex_coord_scale", tex_coord_scale.left, tex_coord_scale.top, tex_coord_scale.right, tex_coord_scale.bottom);
    border_gen_shader->set_texture(0, "terrain_map", *terrain_map);
    border_gen_shader->set_texture(1, "tile_sheet_nation", *tile_sheet_nation);
    map_2d_quad.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    border_tex.gen_mipmaps();

    sdf_shader->use();
    sdf_shader->set_uniform("map_size", width, height);
    sdf_shader->set_uniform("tex_coord_scale", tex_coord_scale.left, tex_coord_scale.top, tex_coord_scale.right, tex_coord_scale.bottom);

    Eng3D::TextureOptions fbo_mipmap_options{};
    fbo_mipmap_options.internal_format = Eng3D::TextureOptions::Format::RGB32F;
    fbo_mipmap_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
    fbo_mipmap_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
    fbo_mipmap_options.editable = true;
    // The Red & Green color channels are the coords on the map
    // The Blue is the distance to a border
    if(border_sdf.get() == nullptr) {
        border_sdf = std::make_unique<Eng3D::Texture>(border_tex.width, border_tex.height);
        border_sdf->upload(fbo_mipmap_options);
    }

    auto swap_tex = std::make_unique<Eng3D::Texture>(width, height);
    swap_tex->upload(fbo_mipmap_options);

    Eng3D::OpenGL::Framebuffer fbo{};
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
        if(step == max_dist) sdf_shader->set_texture(0, "tex", border_tex);
        else sdf_shader->set_texture(0, "tex", draw_on_tex0 ? *swap_tex : *border_sdf);
        // Draw a plane over the entire screen to invoke shaders
        map_2d_quad.draw();
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
void MapRender::update_mapmode(std::vector<ProvinceColor>& province_colors) {
    // Water
    for(size_t i = 0; i < this->gs.world->provinces.size(); i++)
        if(this->gs.world->terrain_types[this->gs.world->provinces[i].terrain_type_id].is_water_body)
            province_colors[i] = ProvinceColor(ProvinceId(i), Eng3D::Color{});

    for(const auto province_color : province_colors)
        tile_sheet->buffer.get()[static_cast<size_t>(province_color.id)] = province_color.color.get_value();
    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    no_drop_options.internal_format = Eng3D::TextureOptions::Format::SRGB;
    this->tile_sheet->upload(no_drop_options);
}

// Updates nations
void MapRender::update_nations(std::vector<ProvinceId>& province_ids) {
    std::vector<NationId> nation_ids;
    for(const auto province_id : province_ids) {
        const auto& province = this->gs.world->provinces[province_id];
        this->tile_sheet_nation->buffer.get()[province] = static_cast<size_t>(province.controller_id);
        nation_ids.push_back(province.controller_id);
    }
    std::sort(nation_ids.begin(), nation_ids.end());
    auto last = std::unique(nation_ids.begin(), nation_ids.end());
    nation_ids.erase(last, nation_ids.end());

    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    this->tile_sheet_nation->upload(no_drop_options);

    // Update labels of the nations
    for(const auto nation_id : nation_ids)
        this->map.update_nation_label(this->gs.world->nations[nation_id]);
}

void MapRender::update_city_lights() {

}

void MapRender::request_update_visibility() {
    this->req_update_vision = true;
}

void MapRender::update_visibility() {
    // Fill out information for visability
    if(gs.curr_nation == nullptr || gs.curr_nation == &gs.world->nations[0]) {
        std::fill_n(province_opt->buffer.get(), gs.world->provinces.size(), 0x000000ff);
        return;
    }
    std::fill_n(province_opt->buffer.get(), gs.world->provinces.size(), 0x00000080);
    
    // Fill out density information for city lights
    for(const auto& province : gs.world->provinces) {
        const auto total = glm::max(province.total_pops(), 0.1f);
        const auto density = glm::clamp(total / 100'000.f, 0.f, 1.f) * 255.f;
        this->province_opt->buffer[province] |= (static_cast<uint8_t>(density) & 0xff) << 8;
    }

    for(const auto& nation : gs.world->nations) {
        const auto nation_id = nation;
        // If it's our own nation or an ally of ours we can see them
        if(nation_id == gs.curr_nation->get_id() || gs.world->get_relation(nation_id, gs.curr_nation->get_id()).is_allied()) {
            for(const auto province_id : gs.world->nations[nation_id].controlled_provinces) {
                const auto& province = gs.world->provinces[province_id];
                this->province_opt->buffer[province_id] |= 0x000000ff;
                for(const auto neighbour_id : province.neighbour_ids)
                    this->province_opt->buffer[neighbour_id] |= 0x000000ff;
            }
        }
    }
    gs.world->unit_manager.units.for_each([this](Unit& unit) {
        auto &_gs = this->gs;
        // Unit must be ours or be owned by our ally
        if(unit.owner_id != _gs.curr_nation->get_id() && !_gs.world->get_relation(unit.owner_id, _gs.curr_nation->get_id()).is_allied())
            return;
        auto prov_id = _gs.world->unit_manager.get_unit_current_province(unit.cached_id);
        this->province_opt->buffer[prov_id] |= 0x000000ff;
        for(const auto neighbour_id : _gs.world->provinces[prov_id].neighbour_ids)
            this->province_opt->buffer[neighbour_id] |= 0x000000ff;
    });
    if(gs.map->province_selected)
        this->province_opt->buffer[gs.map->selected_province_id] |= 0x400000ff;
}

void MapRender::update() {
    if(gs.world->province_manager.is_provinces_changed())
        this->req_update_vision = true;
    
    Eng3D::Rect update_area{ 0, 0, 0, 0 };

    std::vector<ProvinceId> update_provinces;
    auto& changed_owner_provinces = gs.world->province_manager.get_changed_owner_provinces();
    update_provinces.insert(update_provinces.end(), changed_owner_provinces.cbegin(), changed_owner_provinces.cend());
    
    auto& changed_control_provinces = gs.world->province_manager.get_changed_control_provinces();
    update_provinces.insert(update_provinces.end(), changed_control_provinces.cbegin(), changed_control_provinces.cend());
    
    std::sort(update_provinces.begin(), update_provinces.end());
    auto last = std::unique(update_provinces.begin(), update_provinces.end());
    update_provinces.erase(last, update_provinces.end());

    if(!update_provinces.empty()) {
        for(const auto province_id : update_provinces) {
            auto& province = gs.world->provinces[province_id];
            update_area = update_area.join(province.box_area);
            if(this->options.sdf.used)
                this->update_border_sdf(province.box_area, glm::ivec2(gs.width, gs.height));
        }
        this->update_nations(update_provinces);
    }

    if(this->req_update_vision) {
        this->update_visibility();
        this->req_update_vision = false;
    }
    this->update_city_lights();
    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    this->province_opt->upload(no_drop_options);
}

void MapRender::draw(const Eng3D::Camera& camera, MapView view_mode) {
    map_shader->use();
    const auto view = camera.get_view();
    map_shader->set_uniform("view", view);
    const auto cam_pos = camera.get_world_pos();
    map_shader->set_uniform("view_pos", cam_pos.x, cam_pos.y, cam_pos.z);
    const auto projection = camera.get_projection();
    map_shader->set_uniform("projection", projection);
    const auto map_pos = camera.get_map_pos();
    float distance_to_map = map_pos.z / this->gs.world->width;
    map_shader->set_uniform("dist_to_map", distance_to_map);
    map_shader->set_uniform("map_size", static_cast<float>(this->gs.world->width), static_cast<float>(this->gs.world->height));
    // A time uniform to send to the shader
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto millisec_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    const auto time = static_cast<float>(millisec_since_epoch % 1000000) / 1000.f;
    map_shader->set_uniform("time", time);
    map_shader->set_uniform("ticks", this->gs.world->time);
    // Map should have no "model" matrix since it's always static
    map_shader->set_texture(0, "tile_sheet", *tile_sheet);
    map_shader->set_texture(1, "tile_sheet_nation", *tile_sheet_nation);
    if(options.water.used)
        map_shader->set_texture(2, "water_texture", *water_tex);
    if(options.noise.used)
        map_shader->set_texture(3, "noise_texture", *noise_tex);
    map_shader->set_texture(4, "terrain_map", *terrain_map); // 4 col
    if(options.sdf.used)
        map_shader->set_texture(5, "border_sdf", *(border_sdf.get())); // 1 col
    if(options.rivers.used)
        map_shader->set_texture(6, "river_texture", *river_tex); // 1 col
    if(options.lighting.used) {
        map_shader->set_texture(7, "wave1", *wave1);
        map_shader->set_texture(8, "wave2", *wave2);
        map_shader->set_texture(9, "normal", *normal_topo); // 3 col
    }
    map_shader->set_texture(10, "bathymethry", *bathymethry); // 1 col
    map_shader->set_texture(11, "paper_tex", *paper_tex);
    //map_shader->set_texture(12, "stripes", *stripes_tex);
    map_shader->set_texture(13, "province_opt", *province_opt);
    map_shader->set_texture(14, "terrain_sheet", *terrain_sheet);

    if(view_mode == MapView::PLANE_VIEW) {
        for(const auto& quad : this->map_quads)
            quad->draw();
    } else if(view_mode == MapView::SPHERE_VIEW) {
        map_sphere.draw();
    }
}
