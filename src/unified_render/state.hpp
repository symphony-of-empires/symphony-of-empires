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

#include <vector>
#include <mutex>

#include "unified_render/io.hpp"

namespace UnifiedRender {
	class SoundManager;
	class TextureManager;
	class MaterialManager;
	class ModelManager;
	class Sound;
	namespace IO {
		class PackageManager;
	};

	struct State {
		SDL_Window* window;
		SDL_GLContext context;
		int width, height;
	public:
		State();
		~State();
		void clear(void) const;
		void swap(void) const;
		static void mixaudio(void* userdata, uint8_t* stream, int len);
		static State& get_instance(void);

		// Queue of sounds/music
		std::mutex sound_lock;
		std::vector<UnifiedRender::Sound*> sound_queue;
		std::vector<UnifiedRender::Sound*> music_queue;
		float music_fade_value = 1.f;

		UnifiedRender::SoundManager* sound_man;
		UnifiedRender::TextureManager* tex_man;
		UnifiedRender::MaterialManager* material_man;
    	UnifiedRender::ModelManager* model_man;
		UnifiedRender::IO::PackageManager* package_man;
	};
}
