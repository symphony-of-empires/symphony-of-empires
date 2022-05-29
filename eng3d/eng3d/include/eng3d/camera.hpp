// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      camera.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

namespace Eng3D {
    class Camera {
    protected:
        glm::vec3 map_position;
        glm::vec3 world_position;
        glm::vec2 screen_size;
        glm::vec2 map_size;
    public:
        float fov = 45.0f, near_plane = 1.0f, far_plane = 20000.0f;

        Camera(glm::vec2 _screen_size, glm::vec2 _map_size) {
            screen_size = _screen_size;
            map_size = _map_size;
            map_position = glm::vec3(1.0f);
        }

        Camera(const Camera& camera) {
            screen_size = camera.screen_size;
            map_size = camera.map_size;
            map_position = camera.get_map_pos();
        }

        virtual ~Camera() {};

        inline void set_screen(const int width, const int height) {
            screen_size = glm::vec2(width, height);
        }

        virtual void move(float x_dir, float y_dir, float z_dir) = 0;
        virtual void set_pos(float x, float y) = 0;
        virtual glm::vec3 get_map_pos() const = 0;

        inline glm::vec3 get_world_pos() const {
            return world_position;
        }
        
        inline glm::vec2 get_map_size() const {
            return map_size;
        }

        virtual void update(void) = 0;

        virtual glm::mat4 get_projection() const {
            const float aspect_ratio = screen_size.x / screen_size.y;
            return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
        };

        virtual glm::mat4 get_view() const = 0;
        virtual bool get_cursor_map_pos(std::pair<int, int> mouse_pos, glm::ivec2& out_pos) const = 0;
        virtual glm::vec3 get_tile_world_pos(glm::vec2 tile_pos) const = 0;
        virtual glm::vec2 get_tile_screen_pos(glm::vec2 tile_pos) const {
            auto world_pos = get_tile_world_pos(tile_pos);
            auto v = get_projection() * get_view() * glm::vec4(world_pos, 1);
            auto screen_pos = glm::vec2(v) / v.w;
            screen_pos.y *= -1;
            screen_pos = (0.5f * (screen_pos + 1.f)) * screen_size;
            return screen_pos;
        };
    };
};