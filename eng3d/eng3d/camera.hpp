// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
        glm::vec3 map_position; // Z-axis is the zoom
        glm::vec3 world_position;
        glm::vec2 screen_size;
        glm::vec2 map_size;
    public:
        float fov = 45.0f, near_plane = 1.0f, far_plane = 20000.0f;

        constexpr Camera(glm::vec2 _screen_size, glm::vec2 _map_size)
            : map_position{ glm::vec3(1.f) },
            world_position{ glm::vec3(1.f) },
            screen_size{ _screen_size },
            map_size{ _map_size }
        {

        }

        Camera(const Camera& camera)
            : map_position{ camera.get_map_pos() },
            world_position{ glm::vec3(1.f) },
            screen_size{ camera.screen_size },
            map_size{ camera.map_size }
        {

        }

        virtual ~Camera() = default;

        /// @brief Set the width and height of the screen.
        inline void set_screen(const int width, const int height) {
            screen_size = glm::vec2(width, height);
        }

        /// @brief Move the camera in the specified direction. Uses the map coordinate system, where the Z-axis is the map zoom.
        virtual void move(float x_dir, float y_dir, float z_dir) = 0;

        /// @brief Set the map position of the camera.
        virtual void set_pos(float x, float y) = 0;

        /// @brief Get the map position of the camera.
        virtual glm::vec3 get_map_pos() const = 0;

        /// @brief Get the world positions of the camera.
        inline glm::vec3 get_world_pos() const {
            return world_position;
        }

        /// @brief Get the size of the map.
        inline glm::vec2 get_map_size() const {
            return map_size;
        }

        /// @brief Update the movement of the camera. Used for smooth camera movement.
        virtual void update() = 0;

        /// @brief Get the projection matrix.
        virtual glm::mat4 get_projection() const {
            const float aspect_ratio = screen_size.x / screen_size.y;
            return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
        }

        /// @brief Get the view matrix.
        virtual glm::mat4 get_view() const = 0;

        /// @brief Get the cursors position on the map.
        /// @param mouse_pos the cursor position on the screen
        /// @param out_pos where the cursor position on the map is written to
        /// @returns true if the the cursor is inside the map otherwise false
        virtual bool get_cursor_map_pos(glm::ivec2 mouse_pos, glm::ivec2& out_pos) const = 0;

        /// @brief Get the tiles world position.
        virtual glm::vec3 get_tile_world_pos(glm::vec2 tile_pos) const = 0;

        /// @brief Get the tiles position on the screen
        virtual glm::vec2 get_tile_screen_pos(glm::vec2 tile_pos) const {
            auto world_pos = get_tile_world_pos(tile_pos);
            auto v = get_projection() * get_view() * glm::vec4(world_pos, 1);
            auto screen_pos = glm::vec2(v) / v.w;
            screen_pos.y *= -1;
            screen_pos = (0.5f * (screen_pos + 1.f)) * screen_size;
            return screen_pos;
        }
    };
}
