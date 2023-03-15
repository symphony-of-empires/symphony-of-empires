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
//      audio.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <exception>
#include <mutex>
#include <vector>
#include <memory>
#include <string_view>

namespace Eng3D {
    class State;

    class AudioException : public std::exception {
        std::string buffer;
    public:
        AudioException(const std::string_view filename, const std::string_view message) {
            buffer = std::string(filename) + std::string(message);
        }

        virtual const char* what() const noexcept {
            return buffer.c_str();
        }
    };

    struct Audio {
        Audio() = default;
        Audio(const std::string_view path, bool is_sound);
        ~Audio();
        void *stream = nullptr;
        bool is_sound = false;
        int channel = -1;
    };

    class AudioManager {
        Eng3D::State& s;
        std::map<size_t, std::shared_ptr<Eng3D::Audio>> audios;
        std::shared_ptr<Eng3D::Audio> current_sound;
        std::shared_ptr<Eng3D::Audio> current_music;
        
        const std::shared_ptr<Audio> load(const std::string_view path, bool is_sound);
    public:
        AudioManager() = delete;
        AudioManager(Eng3D::State& s);
        ~AudioManager();
        void play_sound(const std::string_view path);
        bool can_play_sound();
        void play_music(const std::string_view path);
        bool can_play_music();

        float music_fade_value = 1.f;
        float music_volume = 0.5f, sound_volume = 0.5f;
    };
}
