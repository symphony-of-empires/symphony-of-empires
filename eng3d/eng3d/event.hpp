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
//      event.hpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <glm/vec2.hpp>

namespace Eng3D::Event {
    struct Key {
        enum class Type : unsigned char {
            /// @todo What usecase this even has?
            NONE,
            // Alphabet
            A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            // Numerals
            NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
            // Function keys
            F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
            UP, DOWN, LEFT, RIGHT,
            // Miscellaneous
            SPACE,
            BACKSPACE,
            ESC,
            TILDE,
        };
        Eng3D::Event::Key::Type type = Eng3D::Event::Key::Type::NONE;
        /// @brief Whetever the key is being held
        bool hold;
        Eng3D::Event::Key::Type from_sdl(int sdl);
    };

    struct MouseButton {
        enum class Type : unsigned char {
            NONE,
            LEFT, MIDDLE, RIGHT,
        };
        /// @brief Button that is checked
        Eng3D::Event::MouseButton::Type type = Eng3D::Event::MouseButton::Type::NONE;
        /// @brief Whetever the button is being held
        bool hold;
        Eng3D::Event::MouseButton::Type from_sdl(int sdl);
        glm::ivec2 pos; // Absolute position of the mouse
    };

    struct MouseMotion {
        glm::ivec2 pos; // Absolute position of the mouse
        glm::ivec2 delta; // Difference from previous and current mouse position
    };

    struct MouseWheel {
        glm::ivec2 wheel; // Wheel relative movement
        glm::ivec2 pos; // Absolute position of the mouse
    };
    
    glm::ivec2 get_mouse_pos();
}
