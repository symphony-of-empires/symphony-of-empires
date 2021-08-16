#include "map.hpp"

#include <GL/glu.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <execution>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <mutex>

#include "path.hpp"
#include "print.hpp"
#include "render/model.hpp"

Map::Map(const World& _world) : world(_world) {
    std::lock_guard<std::recursive_mutex> lock(world.provinces_mutex);

    // print_info("Creating province meshes");
    // for (const auto& province : world.provinces) {
    //     ProvinceShape pr_shape = ProvinceShape(*this, *province);
    //     province_shapes.push_back(pr_shape);
    // }

    overlay_tex = &g_texture_manager->load_texture(Path::get("ui/map_overlay.png"));
    if (glewIsSupported("GL_VERSION_2_1")) {
        water_tex = &g_texture_manager->load_texture(Path::get("water_tex.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        noise_tex = &g_texture_manager->load_texture(Path::get("noise_tex.png"), GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
        map_quad = new UnifiedRender::OpenGl::PrimitiveSquare(0, 0, world.width, world.height);

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
    if (glewIsSupported("GL_VERSION_2_1")) {
        div_sheet_tex = new UnifiedRender::Texture(256, 256);
        for (size_t i = 0; i < 256 * 256; i++) {
            div_sheet_tex->buffer[i] = 0x00000000;
        }
        for (size_t i = 0; i < world.width * world.height; i++) {
            uint8_t r, g, b, a;
            const Tile& tile = world.get_tile(i);
            r = tile.province_id % 256;
            g = (tile.province_id / 256) % 256;
            b = tile.owner_id % 256;
            a = (tile.owner_id / 256) % 256;
            a = tile.elevation == 0 ? 255 : a;
            div_topo_tex->buffer[i] = (a << 24) | (b << 16) | (g << 8) | (r);
            if (tile.owner_id < world.nations.size()) {
                const Nation* owner = world.nations.at(tile.owner_id);
                uint32_t color = owner->color;
                div_sheet_tex->buffer[r + g * 256] = (0xff << 24) | color;
            }
        }
        div_sheet_tex->to_opengl();
    } else {
        for (size_t i = 0; i < world.width * world.height; i++) {
            uint8_t r, g, b;
            const Tile& tile = world.get_tile(i);
            if (tile.elevation <= world.sea_level) {
                r = 8;
                g = 8;
                b = 128;
            } else {
                r = 8;
                g = tile.elevation;
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

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        print_info("Frame buffer error");
}

void Map::draw_flag(const Nation* nation, float x, float y) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_LINE);
    glColor3f(1.f, 1.f, 1.f);
    glVertex3f(x, y, 0.f);
    glVertex3f(x, y, -2.f);
    glEnd();

    // Draw a flag that "waves" with some cheap wind effects it
    // looks nice and it's super cheap to make - only using sine
    const float n_steps = 8.f;  // Resolution of flag in one side (in vertices)
    const float step = 90.f;    // Steps per vertice

    auto flag = UnifiedRender::OpenGl::PackedModel<glm::vec3, glm::vec2, glm::vec3>(GL_TRIANGLE_STRIP);
    for (float r = 0.f; r <= (n_steps * step); r += step) {
        float sin_r = sin(r + wind_osc) / 24.f;

        flag.buffer.push_back(UnifiedRender::OpenGl::PackedData(
            // Vert
            glm::vec3(x + (((r / step) / n_steps) * 1.5f), y + sin_r, -2.f),
            // Texcoord
            glm::vec2((r / step) / n_steps, 0.f),
            // Colour
            glm::vec3((sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f)));

        flag.buffer.push_back(UnifiedRender::OpenGl::PackedData(
            // Vert
            glm::vec3(x + (((r / step) / n_steps) * 1.5f), y + sin_r, -1.f),
            // Texcoord
            glm::vec2((r / step) / n_steps, 0.f),
            // Colour
            glm::vec3((sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f)));
    }

    flag.vao.bind();
    flag.vbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, flag.buffer.size() * sizeof(flag.buffer[0]), &flag.buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(flag.buffer[0]), (void*)0);  // Vertices
    glEnableVertexArrayAttrib(flag.vao.get_id(), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(flag.buffer[0]), (void*)(3 * sizeof(float)));  // Texcoords
    glEnableVertexArrayAttrib(flag.vao.get_id(), 1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(flag.buffer[0]), (void*)(5 * sizeof(float)));  // Colours
    glEnableVertexArrayAttrib(flag.vao.get_id(), 2);
    nation_flags[world.get_id(nation)]->bind();
    flag.draw();

    // sin_r - Sin'ed iterator along with the wind oscillator
    /*glBegin(GL_TRIANGLE_STRIP);
    for(float r = 0.f; r <= (n_steps * step); r += step) {
        float sin_r;

        sin_r = sin(r + wind_osc) / 24.f;
        glColor3f((sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f);
        glTexCoord2f((r / step) / n_steps, 0.f);
        glVertex3f(x + (((r / step) / n_steps) * 1.5f), y + sin_r, -2.f);

        sin_r = sin(r + wind_osc + 90.f) / 24.f;
        glColor3f((sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f, (sin_r * 18.f) + 0.5f);
        glTexCoord2f((r / step) / n_steps, 1.f);
        glVertex3f(x + (((r / step) / n_steps) * 1.5f), y + sin_r, -1.f);
    }
    glEnd();*/
}

// Updates the tiles texture with the changed tiles
void Map::update(World& world) {
    std::lock_guard<std::recursive_mutex> lock(g_world->changed_tiles_coords_mutex);
    if (world.changed_tile_coords.size() > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glViewport(0, 0, div_topo_tex->width, div_topo_tex->height);

        glBegin(GL_POINTS);
        for (const auto& coords : world.changed_tile_coords) {
            uint8_t r, g, b, a;
            Tile tile = world.get_tile(coords.first, coords.second);
            r = tile.province_id % 256;
            g = (tile.province_id / 256) % 256;
            b = tile.owner_id % 256;
            a = (tile.owner_id / 256) % 256;
            a = tile.elevation == 0 ? 255 : a;
            glColor4ui(r, g, b, a);
            glVertex2i(coords.first, coords.second);
        }
        glEnd();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        world.changed_tile_coords.clear();
    }
}

void Map::draw(Camera& cam, const int width, const int height) {
    // Draw with the old method for old hardware
    if (!glewIsSupported("GL_VERSION_2_1")) {
        draw_old(cam, width, height);
        return;
    }

    glm::mat4 view, projection;

    // Map should have no "model" matrix since it's always static
    /*map_shader->use();
    view = cam.get_view();
    map_shader->set_uniform("view", view);
    projection = cam.get_projection();
    map_shader->set_uniform("projection", projection);
    map_shader->set_uniform("map_size", (float)world.width, (float)world.height);
    map_shader->set_uniform("terrain_texture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, div_topo_tex->gl_tex_num);
    map_shader->set_uniform("terrain_sheet", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, div_sheet_tex->gl_tex_num);
    map_shader->set_uniform("water_texture", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, water_tex->gl_tex_num);
    map_shader->set_uniform("noise_texture", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, noise_tex->gl_tex_num);
    map_quad->draw();*/

    // TODO: We need to better this
    obj_shader->use();
    view = cam.get_view();
    obj_shader->set_uniform("view", view);
    projection = cam.get_projection();
    obj_shader->set_uniform("projection", projection);
    obj_shader->set_uniform("map_diffusion", 0);
    world.outposts_mutex.lock();
    for (const auto& outpost : world.outposts) {
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(outpost->x, outpost->y, 0.f));
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        outpost_type_icons[world.get_id(outpost->type)]->draw();
    }
    world.outposts_mutex.unlock();

    world.units_mutex.lock();
    for (const auto& unit : world.units) {
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(unit->x, unit->y, 0.f));
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        unit_type_icons[world.get_id(unit->type)]->draw();
    }
    world.units_mutex.unlock();

    world.boats_mutex.lock();
    for (const auto& boat : world.boats) {
        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(boat->x, boat->y, 0.f));
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        obj_shader->set_uniform("model", model);
        boat_type_icons[world.get_id(boat->type)]->draw();
    }
    world.boats_mutex.unlock();

    // Resets the shader and texture
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
}

void Map::draw_old(Camera& cam, const int width, const int height) {
    wind_osc += 0.1f;
    if (wind_osc >= 180.f)
        wind_osc = 0.f;

    // Topo map texture
    {
        div_topo_tex->bind();
        auto topo_map_plane = UnifiedRender::OpenGl::PrimitiveSquare(0.f, 0.f, world.width, world.height);
        topo_map_plane.draw();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    for (size_t i = 0; i < world.provinces.size(); i++) {
        if (world.provinces[i]->owner != nullptr) {
            uint32_t& color = world.provinces[i]->owner->color;
            glColor4ub(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, 0xc0);
        } else {
            glColor4ub(0x80, 0x80, 0x80, 0xc0);
        }
        glCallList(province_shapes[i].shape_gl_list);
    }
    glCallList(coastline_gl_list);

    world.boats_mutex.lock();
    for (const auto& boat : world.boats) {
        const float size = 1.f;
        if (boat->size) {
            glBegin(GL_TRIANGLES);
            glColor3f(0.f, 1.f, 0.f);
            glVertex2f(boat->x, boat->y - 1.f);
            glVertex2f(boat->x + (boat->size / boat->type->max_health), boat->y - 1.f);
            glVertex2f(boat->x + (boat->size / boat->type->max_health), boat->y - 1.f);
            glVertex2f(boat->x + (boat->size / boat->type->max_health), boat->y - 1.25f);
            glVertex2f(boat->x, boat->y - 1.2f);
            glVertex2f(boat->x, boat->y - 1.f);
            glEnd();
        }
        boat_type_icons[world.get_id(boat->type)]->draw();
        draw_flag(boat->owner, boat->x, boat->y);
    }
    world.boats_mutex.unlock();

    world.units_mutex.lock();
    for (const auto& unit : world.units) {
        const float size = 1.f;
        if (unit->size) {
            glBegin(GL_TRIANGLES);
            glColor3f(0.f, 1.f, 0.f);
            glVertex2f(unit->x, unit->y - 1.f);
            glVertex2f(unit->x + (unit->size / unit->type->max_health), unit->y - 1.f);
            glVertex2f(unit->x + (unit->size / unit->type->max_health), unit->y - 1.f);
            glVertex2f(unit->x + (unit->size / unit->type->max_health), unit->y - 1.25f);
            glVertex2f(unit->x, unit->y - 1.2f);
            glVertex2f(unit->x, unit->y - 1.f);
            glEnd();
        }
        unit_type_icons[world.get_id(unit->type)]->draw();
        draw_flag(unit->owner, unit->x, unit->y);
    }
    world.units_mutex.unlock();

    world.outposts_mutex.lock();
    for (const auto& outpost : world.outposts) {
        const float size = 1.f;
        auto sprite_plane = UnifiedRender::OpenGl::PrimitiveSquare(outpost->x, outpost->y, outpost->x + size, outpost->y + size);
        outpost_type_icons[world.get_id(outpost->type)]->draw();
        draw_flag(outpost->owner, outpost->x, outpost->y);
    }
    world.outposts_mutex.unlock();
    glBindTexture(GL_TEXTURE_2D, 0);
}

ProvinceShape::ProvinceShape(const Map& map, const Province& base) {
    Province::Id province_id = map.world.get_id(&base);

    shape_gl_list = glGenLists(1);
    glNewList(shape_gl_list, GL_COMPILE);

    const size_t min_x = std::min(base.min_x, map.world.width - 1);
    const size_t min_y = std::min(base.min_y, map.world.height - 1);
    const size_t max_x = std::min(base.max_x, map.world.width - 1);
    const size_t max_y = std::min(base.max_y + 1, map.world.height - 1);
    for (size_t y = min_y; y < max_y; y++) {
        for (size_t x = min_x; x < max_x; x++) {
            Tile& tile = map.world.get_tile(x, y);
            if (tile.province_id != province_id)
                continue;

            std::pair<size_t, size_t> start = std::make_pair(x, y);
            size_t len = 1;
            while (x < max_x) {
                if (tile.province_id != province_id) {
                    len--;
                    x--;
                    break;
                }
                len++;
                x++;
                tile = map.world.get_tile(x, y);
            }

            glBegin(GL_QUADS);
            glVertex2f(start.first, start.second);
            glVertex2f(start.first, start.second + 1.f);
            glVertex2f(start.first + len, start.second + 1.f);
            glVertex2f(start.first + len, start.second);
            glEnd();
        }
    }
    glEndList();

    outline_gl_list = glGenLists(1);
    glNewList(outline_gl_list, GL_COMPILE);
    for (size_t y = min_y; y < max_y; y++) {
        if (y == 0) {
            continue;
        }
        for (size_t x = min_x; x < max_x; x++) {
            if (x == 0) {
                continue;
            }

            const Tile& left_tile = map.world.get_tile(x - 1, y);
            const Tile& top_tile = map.world.get_tile(x, y - 1);
            const Tile& tile = map.world.get_tile(x, y);

            if (left_tile.province_id != tile.province_id) {
                glBegin(GL_LINE_STRIP);
                glVertex2f(x, y);
                glVertex2f(x, y + 1.f);
                glEnd();
            }

            if (top_tile.province_id != tile.province_id) {
                glBegin(GL_LINE_STRIP);
                glVertex2f(x, y);
                glVertex2f(x + 1.f, y);
                glEnd();
            }
        }
    }
    glEndList();
}
