// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      state.cpp
//
// Abstract:
//      Defines functions for the game state, these functions initialize the
//      game to a "usable" state, which will allow it to manage resources and
//      show them.
// ----------------------------------------------------------------------------

// Required before GL/gl.h
#include <cstring>
#include <signal.h>
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

#if defined _WIN32
// Need to define setenv
int setenv(const char* name, const char* value, int overwrite)
{
    int errcode = 0;
    if(!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if(errcode || envsize) return errcode;
    }
    return _putenv_s(name, value);
}
#endif
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
        Eng3D::Log::debug("opengl_msg", std::string(tmpbuf.get()) + "\n");
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

#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    // Override (if not set already) MESA variables so it can work on old systems
    setenv("MESA_GL_VERSION_OVERRIDE", "4.6", 0);
    setenv("MESA_GLSL_VERSION_OVERRIDE", "440", 0);
#endif

    // Handle SIGPIPE for networking code
#ifndef _WIN32
    struct sigaction sa = (struct sigaction){ [](int) {
        Eng3D::Log::debug("sigpipe", translate("Caught a pipe signal"));
    } };
    sigaction(SIGPIPE, &sa, NULL);
#endif
    std::string canonical_name = translate("Symphony of Empires");

    // Startup-initialization of SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        CXX_THROW(std::runtime_error, Eng3D::translate_format("Failed to initialize SDL %s", SDL_GetError()));
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
    s.window = SDL_CreateWindow(canonical_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, s.width, s.height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);
#else
    s.window = SDL_CreateWindow(canonical_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, s.width, s.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#endif
    if(s.window == nullptr)
        CXX_THROW(std::runtime_error, Eng3D::translate_format("Failed to initialize SDL window %s", SDL_GetError()));

    // OpenGL configurations
    s.context = static_cast<void*>(SDL_GL_CreateContext(s.window));
    if(s.context == nullptr)
        CXX_THROW(std::runtime_error, Eng3D::translate_format("Failed to initialize SDL context %s", SDL_GetError()));
    SDL_GL_SetSwapInterval(1);

    Eng3D::Log::debug("opengl", Eng3D::translate_format("OpenGL Version: %s", (const char*)glGetString(GL_VERSION)));
#   ifdef E3D_BACKEND_OPENGL
    glewExperimental = GL_TRUE;
    int r = glewInit();
    if(r != GLEW_OK)
        CXX_THROW(std::runtime_error, Eng3D::translate_format("Failed to ininitializeit GLEW %s", glewGetErrorString(r)));
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
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
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
    : string_man(*this),
    installer(*this),
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
    this->run = false;
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

#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/window.hpp"
#include "eng3d/ui/text.hpp"
void Eng3D::State::do_event() {
    // Check window size every update needed cause the window sometimes changes size
    // without calling the change window size event
    SDL_GetWindowSize(this->window, &this->width, &this->height);
    this->ui_ctx.resize(this->width, this->height);
    this->handle_resize();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            Eng3D::Event::MouseButton e{};
            e.type = e.from_sdl(event.button.button);
            e.hold = true;
            handle_mouse_btn(e);
        } break;
        case SDL_MOUSEBUTTONUP: {
            Eng3D::Event::MouseButton e{};
            e.type = e.from_sdl(event.button.button);
            e.hold = false;
            handle_mouse_btn(e);
        } break;
        case SDL_MOUSEMOTION: {
            Eng3D::Event::MouseMotion e{};
            e.pos = Eng3D::Event::get_mouse_pos();
            handle_mouse_motion(e);
        } break;
        case SDL_MOUSEWHEEL: {
            Eng3D::Event::MouseWheel e{};
            e.wheel.x = event.wheel.x;
            e.wheel.y = event.wheel.y;
            handle_mouse_wheel(e);
        } break;
        case SDL_KEYDOWN: {
            Eng3D::Event::Key e{};
            e.type = e.from_sdl(event.key.keysym.sym);
            e.hold = true;
            handle_key(e);
        } break;
        case SDL_KEYUP: {
            Eng3D::Event::Key e{};
            e.type = e.from_sdl(event.key.keysym.sym);
            e.hold = false;
            handle_key(e);
        } break;
        case SDL_TEXTINPUT: {
            this->ui_ctx.check_text_input(event.text.text);
        } break;
        case SDL_QUIT:
            this->run = false;
            break;
        default: break;
        }
    }

    const std::scoped_lock lock(this->ui_ctx.prompt_queue_mutex);
    for(const auto& prompt : this->ui_ctx.prompt_queue) {
        auto* win = new UI::Window(-256, 0, 512, 512);
        win->origin = UI::Origin::CENTER_SCREEN;
        win->text(prompt.first);
        win->is_scroll = true;
        win->set_close_btn_function([win](UI::Widget&) {
            win->kill();
        });
        auto* txt = new UI::Text(0, 0, win->width, win->height, win);
        txt->text_color = Eng3D::Color::rgba32(0x00000000);
        txt->text(prompt.second);
        txt->is_scroll = true;
        win->height = txt->y + txt->height;
        win->y -= win->height / 2;
    }
    this->ui_ctx.prompt_queue.clear();
    this->ui_ctx.clear_dead();
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

