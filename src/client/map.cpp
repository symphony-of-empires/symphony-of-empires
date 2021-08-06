#include "map.hpp"

#include <GL/glu.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <execution>
#include <functional>
#include <mutex>

#include "path.hpp"
#include "print.hpp"

extern TextureManager* g_texture_manager;
Map::Map(const World& _world) : world(_world) {
    std::lock_guard<std::recursive_mutex> lock(world.provinces_mutex);

    print_info("Creating province meshes");
    for (const auto& province : world.provinces) {
        ProvinceShape pr_shape = ProvinceShape(*this, *province);
        province_shapes.push_back(pr_shape);
    }

    overlay_tex = &g_texture_manager->load_texture(Path::get("ui/map_overlay.png"));
    if (glewIsSupported("GL_VERSION_3_0")) {
        // terrain_map_tex = &g_texture_manager->load_texture(Path::get("map_terrain.png"));
        // terrain_sheet_tex = &g_texture_manager->load_texture(Path::get("terrain_sheet.png"));
        map_quad = new  Quad(0, 0, world.width, world.height);
        map_shader = new Shader("shaders/map.vs", "shaders/map.fs");
    }

    print_info("creating topo map");
    // generate the underlying topo map texture, since the topo map
    // dosen't changes too much we can just do a texture
    div_topo_tex = new Texture(world.width, world.height);
    if (glewIsSupported("gl_version_3_0")) {
        for (size_t i = 0; i < world.width * world.height; i++) {
            uint8_t r, g, b;
            const Tile& tile = world.get_tile(i);
            r = tile.province_id % 256;
            g = (tile.province_id / 256) % 256;
            b = tile.elevation;
            div_topo_tex->buffer[i] = (0xff << 24) | (b << 16) | (g << 8) | (r);
        }
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
}

extern TextureManager* g_texture_manager;
void Map::draw(Camera& cam, const int width, const int height) {
    // Draw with the old method for old hardware
    if (glewIsSupported("GL_VERSION_3_0")) {
        draw_old(cam, width, height);
        return;
    }

    glm::mat4 view_proj = cam.get_projection() * cam.get_view();

    map_shader -> use();
    map_shader->setMat4("viewProj", view_proj);
    glBindTexture(GL_TEXTURE_2D, div_topo_tex->gl_tex_num);
    map_quad -> draw();
    glUseProgram(0);
}

void Map::draw_old(Camera& cam, const int width, const int height) {
    // Topo map texture
    glBindTexture(GL_TEXTURE_2D, div_topo_tex->gl_tex_num);
    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(0.f, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(0.f + world.width, 0.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(0.f + world.width, 0.f + world.height);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(0.f, 0.f + world.height);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    for (size_t i = 0; i < world.provinces.size(); i++) {
        if (world.provinces[i]->owner != nullptr) {
            uint32_t& color = world.provinces[i]->owner->color;
            glColor4ub(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, 0xc0);
        } else {
            glColor4ub(0x80, 0x80, 0x80, 0xc0);
        }

        glCallList(province_shapes[i].shape_gl_list);

        //glColor4f(0.f, 0.f, 0.f, 0.5f);
        //glCallList(province_shapes[i].outline_gl_list);
    }
    glCallList(coastline_gl_list);

    /*glBindTexture(GL_TEXTURE_2D, overlay_tex->gl_tex_num);
    glBegin(GL_QUADS);
    glColor4f(1.f, 1.f, 1.f, 0.8f);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(0.f, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(0.f + world.width, 0.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(0.f + world.width, 0.f + world.height);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(0.f, 0.f + world.height);
    glEnd();*/
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
