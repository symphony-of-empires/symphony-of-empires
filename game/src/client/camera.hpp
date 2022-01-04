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
//      client/camera.cpp
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

class Camera {
public:
    float fov = 45.0f, near_plane = 1.0f, far_plane = 20000.0f;
    glm::vec2 screen_size;
    glm::vec3 position;

    Camera(int width, int height) {
        screen_size = glm::vec2(width, height);
    }
    virtual ~Camera() {};

    void set_screen(const int width, const int height) {
        screen_size = glm::vec2(width, height);
    }

    virtual void move(float x_dir, float y_dir, float z_dir) = 0;
    virtual void set_pos(float x, float y) = 0;

    virtual void update(void) = 0;

    virtual glm::mat4 get_projection() = 0;

    virtual glm::mat4 get_view() = 0;
    virtual std::pair<float, float> get_map_pos(std::pair<int, int> mouse_pos) = 0;
};