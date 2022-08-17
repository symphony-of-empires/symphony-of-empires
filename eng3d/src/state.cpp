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
#include <cstring>
#ifdef E3D_TARGET_WINDOWS
#   ifndef WINSOCK2_IMPORTED
#       define WINSOCK2_IMPORTED
#       include <winsock2.h>
#   endif
#   include <windows.h>
#endif
#ifdef E3D_TARGET_UNIX
#   include <unistd.h>
#endif
#ifdef E3D_TARGET_SWITCH
#   include <switch.h>
#endif

#ifdef E3D_BACKEND_OPENGL
// MSVC does not know about glext, mingw does so we just use this ifdef
#   include <GL/glew.h>
#   ifndef _MSC_VER
#       include <GL/glext.h>
#   endif
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#endif
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_audio.h>
#include <SDL_joystick.h>

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

#if defined E3D_BACKEND_OPENGL
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
    // Make sure we're the only state being installed
    if(g_state != nullptr)
        CXX_THROW(std::runtime_error, "Duplicate instancing of GameState");
    g_state = &s;

#ifdef E3D_TARGET_SWITCH
    ::consoleDebugInit(debugDevice_SVC);
    Eng3D::Log::debug("engine", "Hello world!");
    // Make sure to initialize RomFS properly
    ::romfsInit();
    ::chdir("romfs:/");
#endif

    const int seed = (int)((uint32_t)time(NULL) * (uint32_t)getpid());
    Eng3D::Log::debug("engine", "Using random seed of " + std::to_string(seed));
    std::srand(seed);

    // Startup-initialization of SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        CXX_THROW(std::runtime_error, std::string() + "Failed to init SDL " + SDL_GetError());
    SDL_ShowCursor(SDL_DISABLE);
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES // Normal PC computer
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create the initial window
#ifdef E3D_TARGET_SWITCH
    s.width = 1280;
    s.height = 720;
#else
    s.width = 1024;
    s.height = 720;
#endif
    Eng3D::Log::debug("sdl2", "New window " + std::to_string(s.width) + "x" + std::to_string(s.height));
#ifdef E3D_TARGET_SWITCH
    s.window = SDL_CreateWindow("Symphony of Empires", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, s.width, s.height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);
#else
    s.window = SDL_CreateWindow("Symphony of Empires", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, s.width, s.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#endif
    if(s.window == nullptr)
        CXX_THROW(std::runtime_error, std::string() + "Failed to init SDL window " + SDL_GetError());

    // OpenGL configurations
    s.context = reinterpret_cast<void*>(SDL_GL_CreateContext(s.window));
    if(s.context == nullptr)
        CXX_THROW(std::runtime_error, std::string() + "Failed to init SDL context " + SDL_GetError());
    SDL_GL_SetSwapInterval(1);

    Eng3D::Log::debug("opengl", std::string() + "OpenGL Version: " + (const char*)glGetString(GL_VERSION));
#   ifdef E3D_BACKEND_OPENGL
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
        CXX_THROW(std::runtime_error, "Failed to init GLEW");
#   endif

    GLint size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
    Eng3D::Log::debug("gamestate", std::to_string(size));

#   ifdef E3D_BACKEND_OPENGL
    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST);
#       ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, 0);
#       endif
#   endif

#   ifdef E3D_BACKEND_OPENGL
    glEnable(GL_MULTISAMPLE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);
#   endif

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
#   ifdef E3D_BACKEND_OPENGL
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#   endif

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
#   ifdef E3D_BACKEND_OPENGL
    glDepthRange(0.f, 1.f);
#   else
    glDepthRangef(0.f, 1.f);
#   endif
    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
#endif
}

