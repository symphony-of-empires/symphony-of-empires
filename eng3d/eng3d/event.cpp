// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      event.cpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"

#include "eng3d/event.hpp"

Eng3D::Event::Key::Type Eng3D::Event::Key::from_sdl(int sdl) {
    // Use a simple switch case jumptable
    switch(sdl) {
    // Alphabet
    case SDLK_a: return Eng3D::Event::Key::Type::A;
    case SDLK_b: return Eng3D::Event::Key::Type::B;
    case SDLK_c: return Eng3D::Event::Key::Type::C;
    case SDLK_d: return Eng3D::Event::Key::Type::D;
    case SDLK_e: return Eng3D::Event::Key::Type::E;
    case SDLK_f: return Eng3D::Event::Key::Type::F;
    case SDLK_g: return Eng3D::Event::Key::Type::G;
    case SDLK_h: return Eng3D::Event::Key::Type::H;
    case SDLK_i: return Eng3D::Event::Key::Type::I;
    case SDLK_j: return Eng3D::Event::Key::Type::J;
    case SDLK_k: return Eng3D::Event::Key::Type::K;
    case SDLK_l: return Eng3D::Event::Key::Type::L;
    case SDLK_m: return Eng3D::Event::Key::Type::M;
    case SDLK_n: return Eng3D::Event::Key::Type::N;
    case SDLK_o: return Eng3D::Event::Key::Type::O;
    case SDLK_p: return Eng3D::Event::Key::Type::P;
    case SDLK_q: return Eng3D::Event::Key::Type::Q;
    case SDLK_r: return Eng3D::Event::Key::Type::R;
    case SDLK_s: return Eng3D::Event::Key::Type::S;
    case SDLK_t: return Eng3D::Event::Key::Type::T;
    case SDLK_u: return Eng3D::Event::Key::Type::U;
    case SDLK_v: return Eng3D::Event::Key::Type::V;
    case SDLK_w: return Eng3D::Event::Key::Type::W;
    case SDLK_x: return Eng3D::Event::Key::Type::X;
    case SDLK_y: return Eng3D::Event::Key::Type::Y;
    case SDLK_z: return Eng3D::Event::Key::Type::Z;
    // Numericals
    case SDLK_0: return Eng3D::Event::Key::Type::NUM_0;
    case SDLK_1: return Eng3D::Event::Key::Type::NUM_1;
    case SDLK_2: return Eng3D::Event::Key::Type::NUM_2;
    case SDLK_3: return Eng3D::Event::Key::Type::NUM_3;
    case SDLK_4: return Eng3D::Event::Key::Type::NUM_4;
    case SDLK_5: return Eng3D::Event::Key::Type::NUM_5;
    case SDLK_6: return Eng3D::Event::Key::Type::NUM_6;
    case SDLK_7: return Eng3D::Event::Key::Type::NUM_7;
    case SDLK_8: return Eng3D::Event::Key::Type::NUM_8;
    case SDLK_9: return Eng3D::Event::Key::Type::NUM_9;
    // Function
    case SDLK_F1: return Eng3D::Event::Key::Type::F1;
    case SDLK_F2: return Eng3D::Event::Key::Type::F2;
    case SDLK_F3: return Eng3D::Event::Key::Type::F3;
    case SDLK_F4: return Eng3D::Event::Key::Type::F4;
    case SDLK_F5: return Eng3D::Event::Key::Type::F5;
    case SDLK_F6: return Eng3D::Event::Key::Type::F6;
    case SDLK_F7: return Eng3D::Event::Key::Type::F7;
    case SDLK_F8: return Eng3D::Event::Key::Type::F8;
    case SDLK_F9: return Eng3D::Event::Key::Type::F9;
    case SDLK_F10: return Eng3D::Event::Key::Type::F10;
    case SDLK_F11: return Eng3D::Event::Key::Type::F11;
    case SDLK_F12: return Eng3D::Event::Key::Type::F12;
    // Arrow keys
    case SDLK_UP: return Eng3D::Event::Key::Type::UP;
    case SDLK_DOWN: return Eng3D::Event::Key::Type::DOWN;
    case SDLK_LEFT: return Eng3D::Event::Key::Type::LEFT;
    case SDLK_RIGHT: return Eng3D::Event::Key::Type::RIGHT;
    // Miscellaneous
    case SDLK_SPACE: return Eng3D::Event::Key::Type::SPACE;
    case SDLK_BACKSPACE: return Eng3D::Event::Key::Type::BACKSPACE;
    case SDLK_ESCAPE: return Eng3D::Event::Key::Type::ESC;
    case SDLK_BACKSLASH: // Fallthrough
    case SDLK_BACKQUOTE: return Eng3D::Event::Key::Type::TILDE;
    }
    return Eng3D::Event::Key::Type::NONE;
}

Eng3D::Event::MouseButton::Type Eng3D::Event::MouseButton::from_sdl(int sdl) {
    switch(sdl) {
    case SDL_BUTTON_LEFT: return Eng3D::Event::MouseButton::Type::LEFT;
    case SDL_BUTTON_RIGHT: return Eng3D::Event::MouseButton::Type::RIGHT;
    case SDL_BUTTON_MIDDLE: return Eng3D::Event::MouseButton::Type::MIDDLE;
    }
    return Eng3D::Event::MouseButton::Type::NONE;
}

glm::ivec2 Eng3D::Event::get_mouse_pos() {
    int sdl_x, sdl_y;
    SDL_GetMouseState(&sdl_x, &sdl_y);
    return glm::ivec2{ sdl_x, sdl_y };
}
