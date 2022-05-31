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

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#elif defined E3D_BACKEND_RGX
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_joystick.h>

#include "eng3d/io.hpp"
#include "eng3d/ui/ui.hpp"

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

    class State {
    public:
        State(const std::vector<std::string>& pkg_paths);
        ~State();
        void clear() const;
        void swap() const;
        void set_multisamples(int samples) const;
        static void mixaudio(void* userdata, uint8_t* stream, int len);
        static State& get_instance();

        SDL_Joystick* joy = nullptr;
        float joy_sensivity = 2.f;

        // Queue of sounds/music
        std::mutex sound_lock;
        std::vector<Eng3D::Audio*> sound_queue;
        std::vector<Eng3D::Audio*> music_queue;
        float music_fade_value = 1.f;

        Eng3D::AudioManager* sound_man;
        Eng3D::TextureManager* tex_man;
        Eng3D::MaterialManager* material_man;
        Eng3D::ModelManager* model_man;
        Eng3D::IO::PackageManager* package_man;
        UI::Context* ui_ctx;

        float music_volume = 50.f, sound_volume = 50.f;
#ifdef E3D_BACKEND_RGX
        // RVL uses global state variables that are platform specific
#else
        SDL_Window* window;
        SDL_GLContext context;
        int width, height;
#endif
#ifdef E3D_BACKEND_OPENGL
        // Builtin shaders
        std::map<std::string, std::unique_ptr<Eng3D::OpenGL::Shader>> builtin_shaders;
#endif
    };
}
