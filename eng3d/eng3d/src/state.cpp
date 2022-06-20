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
//      state.cpp
//
// Abstract:
//      Defines functions for the game state, these functions initialize the
//      game to a "usable" state, which will allow it to manage resources and
//      show them.
// ----------------------------------------------------------------------------

// Required before GL/gl.h
#ifdef E3D_TARGET_WINDOWS
#   define WIN32_LEAN_AND_MEAN 1
#   ifndef NOMINMAX
#       define NOMINMAX 1
#   endif
#   include <windows.h>
#   undef WIN32_LEAN_AND_MEAN
#endif

// MSVC does not know about glext, mingw does so we just use this ifdef
#ifndef _MSC_VER
#   include <GL/glext.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <filesystem>
#include <cstring>

#include "eng3d/state.hpp"
#include "eng3d/path.hpp"
#include "eng3d/io.hpp"
#include "eng3d/audio.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/material.hpp"
#include "eng3d/model.hpp"
#include "eng3d/log.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/utils.hpp"

// Used for the singleton
static Eng3D::State* g_state = nullptr;

#ifdef E3D_BACKEND_OPENGL
// Callback function for printing debug statements
static void GLAPIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data) {
    std::string _source;
    switch(source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;
    default:
        _source = "UNKNOWN";
        break;
    }

    std::string _type;
    switch(type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;
    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;
    default:
        _type = "UNKNOWN";
        break;
    }

    std::string _severity;
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        return;
    default:
        _severity = "UNKNOWN";
        break;
    }

    std::unique_ptr<char[]> tmpbuf(new char[512]);
    snprintf(tmpbuf.get(), 512, "%d: %s of %s severity, raised from %s: %s", id, _type.c_str(), _severity.c_str(), _source.c_str(), msg);
    // Do not put double-newlines
    if(std::strchr(tmpbuf.get(), '\n') == nullptr) {
        Eng3D::Log::debug("opengl_msg", std::string() + tmpbuf.get() + "\n");
    } else {
        Eng3D::Log::debug("opengl_msg", tmpbuf.get());
    }
}
#endif

//
// Installer
//
Eng3D::Installer::Installer(Eng3D::State& _s)
    : s{ _s }
{
    // Startup-initialization of SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        CXX_THROW(std::runtime_error, std::string() + "Failed to init SDL " + SDL_GetError());
    if(TTF_Init() < 0)
        CXX_THROW(std::runtime_error, std::string() + "Failed to init TTF " + TTF_GetError());
    SDL_ShowCursor(SDL_DISABLE);
#ifdef E3D_BACKEND_OPENGL // Normal PC computer
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create the initial window
    s.width = 1024;
    s.height = 720;
    s.window = SDL_CreateWindow("Symphony of Empires", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, s.width, s.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(s.window == nullptr)
        CXX_THROW(std::runtime_error, std::string() + "Failed to init SDL window " + SDL_GetError());

    // OpenGL configurations
    s.context = SDL_GL_CreateContext(s.window);
    if(s.context == nullptr)
        CXX_THROW(std::runtime_error, std::string() + "Failed to init SDL context " + SDL_GetError());
    SDL_GL_SetSwapInterval(1);

    Eng3D::Log::debug("opengl", std::string() + "OpenGL Version: " + (const char*)glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
        CXX_THROW(std::runtime_error, "Failed to init GLEW");

    GLint size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
    Eng3D::Log::debug("gamestate", std::to_string(size));

    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, 0);

    glEnable(GL_MULTISAMPLE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.f, 1.f);

    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
#endif
}

Eng3D::Installer::~Installer()
{
#ifdef E3D_BACKEND_OPENGL
    SDL_GL_DeleteContext(s.context);
#endif
    SDL_DestroyWindow(s.window);
    SDL_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

//
// State
//
Eng3D::State::State(const std::vector<std::string>& pkg_paths)
    : installer(*this),
    package_man(*this, pkg_paths), // Initialize the IO first, as other subsystems may require access to files (i.e the UI context)
    audio_man(*this),
    tex_man(*this),
    material_man(*this),
    model_man(*this),
    ui_ctx(*this)
{
    // Make sure we're the only state running
    if(g_state != nullptr)
        CXX_THROW(std::runtime_error, "Duplicate instancing of GameState");
    g_state = this;

    this->reload_shaders();

    // Plugins system (still wip)
    /*for(const auto& plugin : Path::get_all("plugin.dll")) {
#ifdef E3D_TARGET_WINDOWS
        HINSTANCE hGetProcIDDLL = LoadLibrary(plugin.c_str());
        // This shouldn't happen - like ever!
        if(!hGetProcIDDLL) {
            Eng3D::Log::error("plugin", "DLL file " + plugin + " not found");
            continue;
        }

        typedef int(__stdcall* plugin_dll_entry_t)(const char* gameid, int gamever);
        plugin_dll_entry_t entry = (plugin_dll_entry_t)GetProcAddress(hGetProcIDDLL, "__unirend_entry");
        if(!entry) {
            Eng3D::Log::warning("plugin", "Can't find __unirend_entry on " + plugin);
            continue;
        }

        int r = entry("SYMPHONY_EMPIRES", 0x00F0);
        if(r != 0) {
            Eng3D::Log::warning("plugin", "Error RET=" + std::to_string(r) + " on plugin " + plugin);
        }
#endif
    }*/
    ui_ctx.resize(width, height);
}

Eng3D::State::~State() {
    g_state = nullptr;
}

void Eng3D::State::reload_shaders() {
    // Compile built-in shaders
    const auto read_file = [this](const std::string& file_name) {
        return this->package_man.get_unique("shaders/" + file_name)->read_all();
    };
    const auto load_fragment_shader = [read_file](std::string file_name) {
        return std::unique_ptr<Eng3D::OpenGL::FragmentShader>(new Eng3D::OpenGL::FragmentShader(read_file(file_name)));
    };
    const auto load_vertex_shader = [read_file](std::string file_name) {
        return std::unique_ptr<Eng3D::OpenGL::VertexShader>(new Eng3D::OpenGL::VertexShader(read_file(file_name)));
    };

    builtin_shaders.clear();
#ifdef E3D_BACKEND_OPENGL
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
#endif
}

void Eng3D::State::clear() const {
#ifdef E3D_BACKEND_OPENGL
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.f);
#else
    /// @todo RGX clear function
#endif
}

void Eng3D::State::swap() const {
#ifdef E3D_BACKEND_OPENGL
    // Required by macOS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    SDL_GL_SwapWindow(window);
#else

#endif
}

void Eng3D::State::set_multisamples(int samples) const {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
}

Eng3D::State& Eng3D::State::get_instance() {
    return *g_state;
}
