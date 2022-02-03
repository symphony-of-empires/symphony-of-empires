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
//      client/flat_camera.hpp
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

#include "world.hpp"
#include "value_chase.hpp"

class FlatCamera: public Camera {
public:
    glm::vec3 target;
    ValueChase<glm::vec3> chase{ 0.2f };

    FlatCamera(glm::vec2 screen_size, glm::vec2 map_size)
        : Camera(screen_size, map_size)
    {
        world_position = glm::vec3(400, 200, 400.f);
        map_position = world_position;
        target = map_position;
    }

    FlatCamera(Camera* camera)
        : Camera(camera)
    {
        world_position = map_position;
        target = map_position;
    }

    void move(float x_dir, float y_dir, float z_dir) override {
        float scale = glm::abs(target.z * map_size.x / 1e5);
        target.x += x_dir * scale;
        target.y += y_dir * scale;
        target.y = glm::clamp(target.y, 0.f, map_size.y);
        target.z += z_dir * scale;
        target.z = glm::clamp(target.z, 0.f, map_size.x / 2.f);
    }

    void update(void) override {
        map_position = chase.move_toward(map_position, target);
        map_position.y = glm::clamp(map_position.y, 0.f, map_size.y);
        map_position.z = glm::clamp(map_position.z, 10.f, map_size.x / 2.f);

        world_position = map_position;
        world_position.x = glm::mod(world_position.x, map_size.x);
        world_position.z *= -1;
    };

    void set_pos(float x, float y) override {
        map_position.x = glm::mod(x, map_size.x);
        map_position.y = glm::clamp(y, 0.f, map_size.y);
        target = map_position;

        update();
    }


    glm::vec3 get_map_pos() override {
        glm::vec3 out_pos = map_position;
        out_pos.x = glm::mod(out_pos.x, map_size.x);
        return out_pos;
    }

    glm::mat4 get_view() override {
        glm::vec3 look_at = world_position;
        look_at.z = 0;
        look_at.y -= world_position.z > -200 ? 0.06f * (200 + world_position.z) : 0;
        glm::vec3 up_vector = glm::vec3(0.f, -1.f, 0.f);
        return glm::lookAt(world_position, look_at, up_vector);
    };

    bool get_cursor_map_pos(std::pair<int, int> mouse_pos, glm::ivec2& out_pos) override {
        float mouse_x = mouse_pos.first;
        float mouse_y = screen_size.y - 1.f - mouse_pos.second;

        glm::mat4 view = get_view();
        glm::mat4 projection = get_projection();

        glm::vec3 world_space_near = glm::unProject(
            glm::vec3(mouse_x, mouse_y, 0.f),
            view, projection,
            glm::vec4(0.f, 0.f, screen_size));

        glm::vec3 world_space_far = glm::unProject(
            glm::vec3(mouse_x, mouse_y, 1.f),
            view, projection,
            glm::vec4(0.f, 0.f, screen_size)
        );

        glm::vec3 ray_direction = world_space_far - world_space_near;

        float distance = 0.f;
        bool hit = glm::intersectRayPlane(world_space_near, ray_direction, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), distance);

        glm::vec3 intersection_point = world_space_near + ray_direction * distance;
        out_pos.x = intersection_point.x;
        out_pos.x = glm::mod((float)out_pos.x, map_size.x);
        out_pos.y = intersection_point.y;
        if(intersection_point.y < 0 || intersection_point.y > map_size.y) {
            hit = false;
        }
        return hit;
    };

    glm::vec3 get_tile_world_pos(glm::vec2 tile_pos) override {
        return glm::vec3(tile_pos.x, tile_pos.y, 0.f);
    }
};