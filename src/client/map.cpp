#include <algorithm>
#include <functional>
#include <execution>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include "map.hpp"
#include "path.hpp"

Map::Map(const World& _world) : world(_world) {
    std::lock_guard<std::recursive_mutex> lock(world.provinces_mutex);
    for(const auto& province: world.provinces) {
        ProvinceShape pr_shape = ProvinceShape(*this, *province);
        province_shapes.push_back(pr_shape);
    }
    
    // Generate the underlying topo map texture, since the topo map
    // dosen't changes too much we can just do a texture
    topo_tex = new Texture(world.width, world.height);
    for(size_t i = 0; i < world.width* world.height; i++) {
        uint8_t r, g, b;
        const Tile& tile = world.get_tile(i);
        
        if(tile.elevation <= world.sea_level) {
            r = 16;
            g = 16;
            b = tile.elevation + 32;
        } else {
            r = 256 - (tile.elevation - world.sea_level);
            g = 256 - (tile.elevation - world.sea_level);
            b = 256 - (tile.elevation - world.sea_level);
        }
        topo_tex->buffer[i] = (0xff << 24) | (b << 16) | (g << 8) | (r);
    }
    topo_tex->to_opengl();
    
    coastline_gl_list = glGenLists(1);
    glNewList(coastline_gl_list, GL_COMPILE);
    glLineWidth(2.f);
    glColor3f(0.f, 0.f, 0.f);
    for(size_t x = 1; x < world.width; x++) {
        for(size_t y = 1; y < world.height; y++) {
            const Tile& left_tile = world.get_tile(x - 1, y);
            const Tile& top_tile = world.get_tile(x, y - 1);
            const Tile& tile = world.get_tile(x, y);
            
            if((left_tile.elevation <= world.sea_level && tile.elevation > world.sea_level)
            || (left_tile.elevation > world.sea_level && tile.elevation <= world.sea_level)) {
                glBegin(GL_LINE_STRIP);
                glVertex2f(x, y);
                glVertex2f(x, y + 1.f);
                glEnd();
            }
            
            if((top_tile.elevation <= world.sea_level && tile.elevation > world.sea_level)
            || (top_tile.elevation > world.sea_level && tile.elevation <= world.sea_level)) {
                glBegin(GL_LINE_STRIP);
                glVertex2f(x, y);
                glVertex2f(x + 1.f, y);
                glEnd();
            }
        }
    }
    glEndList();
}

extern TextureManager* g_texture_manager;
void Map::draw(float zoom) {
    // Topo map texture
    glBindTexture(GL_TEXTURE_2D, topo_tex->gl_tex_num);
    glBegin(GL_QUADS);
    glColor3f(1.f, 1.f, 1.f);
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
    
    for(size_t i = 0; i < world.provinces.size(); i++) {
        if(world.provinces[i]->owner != nullptr) {
            uint32_t& color = world.provinces[i]->owner->color;
            glColor4ub(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, 0x80);
        } else {
            uint32_t color = 0xffd0d0d0;
            glColor4ub(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, 0x80);
        }

        glCallList(province_shapes[i].shape_gl_list);
    }
    
    glCallList(coastline_gl_list);
    
    /*
    if(zoom >= -200.f) {
        glLineWidth(4.f);
        glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
        for(size_t x = 1; x < world.width; x++) {
            for(size_t y = 1; y < world.height; y++) {
                const Tile& left_tile = world.get_tile(x - 1, y);
                const Tile& top_tile = world.get_tile(x, y - 1);
                const Tile& tile = world.get_tile(x, y);
                
                if(left_tile.province_id != tile.province_id) {
                    glBegin(GL_LINE_STRIP);
                    glVertex2f(x, y);
                    glVertex2f(x, y + 1.f);
                    glEnd();
                }
                
                if(top_tile.province_id != tile.province_id) {
                    glBegin(GL_LINE_STRIP);
                    glVertex2f(x, y);
                    glVertex2f(x + 1.f, y);
                    glEnd();
                }
            }
        }
    }
    */
}

ProvinceShape::ProvinceShape(const Map& map, const Province& base) {
    ProvinceId province_id = map.world.get_id(&base);

    shape_gl_list = glGenLists(1);
    glNewList(shape_gl_list, GL_COMPILE);
    for(size_t y = base.min_y; y <= base.max_y; y++) {
        for(size_t x = base.min_x; x <= base.max_x; x++) {
            Tile& tile = map.world.get_tile(x, y);
            if(tile.province_id != province_id)
                continue;
            
            std::pair<size_t, size_t> start = std::make_pair(x, y);
            size_t len = 0;
            while(tile.province_id == province_id && x <= base.max_x) {
                len++;
                x++;
                tile = map.world.get_tile(x, y);
            }
            x--;
            
            glBegin(GL_QUADS);
            glVertex2f(start.first, start.second);
            glVertex2f(start.first, start.second + 1.f);
            glVertex2f(start.first + len, start.second + 1.f);
            glVertex2f(start.first + len, start.second);
            glEnd();
        }
    }
    glEndList();
}
