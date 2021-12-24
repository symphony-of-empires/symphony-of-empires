#pragma once 

#include <GL/glew.h>
#ifdef _MSC_VER
#   include <SDL.h>
#   include <SDL_events.h>
#   include <SDL_keycode.h>
#   include <SDL_mouse.h>
#   include <SDL_opengl.h>
#   include <SDL_ttf.h>
#   include <SDL_audio.h>
#else
#   include <SDL2/SDL.h>
#   include <SDL2/SDL_events.h>
#   include <SDL2/SDL_keycode.h>
#   include <SDL2/SDL_mouse.h>
#   include <SDL2/SDL_opengl.h>
#   include <SDL2/SDL_ttf.h>
#   include <SDL2/SDL_audio.h>
#endif

#include "unified_render/sound.hpp"
#include "unified_render/asset.hpp"
#include <vector>
#include <mutex>

namespace UnifiedRender {
	struct State {
		SDL_Window* window;
		SDL_GLContext context;
		int width, height;
	public:
		State();
		~State();
		static void mixaudio(void* userdata, uint8_t* stream, int len);
		static State& get_instance(void);

		// Queue of sounds/music
		std::mutex sound_lock;
		std::vector<UnifiedRender::Sound*> sound_queue;
		std::vector<UnifiedRender::Sound*> music_queue;
		float music_fade_value = 1.f;

		std::vector<Package> packages;
	};
}
