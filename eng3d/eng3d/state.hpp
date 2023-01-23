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
//      state.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <mutex>
#include <memory>
#include <map>
#include <string>
#include <atomic>
#include <chrono>
#include <functional>

#include "eng3d/ttf.hpp"
#include "eng3d/io.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/audio.hpp"
#include "eng3d/material.hpp"
#include "eng3d/model.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/string.hpp"
#include "eng3d/event.hpp"

struct SDL_Window;
namespace Eng3D {
    namespace OpenGL {
        class Shader;
    }

    class State;
    class Installer {
        Eng3D::State& s;
    public:
        Installer(Eng3D::State& s);
        ~Installer();
    };

    class State {
        glm::ivec2 mouse_pos; // Mouse position
    public:
        State(const std::vector<std::string>& pkg_paths);
        ~State();
        void init_window(void);
        void clear() const;
        void reload_shaders();
        void swap();
        void do_event();
        void set_multisamples(int samples) const;
        static State& get_instance();
        void do_run(std::function<bool(void)> cond, std::function<void(void)> event, std::function<void(void)> render);

        /// @brief Value to ignore x/y axis motion taps (useful ignoring stray joystick input)
        constexpr static auto JOYSTICK_DEAD_ZONE = 3000;

        virtual void handle_resize();
        virtual void handle_mouse_btn(const Eng3D::Event::MouseButton& e);
        virtual void handle_mouse_motion(const Eng3D::Event::MouseMotion& e);
        virtual void handle_mouse_wheel(const Eng3D::Event::MouseWheel& e);
        virtual void handle_key(const Eng3D::Event::Key& e);

        /// @brief Number of the axis assigned to map movement
        int map_movement_axis_num = 0;
        /// @brief Number of the axis assigned to cursor movement
        int cursor_movement_axis_num = 1;
        /// @brief Button for zooming in
        int zoomin_button_num = 10; // Plus
        int zoomout_button_num = 11; // Minus
        int left_button_num = 12; // Dpad left
        int up_button_num = 13; // Dpad up
        int right_button_num = 14; // Dpad right
        int down_button_num = 15; // Dpad down
        int select_button_num = 0; // A
        int back_button_num = 1; // B
        int map_select_button_num = 2; // X
        int map_back_button_num = 3; // Y

        /// @brief Variable telling if the game should quit, honored by most event loops
        /// but should be used explicitly if possible
        std::atomic<bool> run;
        bool show_ui = true;
        std::chrono::system_clock::time_point current_frame_time;
        float delta_time;

        // These variables needs to be initialized before any installers
        SDL_Window* window;
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        void* context;
        int width, height;
        size_t max_texture_size = 0;
        // Builtin shaders
        std::map<std::string, std::unique_ptr<Eng3D::OpenGL::Shader>> builtin_shaders;
#else
        // RVL uses global state variables that are platform specific
#endif
#if defined __switch__
        uint64_t application_id = 0x0000000000000000;
#endif
        // Above variables might get modified by these so keep them in order as-is
        Eng3D::StringManager string_man;
        Eng3D::Installer installer;
        Eng3D::IO::PackageManager package_man;
        Eng3D::AudioManager audio_man;
        Eng3D::TextureManager tex_man;
        Eng3D::MaterialManager material_man;
        Eng3D::ModelManager model_man;
        Eng3D::TrueType::Manager ttf_man;
        UI::Context ui_ctx;
    };
}