Eng3D::Installer::~Installer()
{
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    SDL_GL_DeleteContext(s.context);
#endif
    SDL_DestroyWindow(s.window);
    SDL_Quit();
#ifdef E3D_TARGET_SWITCH
    // Make sure to gracefully unmount
    ::romfsExit();
#endif
    g_state = nullptr;
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
    ttf_man(*this),
    ui_ctx(*this)
{
    // Plugins system (still wip)
#if 0
    for(const auto& plugin : Path::get_all("plugin.dll")) {
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
    }
#endif
    ui_ctx.resize(width, height);

    // Initialize joysticks if any
    SDL_JoystickOpen(0);
    SDL_JoystickOpen(1);
}

Eng3D::State::~State() {
    
}

void Eng3D::State::reload_shaders() {
    // Compile built-in shaders
    const auto read_file = [this](const std::string& file_name) {
        return this->package_man.get_unique("shaders/" + file_name)->read_all();
    };
    const auto load_fragment_shader = [read_file](std::string file_name) {
        return std::make_unique<Eng3D::OpenGL::FragmentShader>(read_file(file_name));
    };
    const auto load_vertex_shader = [read_file](std::string file_name) {
        return std::make_unique<Eng3D::OpenGL::VertexShader>(read_file(file_name));
    };

    builtin_shaders.clear();
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    // Big library used mostly by every shader, compiled for faster linking or other stuff
    builtin_shaders["fs_lib"] = load_fragment_shader("lib.fs");
    // 2D generic fragment shader
    builtin_shaders["fs_2d"] = load_fragment_shader("2d.fs");
    // 2D generic vertex shader
    builtin_shaders["vs_2d"] = load_vertex_shader("2d.vs");
    // 3D generic fragment shader
    builtin_shaders["fs_3d"] = load_fragment_shader("3d.fs");
    // 3D generic vertex shader
    builtin_shaders["vs_3d"] = load_vertex_shader("3d.vs");
    // 3D tree fragment shader
    builtin_shaders["fs_tree"] = load_fragment_shader("tree.fs");
    // 3D tree vertex shader
    builtin_shaders["vs_tree"] = load_vertex_shader("tree.vs");
    builtin_shaders["vs_font_sdf"] = load_vertex_shader("font_sdf.vs");
    builtin_shaders["fs_font_sdf"] = load_fragment_shader("font_sdf.fs");
    // 2D Piechart shaders
    builtin_shaders["vs_piechart"] = load_vertex_shader("piechart.vs");
    builtin_shaders["fs_piechart"] = load_fragment_shader("piechart.fs");
#endif
}

void Eng3D::State::clear() const {
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#   ifdef E3D_BACKEND_OPENGL
    glClearDepth(1.f);
#   endif
#else
    /// @todo RGX clear function
#endif
}

void Eng3D::State::swap() {
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    // Required by macOS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    SDL_GL_SwapWindow(window);
#else

#endif
    tex_man.upload();
}

void Eng3D::State::do_event() {
    // Check window size every update needed cause the window sometimes changes size
    // without calling the change window size event
    SDL_GetWindowSize(this->window, &this->width, &this->height);
    this->ui_ctx.resize(this->width, this->height);
    if(this->resize_fn) this->resize_fn();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            Eng3D::Event::MouseButton e{};
            e.type = e.from_sdl(event.button.button);
            e.hold = true;
            if(mouse_btn_fn) mouse_btn_fn(e);
        } break;
        case SDL_MOUSEBUTTONUP: {
            Eng3D::Event::MouseButton e{};
            e.type = e.from_sdl(event.button.button);
            e.hold = false;
            if(mouse_btn_fn) mouse_btn_fn(e);
        } break;
        case SDL_MOUSEMOTION: {
            Eng3D::Event::MouseMotion e{};
            e.pos = Eng3D::Event::get_mouse_pos();
            if(mouse_motion_fn) mouse_motion_fn(e);
        } break;
        case SDL_MOUSEWHEEL: {
            Eng3D::Event::MouseWheel e{};
            e.wheel.x = event.wheel.x;
            e.wheel.y = event.wheel.y;
            if(mouse_wheel_fn) mouse_wheel_fn(e);
        } break;
        case SDL_KEYDOWN: {
            Eng3D::Event::Key e{};
            e.type = e.from_sdl(event.key.keysym.sym);
            e.hold = true;
            if(key_fn) key_fn(e);
        } break;
        case SDL_KEYUP: {
            Eng3D::Event::Key e{};
            e.type = e.from_sdl(event.key.keysym.sym);
            e.hold = false;
            if(key_fn) key_fn(e);
        } break;
        case SDL_QUIT:
            this->run = false;
            break;
        default: break;
        }
    }
}

void Eng3D::State::set_multisamples(int samples) const {
#ifdef E3D_BACKEND_OPENGL
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
#endif
}

Eng3D::State& Eng3D::State::get_instance() {
    return *g_state;
}
