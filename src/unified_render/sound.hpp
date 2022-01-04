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
//      sound.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <exception>

namespace UnifiedRender {
    class SoundException : public std::exception {
        std::string buffer;
    public:
        SoundException(const std::string& filename, const std::string& message) {
            buffer = filename + message;
        }

        virtual const char* what(void) const noexcept {
            return buffer.c_str();
        }
    };

    class Sound {
    public:
        Sound();
        Sound(const std::string& path);
        ~Sound();

        uint8_t *data;
        uint32_t pos;
        uint32_t len;
    };

    class SoundManager {
    private:
        std::set<std::pair<UnifiedRender::Sound*, std::string>> sounds;
    public:
        const Sound& load(const std::string& path);
    };
};
