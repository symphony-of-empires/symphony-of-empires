// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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
//      state.cpp
//
// Abstract:
//      Defines functions for the game state, these functions initialize the
//      game to a "usable" state, which will allow it to manage resources and
//      show them.
// ----------------------------------------------------------------------------

#include "unified_render/state.hpp"

#ifdef _MSC_VER
// Required before GL/gl.h
#   ifndef _WINDOWS_
#   define WIN32_LEAN_AND_MEAN 1
#       ifndef NOMINMAX
#	        define NOMINMAX 1
#       endif
#       include <windows.h>
#       undef WIN32_LEAN_AND_MEAN
#   endif
#endif
// MSVC does not know about glext, mingw does so we just use this ifdef
#ifndef _MSC_VER
#   include <GL/glext.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <filesystem>
#include <cstring>

#include "unified_render/path.hpp"
#include "unified_render/io.hpp"

#include "unified_render/print.hpp"
#include "unified_render/audio.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/material.hpp"
#include "unified_render/model.hpp"
#include "unified_render/log.hpp"
#include "unified_render/shader.hpp"

// Used for the singleton
static UnifiedRender::State* g_state = nullptr;

UnifiedRender::State::State(void) {
    if(g_state != nullptr) {
        throw std::runtime_error("Duplicate instancing of GameState");
    }
    g_state = this;

    // Startup-initialization of subsystems
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_ShowCursor(SDL_DISABLE);

    // Create the initial window
    width = 1280;
    height = 800;
    window = SDL_CreateWindow("Symphony of Empires", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    // OpenGL configurations
    context = SDL_GL_CreateContext(window);
    //SDL_GL_SetSwapInterval(1);

    print_info("OpenGL Version: %s", glGetString(GL_VERSION));

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        throw std::runtime_error("Failed to init GLEW");
    }

    GLint size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
    print_info("%d", size);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, 0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.f, 1.f);

    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

    // Initialize sound subsystem (at 11,050 hz)
    SDL_AudioSpec fmt;
    fmt.freq = 11050;
    fmt.format = AUDIO_S16;
    fmt.channels = 1;
    fmt.samples = 512;
    fmt.callback = &UnifiedRender::State::mixaudio;
    fmt.userdata = this;
    if(SDL_OpenAudio(&fmt, NULL) < 0) {
        throw std::runtime_error("Unable to open audio: " + std::string(SDL_GetError()));
    }
    SDL_PauseAudio(0);

    tex_man = new UnifiedRender::TextureManager();
    sound_man = new UnifiedRender::AudioManager();
    material_man = new UnifiedRender::MaterialManager();
    model_man = new UnifiedRender::ModelManager();
    package_man = new UnifiedRender::IO::PackageManager();

    const std::string asset_path = Path::get_full();

    print_info("Assets path: %s", asset_path.c_str());
    for(const auto& entry : std::filesystem::directory_iterator(asset_path)) {
        if(entry.is_directory()) {
            const auto& path = entry.path().lexically_relative(asset_path);
            Path::add_path(path.string());
        }
    }

    // Compile built-in shaders
    const auto read_file = [](std::string file_name) {
        return Path::cat_strings(Path::get_data("shaders/" + file_name));
    };
    const auto load_fragment_shader = [read_file](std::string file_name) {
        return std::unique_ptr<UnifiedRender::OpenGL::FragmentShader>(new UnifiedRender::OpenGL::FragmentShader(read_file(file_name)));
    };
    const auto load_vertex_shader = [read_file](std::string file_name) {
        return std::unique_ptr<UnifiedRender::OpenGL::VertexShader>(new UnifiedRender::OpenGL::VertexShader(read_file(file_name)));
    };

    // Big library used mostly by every shader, compiled for faster linking or other stuff
    builtin_shaders["fs_lib"] = load_fragment_shader("fs_lib.fs");

    // 2D generic fragment shader
    builtin_shaders["fs_2d"] = load_fragment_shader("fs_2d.fs");

    // 2D generic vertex shader
    builtin_shaders["vs_2d"] = load_vertex_shader("vs_2d.vs");

    // 3D generic fragment shader
    builtin_shaders["fs_3d"] = load_fragment_shader("fs_3d.fs");

    // 3D generic vertex shader
    builtin_shaders["vs_3d"] = load_vertex_shader("vs_3d.vs");

    builtin_shaders["vs_font_sdf"] = load_vertex_shader("vs_font_sdf.vs");

    builtin_shaders["fs_font_sdf"] = load_fragment_shader("fs_font_sdf.fs");

    // Plugins system (still wip)
    for(const auto& plugin : Path::get_all("plugin.dll")) {
#ifdef _MSC_VER
#   if defined windows
        HINSTANCE hGetProcIDDLL = LoadLibrary(plugin.c_str());
        // This shouldn't happen - like ever!
        if(!hGetProcIDDLL) {
            UnifiedRender::Log::error("plugin", "DLL file " + plugin + " not found");
            continue;
        }

        typedef int(__stdcall* plugin_dll_entry_t)(const char* gameid, int gamever);
        plugin_dll_entry_t entry = (plugin_dll_entry_t)GetProcAddress(hGetProcIDDLL, "__unirend_entry");
        if(!entry) {
            UnifiedRender::Log::warning("plugin", "Can't find __unirend_entry on " + plugin);
            continue;
        }

        int r = entry("SYMPHONY_EMPIRES", 0x00F0);
        if(r != 0) {
            UnifiedRender::Log::warning("plugin", "Error RET=" + std::to_string(r) + " on plugin " + plugin);
        }
#   endif
#endif
    }

    ui_ctx = new UI::Context();
    ui_ctx->resize(width, height);
}

