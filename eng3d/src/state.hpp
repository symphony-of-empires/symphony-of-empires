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

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_audio.h>
#include <SDL_joystick.h>
#include <SDL_ttf.h>

#include "eng3d/io.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/audio.hpp"
#include "eng3d/material.hpp"
#include "eng3d/model.hpp"
#include "eng3d/texture.hpp"

namespace Eng3D {
    class TextureManager;
    class MaterialManager;
    class ModelManager;
    class AudioManager;
    class Audio;
    namespace IO {
        class PackageManager;
    };
    namespace OpenGL {
        class Shader;
    };

    class State;
    class Installer {
        Eng3D::State& s;
    public:
        Installer(Eng3D::State& s);
        ~Installer();
    };

    class State {
    public:
        State(const std::vector<std::string>& pkg_paths);
        ~State();
        void init_window(void);
        void clear() const;
        void reload_shaders();
        void swap();
        void set_multisamples(int samples) const;
        static void mixaudio(void* userdata, uint8_t* stream, int len);
        static State& get_instance();

        SDL_Joystick* joy = nullptr;
        float joy_sensivity = 2.f;

        // These variables needs to be initialized before any installers
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        SDL_Window* window;
        SDL_GLContext context;
        int width, height;
        // Builtin shaders
        std::map<std::string, std::unique_ptr<Eng3D::OpenGL::Shader>> builtin_shaders;
#else
        // RVL uses global state variables that are platform specific
#endif

        // Above variables might get modified by these so keep them in order as-is
        Eng3D::Installer installer;
        Eng3D::IO::PackageManager package_man;
        Eng3D::AudioManager audio_man;
        Eng3D::TextureManager tex_man;
        Eng3D::MaterialManager material_man;
        Eng3D::ModelManager model_man;
        UI::Context ui_ctx;
    };
}
