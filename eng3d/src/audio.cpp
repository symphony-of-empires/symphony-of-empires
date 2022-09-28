// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
extern "C" {
#include "stb_vorbis.c"
}

//
// Audio
//
Eng3D::Audio::Audio(const std::string& path) {
    Eng3D::Log::debug("audio", Eng3D::translate_format("Decoding audio %s", path.c_str()));
    
    int err;
    this->stream = static_cast<void *>(stb_vorbis_open_filename(path.c_str(), &err, nullptr));
    if(this->stream == nullptr)
        CXX_THROW(Eng3D::AudioException, path, translate("Error opening audio"));
}

Eng3D::Audio::~Audio() {
    stb_vorbis_close(static_cast<stb_vorbis*>(this->stream));
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
    SDL_AudioSpec new_fmt{};
    this->audio_dev_id = SDL_OpenAudioDevice(NULL, 0, &fmt, &new_fmt, 0);
    if(this->audio_dev_id < 0)
        CXX_THROW(std::runtime_error, Eng3D::translate_format("Unable to open audio: %s", SDL_GetError()));
    SDL_PauseAudioDevice(this->audio_dev_id, 0);
}

Eng3D::AudioManager::~AudioManager() {
    SDL_PauseAudioDevice(this->audio_dev_id, 1);
    SDL_CloseAudioDevice(this->audio_dev_id);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void Eng3D::AudioManager::mixaudio(void* userdata, uint8_t* stream, int len) {
    auto& audio_man = *(static_cast<Eng3D::AudioManager*>(userdata));
    SDL_memset(stream, 0, len);

    auto audiobuf = std::unique_ptr<uint8_t[]>(new uint8_t[len]);

    const std::scoped_lock lock(audio_man.sound_lock);
    if(!audio_man.sound_queue.empty()) {
        auto& audio = **audio_man.sound_queue.begin();
        auto* audio_stream = static_cast<stb_vorbis*>(audio.stream);
        stb_vorbis_info info = stb_vorbis_get_info(audio_stream);
        if(stb_vorbis_get_samples_short_interleaved(audio_stream, info.channels, (short*)audiobuf.get(), len / sizeof(short)) == 0) {
            // Take off queue
            stb_vorbis_seek_start(audio_stream); // Rewind
            audio_man.sound_queue.erase(audio_man.sound_queue.begin());
        }
        for(size_t i = 0; i < len / sizeof(int16_t); i++) {
            auto sample = static_cast<float>(((int16_t *)audiobuf.get())[i]);
            ((int16_t *)stream)[i] += (int16_t)(sample * audio_man.sound_volume);
        }
    }
    
    if(!audio_man.music_queue.empty()) {
        auto& audio = **audio_man.music_queue.begin();
        auto* audio_stream = static_cast<stb_vorbis*>(audio.stream);
        stb_vorbis_info info = stb_vorbis_get_info(audio_stream);
        if(stb_vorbis_get_samples_short_interleaved(audio_stream, info.channels, (short*)audiobuf.get(), len / sizeof(short)) == 0) {
            // Take off queue
            stb_vorbis_seek_start(audio_stream); // Rewind
            audio_man.music_queue.erase(audio_man.music_queue.begin());
        }
        for(size_t i = 0; i < len / sizeof(int16_t); i++) {
            auto sample = static_cast<float>(((int16_t *)audiobuf.get())[i]);
            ((int16_t *)stream)[i] += (int16_t)(sample * audio_man.music_volume);
        }
    }
}

const std::shared_ptr<Eng3D::Audio> Eng3D::AudioManager::load(const std::string& path) {
    // Find Sound when wanting to be loaded
    auto it = sounds.find(path);
    if(it != sounds.cend())
        return (*it).second;

    // Otherwise Sound is not in our control, so we create a new one
    sounds[path] = std::make_shared<Eng3D::Audio>(path);
    Eng3D::Log::debug("audio", Eng3D::translate_format("Loaded and cached sound %s", path.c_str()));
    return sounds[path];
}