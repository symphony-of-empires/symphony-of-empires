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

    // Compile built-in shaders

    // Big library used mostly by every shader, compiled for faster linking or other stuff
    builtin_shaders["fs_lib"] = std::unique_ptr<UnifiedRender::OpenGL::FragmentShader>(new UnifiedRender::OpenGL::FragmentShader(
        "#version 330 compatibility\n"
        "precision lowp float;\n"
        "\n"
        "// Generic function for normalization of the water\n"
        "vec3 get_water_normal(float time, sampler2D wave1, sampler2D wave2, vec2 tex_coords) {\n"
        "    float offset = time * 0.01;\n"
        "    vec2 coords = tex_coords * 50.0;\n"
        "    vec3 normal1 = texture(wave1, coords + vec2(1.0) * offset).xyz;\n"
        "    normal1 = normalize(normal1 * 2.0 - 1.0);\n"
        "    vec3 normal2 = texture(wave2, coords + vec2(0.2, -0.8) * offset).xyz;\n"
        "    normal2 = normalize(normal2 * 2.0 - 1.0);\n"
        "    vec3 normal = normalize(normal1 + normal2);\n"
        "    normal.z *= -1.0;\n"
        "    return normal;\n"
        "}\n"
        "\n"
        "// https://iquilezles.org/www/articles/texturerepetition/texturerepetition.htm\n"
        "vec4 no_tiling(sampler2D tex, vec2 uv, sampler2D noisy_tex) {\n"
        "    float k = texture(noisy_tex, 0.005 * uv).x; // cheap (cache friendly) lookup\n"
        "    float v = 1.0;\n"
        "\n"
        "    vec2 duvdx = dFdx(uv);\n"
        "    vec2 duvdy = dFdx(uv);\n"
        "\n"
        "    float l = k * 8.0;\n"
        "    float f = fract(l);\n"
        "\n"
        "    float ia = floor(l); // my method\n"
        "    float ib = ia + 1.0;\n"
        "\n"
        "    vec2 offa = sin(vec2(3.0f, 7.0) * ia); // can replace with any other hash\n"
        "    vec2 offb = sin(vec2(3.0f, 7.0) * ib); // can replace with any other hash\n"
        "\n"
        "    vec4 cola = textureGrad(tex, uv + v * offa, duvdx, duvdy);\n"
        "    vec4 colb = textureGrad(tex, uv + v * offb, duvdx, duvdy);\n"
        "    vec4 diff = cola - colb;\n"
        "    return mix(cola, colb, smoothstep(0.2, 0.8, f - 0.1 * (diff.x + diff.y + diff.z)));\n"
        "}\n"
        "\n"
        "// Watercolor efffect\n"
        "float water_aquarelle(sampler2D noise_tex, vec2 tex_coords) {\n"
        "    vec2 uv = tex_coords * 20.0;\n"
        "    vec3 col = vec3(1.0);\n"
        "    \n"
        "    float strenght = 0.5;\n"
        "    float tex3 = textureLod(noise_tex, uv * 0.02, 1.5).x;\n"
        "    float layer1 = mix(strenght, 1.0, tex3);\n"
        "    \n"
        "    uv *= 2.1;\n"
        "    float tex4 = textureLod(noise_tex, -uv * 0.02 + 0.3, 1.5).x;\n"
        "    float layer2 = mix(strenght, 1.0,  tex4);\n"
        "    layer1 += layer2;\n"
        "    layer1 *= 0.69;\n"
        "    layer1 = clamp(layer1, 0.0, 1.05);\n"
        "    return layer1;\n"
        "}\n"
    ));

    // 2D generic fragment shader
    builtin_shaders["fs_2d"] = std::unique_ptr<UnifiedRender::OpenGL::FragmentShader>(new UnifiedRender::OpenGL::FragmentShader(
        "#version 330 compatibility\n"
        "precision lowp float;\n"
        "\n"
        "out vec4 f_frag_color;\n"
        "in vec2 v_texcoord;\n"
        "\n"
        "provided sampler2D tex;\n"
        "\n"
        "void main() {\n"
        "    f_frag_color = texture(tex, v_texcoord);\n"
        "}\n"
    ));

    // 2D generic vertex shader
    builtin_shaders["vs_2d"] = std::unique_ptr<UnifiedRender::OpenGL::VertexShader>(new UnifiedRender::OpenGL::VertexShader(
        "#version 330 compatibility\n"
        "precision lowp float;\n"
        "\n"
        "layout (location = 0) in vec2 m_pos;\n"
        "layout (location = 1) in vec2 m_texcoord;\n"
        "\n"
        "out vec2 v_texcoord;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = vec4(m_pos.xy, 0.0, 1.0);\n"
        "    v_texcoord = m_texcoord;\n"
        "}\n"
    ));

    // 3D generic fragment shader
    builtin_shaders["fs_3d"] = std::unique_ptr<UnifiedRender::OpenGL::FragmentShader>(new UnifiedRender::OpenGL::FragmentShader(
        "#version 330 compatibility\n"
        "precision lowp float;\n"
        "\n"
        "out vec4 f_color;\n"
        "in vec2 v_texcoord;\n"
        "\n"
        "uniform sampler2D diffuse_map;\n"
        "\n"
        "uniform vec3 ambient_color;\n"
        "uniform vec3 diffuse_color;\n"
        "\n"
        "void main() {\n"
        "    vec4 tex_color = texture(diffuse_map, v_texcoord);\n"
        "    tex_color.xyz *= ambient_color * diffuse_color;\n"
        "    f_color = tex_color;\n"
        "}\n"
    ));

    // 3D generic vertex shader
    builtin_shaders["vs_3d"] = std::unique_ptr<UnifiedRender::OpenGL::VertexShader>(new UnifiedRender::OpenGL::VertexShader(
        "#version 330 compatibility\n"
        "precision lowp float;\n"
        "\n"
        "layout (location = 0) in vec3 m_pos;\n"
        "layout (location = 1) in vec2 m_texcoord;\n"
        "\n"
        "provided mat4 view;\n"
        "provided mat4 projection;\n"
        "provided mat4 model;\n"
        "\n"
        "out vec2 v_texcoord;\n"
        "\n"
        "void main() {\n"
        "    v_texcoord = m_texcoord;\n"
        "    gl_Position = projection * view * model * vec4(m_pos, 1.0);\n"
        "}\n"
    ));

    builtin_shaders["vs_font_sdf"] = std::unique_ptr<UnifiedRender::OpenGL::VertexShader>(new UnifiedRender::OpenGL::VertexShader(
        "#version 330 compatibility\n"
        "precision lowp float;\n"
        "\n"
        "layout (location = 0) in vec3 m_pos;\n"
        "layout (location = 1) in vec2 m_texcoord;\n"
        "\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "\n"
        "out vec2 v_texcoord;\n"
        "\n"
        "void main() {\n"
        "    v_texcoord = m_texcoord;\n"
        "    gl_Position = projection * view * model * vec4(m_pos, 1.0);\n"
        "}\n"
    ));

    builtin_shaders["fs_font_sdf"] = std::unique_ptr<UnifiedRender::OpenGL::FragmentShader>(new UnifiedRender::OpenGL::FragmentShader(
        "#version 330 compatibility\n"
        "precision lowp float;\n"
        "\n"
        "out vec4 f_color;\n"
        "in vec2 v_texcoord;\n"
        "\n"
        "uniform sampler2D atlas;\n"
        "uniform float px_range;\n"
        "\n"
        "float screen_px_range() {\n"
        "    vec2 uv = v_texcoord;\n"
        "    uv.y = 1.0 - uv.y;\n"
        "    vec2 unit_range = vec2(px_range) / vec2(textureSize(atlas, 0));\n"
        "    return max(0.5 * dot(unit_range, vec2(1.0) / fwidth(uv)), 1.0);\n"
        "}\n"
        "\n"
        "float median(float r, float g, float b) {\n"
        "    return max(min(r, g), min(max(r, g), b));\n"
        "}\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texcoord;\n"
        "    uv.y = 1. - uv.y;\n"
        "    vec3 msd = texture(atlas, uv).rgb;\n"
        "    float sd = median(msd.r, msd.g, msd.b);\n"
        "    float screen_px_distance = screen_px_range() * (sd - 0.5);\n"
        "    float opacity = clamp(screen_px_distance + 0.5, 0.0, 1.0);\n"
        "    f_color = vec4(0.0, 0.0, 0.0, opacity);\n"
        "}\n"
    ));

    const std::string asset_path = Path::get_full();

    print_info("Assets path: %s", asset_path.c_str());
    for(const auto& entry : std::filesystem::directory_iterator(asset_path)) {
        if(entry.is_directory()) {
            const auto& path = entry.path().lexically_relative(asset_path);
            Path::add_path(path.string());
        }
    }

    // Plugins system (still wip)
    for(const auto& plugin : Path::get_all("plugin.dll")) {
#if defined windows
        HINSTANCE hGetProcIDDLL = LoadLibrary(plugin.c_str());
        // This shouldn't happen - like ever!
        if(!hGetProcIDDLL) {
            UnifiedRender::Log::error("plugin", "DLL file " + plugin + " not found");
            continue;
        }
        
        typedef int(__stdcall* plugin_dll_entry_t)(const char *gameid, int gamever);
        plugin_dll_entry_t entry = (plugin_dll_entry_t)GetProcAddress(hGetProcIDDLL, "__unirend_entry");
        if(!entry) {
            UnifiedRender::Log::warning("plugin", "Can't find __unirend_entry on " + plugin);
            continue;
        }

        int r = entry("SYMPHONY_EMPIRES", 0x00F0);
        if(r != 0) {
            UnifiedRender::Log::warning("plugin", "Error RET=" + std::to_string(r) + " on plugin " + plugin);
        }
#endif
    }

    ui_ctx = new UI::Context();
    ui_ctx->resize(width, height);
}

UnifiedRender::State::~State(void) {
    delete ui_ctx;

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