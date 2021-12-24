#include "unified_render/state.hpp"
#include "unified_render/print.hpp"

#ifdef _MSC_VER
// Required before GL/gl.h
#   ifndef _WINDOWS_
#   define WIN32_LEAN_AND_MEAN 1
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

#include <cstring>

using namespace UnifiedRender;

static State* g_state = nullptr;

State::State(void) {
    g_state = this;

	// Startup-initialization of subsystems
	SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	// Create the initial window
	width = 1280;
	height = 800;
    window = SDL_CreateWindow("Symphony of Empires", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
	// OpenGL configurations
	context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    
	print_info("OpenGL Version: %s", glGetString(GL_VERSION));
	
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(err != GLEW_OK)
        throw std::runtime_error("Failed to init GLEW");

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

    // Initialize sound subsystem (at 11,050 hz)
    SDL_AudioSpec fmt;
    fmt.freq = 11050;
    fmt.format = AUDIO_S16;
    fmt.channels = 1;
    fmt.samples = 512;
    fmt.callback = &State::mixaudio;
    fmt.userdata = this;
    if(SDL_OpenAudio(&fmt, NULL) < 0)
        throw std::runtime_error("Unable to open audio: " + std::string(SDL_GetError()));
    SDL_PauseAudio(0);
}

State::~State(void) {
	SDL_CloseAudio();
	
    TTF_Quit();
    SDL_Quit();

    g_state = nullptr;
}

void State::mixaudio(void* userdata, uint8_t* stream, int len) {
    State& gs = *((State*)userdata);
    std::memset(stream, 0, len);

    if(gs.sound_lock.try_lock()) {
        for(unsigned int i = 0; i < gs.sound_queue.size(); ) {
            int size = gs.sound_queue.size();
            UnifiedRender::Sound* sound = gs.sound_queue[i];
            int amount = sound->len - sound->pos;
            if(amount > len) amount = len;
            if(amount <= 0) {
                delete sound;
                gs.sound_queue.erase(gs.sound_queue.begin() + i);
                continue;
            }

            SDL_MixAudio(stream, &sound->data[sound->pos], amount, SDL_MIX_MAXVOLUME);
            sound->pos += amount;
            i++;
        }

        for(unsigned int i = 0; i < gs.music_queue.size(); ) {
            UnifiedRender::Sound* music = gs.music_queue[i];
            int amount = music->len - music->pos;
            if(amount > len) amount = len;
            if(amount <= 0) {
                delete music;
                gs.music_queue.erase(gs.music_queue.begin() + i);
                continue;
            }

            SDL_MixAudio(stream, &music->data[music->pos], amount, SDL_MIX_MAXVOLUME / gs.music_fade_value);
            music->pos += amount;
            i++;
        }
        gs.sound_lock.unlock();
    }

    if(gs.music_fade_value > 1.f) gs.music_fade_value -= 1.f;
}

State& State::get_instance(void) {
    return *g_state;
}