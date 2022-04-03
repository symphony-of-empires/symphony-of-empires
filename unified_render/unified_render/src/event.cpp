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
//      event.cpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/event.hpp"

#ifdef _MSC_VER
#   include <SDL.h>
#   include <SDL_events.h>
#   include <SDL_keycode.h>
#   include <SDL_mouse.h>
#else
#   include <SDL2/SDL.h>
#   include <SDL2/SDL_events.h>
#   include <SDL2/SDL_keycode.h>
#   include <SDL2/SDL_mouse.h>
#endif

UnifiedRender::Keyboard::Key UnifiedRender::Keyboard::from_sdlk(int sdlk)
{
	// Use a simple switch case jumptable
	switch(sdlk) {
	// Alphabet
	case SDLK_a:
		return UnifiedRender::Keyboard::Key::A;
	case SDLK_b:
		return UnifiedRender::Keyboard::Key::B;
	case SDLK_c:
		return UnifiedRender::Keyboard::Key::C;
	case SDLK_d:
		return UnifiedRender::Keyboard::Key::D;
	case SDLK_e:
		return UnifiedRender::Keyboard::Key::E;
	case SDLK_f:
		return UnifiedRender::Keyboard::Key::F;
	case SDLK_g:
		return UnifiedRender::Keyboard::Key::G;
	case SDLK_h:
		return UnifiedRender::Keyboard::Key::H;
	case SDLK_i:
		return UnifiedRender::Keyboard::Key::I;
	case SDLK_j:
		return UnifiedRender::Keyboard::Key::J;
	case SDLK_k:
		return UnifiedRender::Keyboard::Key::K;
	case SDLK_l:
		return UnifiedRender::Keyboard::Key::L;
	case SDLK_m:
		return UnifiedRender::Keyboard::Key::M;
	case SDLK_n:
		return UnifiedRender::Keyboard::Key::N;
	case SDLK_o:
		return UnifiedRender::Keyboard::Key::O;
	case SDLK_p:
		return UnifiedRender::Keyboard::Key::P;
	case SDLK_q:
		return UnifiedRender::Keyboard::Key::Q;
	case SDLK_r:
		return UnifiedRender::Keyboard::Key::R;
	case SDLK_s:
		return UnifiedRender::Keyboard::Key::S;
	case SDLK_t:
		return UnifiedRender::Keyboard::Key::T;
	case SDLK_u:
		return UnifiedRender::Keyboard::Key::U;
	case SDLK_v:
		return UnifiedRender::Keyboard::Key::V;
	case SDLK_w:
		return UnifiedRender::Keyboard::Key::W;
	case SDLK_x:
		return UnifiedRender::Keyboard::Key::X;
	case SDLK_y:
		return UnifiedRender::Keyboard::Key::Y;
	case SDLK_z:
		return UnifiedRender::Keyboard::Key::Z;
	// Numericals
	case SDLK_0:
		return UnifiedRender::Keyboard::Key::NUM_0;
	case SDLK_1:
		return UnifiedRender::Keyboard::Key::NUM_1;
	case SDLK_2:
		return UnifiedRender::Keyboard::Key::NUM_2;
	case SDLK_3:
		return UnifiedRender::Keyboard::Key::NUM_3;
	case SDLK_4:
		return UnifiedRender::Keyboard::Key::NUM_4;
	case SDLK_5:
		return UnifiedRender::Keyboard::Key::NUM_5;
	case SDLK_6:
		return UnifiedRender::Keyboard::Key::NUM_6;
	case SDLK_7:
		return UnifiedRender::Keyboard::Key::NUM_7;
	case SDLK_8:
		return UnifiedRender::Keyboard::Key::NUM_8;
	case SDLK_9:
		return UnifiedRender::Keyboard::Key::NUM_9;
	// Function
	case SDLK_F1:
		return UnifiedRender::Keyboard::Key::F1;
	case SDLK_F2:
		return UnifiedRender::Keyboard::Key::F2;
	case SDLK_F3:
		return UnifiedRender::Keyboard::Key::F3;
	case SDLK_F4:
		return UnifiedRender::Keyboard::Key::F4;
	case SDLK_F5:
		return UnifiedRender::Keyboard::Key::F5;
	case SDLK_F6:
		return UnifiedRender::Keyboard::Key::F6;
	case SDLK_F7:
		return UnifiedRender::Keyboard::Key::F7;
	case SDLK_F8:
		return UnifiedRender::Keyboard::Key::F8;
	case SDLK_F9:
		return UnifiedRender::Keyboard::Key::F9;
	case SDLK_F10:
		return UnifiedRender::Keyboard::Key::F10;
	case SDLK_F11:
		return UnifiedRender::Keyboard::Key::F11;
	case SDLK_F12:
		return UnifiedRender::Keyboard::Key::F12;
	// Miscellaneous
	case SDLK_SPACE:
		return UnifiedRender::Keyboard::Key::SPACE;
	case SDLK_BACKSPACE:
		return UnifiedRender::Keyboard::Key::BACKSPACE;
	case SDLK_ESCAPE:
		return UnifiedRender::Keyboard::Key::ESC;
	// Other
	default:
		return UnifiedRender::Keyboard::Key::NONE;
	}
}
