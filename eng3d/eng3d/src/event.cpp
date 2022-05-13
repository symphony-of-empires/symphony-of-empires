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
//      event.cpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>

#include "eng3d/event.hpp"

Eng3D::Keyboard::Key Eng3D::Keyboard::from_sdlk(int sdlk)
{
	// Use a simple switch case jumptable
	switch(sdlk) {
	// Alphabet
	case SDLK_a:
		return Eng3D::Keyboard::Key::A;
	case SDLK_b:
		return Eng3D::Keyboard::Key::B;
	case SDLK_c:
		return Eng3D::Keyboard::Key::C;
	case SDLK_d:
		return Eng3D::Keyboard::Key::D;
	case SDLK_e:
		return Eng3D::Keyboard::Key::E;
	case SDLK_f:
		return Eng3D::Keyboard::Key::F;
	case SDLK_g:
		return Eng3D::Keyboard::Key::G;
	case SDLK_h:
		return Eng3D::Keyboard::Key::H;
	case SDLK_i:
		return Eng3D::Keyboard::Key::I;
	case SDLK_j:
		return Eng3D::Keyboard::Key::J;
	case SDLK_k:
		return Eng3D::Keyboard::Key::K;
	case SDLK_l:
		return Eng3D::Keyboard::Key::L;
	case SDLK_m:
		return Eng3D::Keyboard::Key::M;
	case SDLK_n:
		return Eng3D::Keyboard::Key::N;
	case SDLK_o:
		return Eng3D::Keyboard::Key::O;
	case SDLK_p:
		return Eng3D::Keyboard::Key::P;
	case SDLK_q:
		return Eng3D::Keyboard::Key::Q;
	case SDLK_r:
		return Eng3D::Keyboard::Key::R;
	case SDLK_s:
		return Eng3D::Keyboard::Key::S;
	case SDLK_t:
		return Eng3D::Keyboard::Key::T;
	case SDLK_u:
		return Eng3D::Keyboard::Key::U;
	case SDLK_v:
		return Eng3D::Keyboard::Key::V;
	case SDLK_w:
		return Eng3D::Keyboard::Key::W;
	case SDLK_x:
		return Eng3D::Keyboard::Key::X;
	case SDLK_y:
		return Eng3D::Keyboard::Key::Y;
	case SDLK_z:
		return Eng3D::Keyboard::Key::Z;
	// Numericals
	case SDLK_0:
		return Eng3D::Keyboard::Key::NUM_0;
	case SDLK_1:
		return Eng3D::Keyboard::Key::NUM_1;
	case SDLK_2:
		return Eng3D::Keyboard::Key::NUM_2;
	case SDLK_3:
		return Eng3D::Keyboard::Key::NUM_3;
	case SDLK_4:
		return Eng3D::Keyboard::Key::NUM_4;
	case SDLK_5:
		return Eng3D::Keyboard::Key::NUM_5;
	case SDLK_6:
		return Eng3D::Keyboard::Key::NUM_6;
	case SDLK_7:
		return Eng3D::Keyboard::Key::NUM_7;
	case SDLK_8:
		return Eng3D::Keyboard::Key::NUM_8;
	case SDLK_9:
		return Eng3D::Keyboard::Key::NUM_9;
	// Function
	case SDLK_F1:
		return Eng3D::Keyboard::Key::F1;
	case SDLK_F2:
		return Eng3D::Keyboard::Key::F2;
	case SDLK_F3:
		return Eng3D::Keyboard::Key::F3;
	case SDLK_F4:
		return Eng3D::Keyboard::Key::F4;
	case SDLK_F5:
		return Eng3D::Keyboard::Key::F5;
	case SDLK_F6:
		return Eng3D::Keyboard::Key::F6;
	case SDLK_F7:
		return Eng3D::Keyboard::Key::F7;
	case SDLK_F8:
		return Eng3D::Keyboard::Key::F8;
	case SDLK_F9:
		return Eng3D::Keyboard::Key::F9;
	case SDLK_F10:
		return Eng3D::Keyboard::Key::F10;
	case SDLK_F11:
		return Eng3D::Keyboard::Key::F11;
	case SDLK_F12:
		return Eng3D::Keyboard::Key::F12;
	// Miscellaneous
	case SDLK_SPACE:
		return Eng3D::Keyboard::Key::SPACE;
	case SDLK_BACKSPACE:
		return Eng3D::Keyboard::Key::BACKSPACE;
	case SDLK_ESCAPE:
		return Eng3D::Keyboard::Key::ESC;
	// Other
	default:
		return Eng3D::Keyboard::Key::NONE;
	}
}
