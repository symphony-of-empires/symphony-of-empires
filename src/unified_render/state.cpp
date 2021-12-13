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

using namespace UnifiedRender;

State::State(void) {
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
}

State::~State(void) {
	SDL_CloseAudio();
	
    TTF_Quit();
    SDL_Quit();
}