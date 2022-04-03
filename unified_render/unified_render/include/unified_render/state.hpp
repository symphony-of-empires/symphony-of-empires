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

#include <GL/glew.h>
#ifdef _MSC_VER
#   include <SDL.h>
#   include <SDL_events.h>
#   include <SDL_keycode.h>
#   include <SDL_mouse.h>
#   include <SDL_opengl.h>
#   include <SDL_ttf.h>
#   include <SDL_audio.h>
#	include <SDL_joystick.h>
#else
#   include <SDL2/SDL.h>
#   include <SDL2/SDL_events.h>
#   include <SDL2/SDL_keycode.h>
#   include <SDL2/SDL_mouse.h>
#   include <SDL2/SDL_opengl.h>
#   include <SDL2/SDL_ttf.h>
#   include <SDL2/SDL_audio.h>
#	include <SDL2/SDL_joystick.h>
#endif

#include "unified_render/io.hpp"
#include "unified_render/ui/ui.hpp"

namespace UnifiedRender {
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

		SDL_Joystick* joy = nullptr;
		float joy_sensivity = 2.f;

		// Queue of sounds/music
		std::mutex sound_lock;
		std::vector<UnifiedRender::Audio*> sound_queue;
		std::vector<UnifiedRender::Audio*> music_queue;
		float music_fade_value = 1.f;

		UnifiedRender::AudioManager* sound_man;
		UnifiedRender::TextureManager* tex_man;
		UnifiedRender::MaterialManager* material_man;
    	UnifiedRender::ModelManager* model_man;
		UnifiedRender::IO::PackageManager* package_man;
		UI::Context* ui_ctx;

		float music_volume = 50.f, sound_volume = 50.f;

		// Builtin shaders
		std::map<std::string, std::unique_ptr<UnifiedRender::OpenGL::Shader>> builtin_shaders;
	};
}