UnifiedRender::State::~State(void) {
    delete tex_man;
    delete sound_man;
    delete material_man;
    delete model_man;
    delete package_man;

    delete ui_ctx;

    SDL_GL_DeleteContext(context);
    SDL_CloseAudio();

    TTF_Quit();
    SDL_Quit();

    g_state = nullptr;
}

void UnifiedRender::State::clear(void) const {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.f);
}

void UnifiedRender::State::swap(void) const {
    // Required by macOS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    SDL_GL_SwapWindow(window);
}

void UnifiedRender::State::mixaudio(void* userdata, uint8_t* stream, int len) {
    UnifiedRender::State& gs = *(static_cast<UnifiedRender::State*>(userdata));
    std::memset(stream, 0, len);

    if(gs.sound_lock.try_lock()) {
        for(unsigned int i = 0; i < gs.sound_queue.size(); ) {
            int size = gs.sound_queue.size();
            UnifiedRender::Audio* sound = gs.sound_queue[i];

            int amount = sound->len - sound->pos;
            if(amount > len) {
                amount = len;
            }

            if(amount <= 0) {
                delete sound;
                gs.sound_queue.erase(gs.sound_queue.begin() + i);
                continue;
            }

            const float volume = (SDL_MIX_MAXVOLUME / 100.f) * gs.sound_volume;
            SDL_MixAudio(stream, &sound->data[sound->pos], amount, volume);
            sound->pos += amount;
            i++;
        }

        for(unsigned int i = 0; i < gs.music_queue.size(); ) {
            UnifiedRender::Audio* music = gs.music_queue[i];

            int amount = music->len - music->pos;
            if(amount > len) {
                amount = len;
            }

            if(amount <= 0) {
                delete music;
                gs.music_queue.erase(gs.music_queue.begin() + i);
                continue;
            }

            const float volume = (SDL_MIX_MAXVOLUME / 100.f) * gs.music_volume;
            const float fade = (SDL_MIX_MAXVOLUME / 100.f) * gs.music_fade_value;
            SDL_MixAudio(stream, &music->data[music->pos], amount, volume - fade);
            music->pos += amount;
            i++;
        }
        gs.sound_lock.unlock();
    }

    if(gs.music_fade_value > 1.f) {
        gs.music_fade_value -= 1.f;
    }
}

UnifiedRender::State& UnifiedRender::State::get_instance(void) {
    return *g_state;
}