/// @brief Perform the main game loop
/// @param CondFn Condition that is evaluated and checked, if it is not true then stop execution
/// @param EventFn Function to handle events
/// @param RenderFn Function to handle the rendering of the game
void Eng3D::State::do_run(std::function<bool(void)> cond, std::function<void(void)> event, std::function<void(void)> render) {
    this->current_frame_time = std::chrono::system_clock::now();
    while(cond()) {
        auto prev_num = std::chrono::duration<double>(this->current_frame_time.time_since_epoch()).count();
        auto now_num = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
        this->current_frame_time = std::chrono::system_clock::now();
        this->delta_time = now_num - prev_num;
        event();
        this->clear();
        render();
        if(this->show_ui) this->ui_ctx.render_all(this->mouse_pos);
        this->swap();
    }
}

void Eng3D::State::handle_resize() {

}

void Eng3D::State::handle_mouse_btn(const Eng3D::Event::MouseButton& e) {
    if(!this->show_ui) return;
    
    if(e.hold) {
        if(this->ui_ctx.check_hover(this->mouse_pos) && e.type == Eng3D::Event::MouseButton::Type::LEFT) {
            this->mouse_pos = Eng3D::Event::get_mouse_pos();
            this->ui_ctx.check_drag(this->mouse_pos);
        }
    } else {
        this->mouse_pos = Eng3D::Event::get_mouse_pos();
        if(e.type == Eng3D::Event::MouseButton::Type::LEFT || e.type == Eng3D::Event::MouseButton::Type::RIGHT) {
            if(this->ui_ctx.check_click(this->mouse_pos)) {
                const std::scoped_lock lock(this->audio_man.sound_lock);
                auto entries = package_man.get_multiple_prefix("sfx/click");
                if(!entries.empty()) {
                    auto audio = this->audio_man.load(entries[rand() % entries.size()]->get_abs_path());
                    this->audio_man.sound_queue.push_back(audio);
                }
                return;
            }
        }
    }
}

void Eng3D::State::handle_mouse_motion(const Eng3D::Event::MouseMotion& e) {
    this->mouse_pos = e.pos;
    if(this->show_ui && this->ui_ctx.check_hover(this->mouse_pos))
        return;
}

void Eng3D::State::handle_mouse_wheel(const Eng3D::Event::MouseWheel& e) {
    if(!this->show_ui) return;
    this->mouse_pos = Eng3D::Event::get_mouse_pos();
    this->ui_ctx.check_hover(this->mouse_pos);
    if(this->ui_ctx.check_wheel(this->mouse_pos, e.wheel.y * 6))
        return;
}

void Eng3D::State::handle_key(const Eng3D::Event::Key& e) {
    if(e.hold) {
        switch(e.type) {
        case Eng3D::Event::Key::Type::ESC:
            this->run = !this->run;
            break;
        case Eng3D::Event::Key::Type::F2:
            break;
        default:
            break;
        }
    }
}
