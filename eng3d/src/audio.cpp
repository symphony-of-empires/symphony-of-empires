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
//      sound.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstring>
#include <algorithm>

#include <SDL.h>
#include <SDL_audio.h>

#include "eng3d/audio.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"
#include "eng3d/state.hpp"
#include "stb_vorbis.c"

//
// Audio
//
Eng3D::Audio::Audio(const std::string& path) {
    Eng3D::Log::debug("audio", "Decoding audio " + path);

    int err;
    this->stream = reinterpret_cast<void *>(stb_vorbis_open_filename(path.c_str(), &err, nullptr));
    if(this->stream == nullptr)
        CXX_THROW(Eng3D::AudioException, path, "error opening audio");
}

Eng3D::Audio::~Audio() {
    stb_vorbis_close(reinterpret_cast<stb_vorbis*>(this->stream));
}

//
// Audio manager
//
Eng3D::AudioManager::AudioManager(Eng3D::State& _s)
    : s{ _s }
{
    // Initialize sound subsystem (at 11,050 hz)
    SDL_AudioSpec fmt;
    fmt.freq = 44100;
    fmt.format = AUDIO_S16;
    fmt.channels = 2;
    fmt.samples = 16;
    fmt.callback = &Eng3D::AudioManager::mixaudio;
    fmt.userdata = this;
    if(SDL_OpenAudio(&fmt, NULL) < 0)
        CXX_THROW(std::runtime_error, "Unable to open audio: " + std::string(SDL_GetError()));
    SDL_PauseAudio(0);
}

void Eng3D::AudioManager::mixaudio(void* userdata, uint8_t* stream, int len) {
    auto& audio_man = *(reinterpret_cast<Eng3D::AudioManager*>(userdata));
    SDL_memset(stream, 0, len);

    auto audiobuf = std::unique_ptr<uint8_t[]>(new uint8_t[len]);

    const std::scoped_lock lock(audio_man.sound_lock);
    if(!audio_man.sound_queue.empty()) {
        auto& audio = **audio_man.sound_queue.begin();
        auto* audio_stream = reinterpret_cast<stb_vorbis*>(audio.stream);
        stb_vorbis_info info = stb_vorbis_get_info(audio_stream);
        if(stb_vorbis_get_samples_short_interleaved(audio_stream, info.channels, (short*)audiobuf.get(), len / sizeof(short)) == 0) {
            // Take off queue
            stb_vorbis_seek_start(audio_stream); // Rewind
            audio_man.sound_queue.erase(audio_man.sound_queue.begin());
        }
        SDL_MixAudio(stream, audiobuf.get(), len, SDL_MIX_MAXVOLUME * audio_man.sound_volume);
    } else if(!audio_man.music_queue.empty()) {
        auto& audio = **audio_man.music_queue.begin();
        auto* audio_stream = reinterpret_cast<stb_vorbis*>(audio.stream);
        stb_vorbis_info info = stb_vorbis_get_info(audio_stream);
        if(stb_vorbis_get_samples_short_interleaved(audio_stream, info.channels, (short*)audiobuf.get(), len / sizeof(short)) == 0) {
            // Take off queue
            stb_vorbis_seek_start(audio_stream); // Rewind
            audio_man.music_queue.erase(audio_man.music_queue.begin());
        }
        SDL_MixAudio(stream, audiobuf.get(), len, SDL_MIX_MAXVOLUME * audio_man.music_volume);
    }
}

const std::shared_ptr<Eng3D::Audio> Eng3D::AudioManager::load(const std::string& path) {
    // Find Sound when wanting to be loaded
    auto it = sounds.find(path);
    if(it != sounds.cend())
        return (*it).second;

    // Otherwise Sound is not in our control, so we create a new one
    sounds[path] = std::shared_ptr<Eng3D::Audio>(new Eng3D::Audio(path));
    Eng3D::Log::debug("audio", "Loaded and cached sound " + path);
    return sounds[path];
}