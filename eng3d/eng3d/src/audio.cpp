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

#include "stb_vorbis.c"

#include "eng3d/audio.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"
#include "eng3d/state.hpp"

//
// Audio
//
Eng3D::Audio::Audio(const std::string& path) {
    SDL_AudioCVT cvt;

    Eng3D::Log::debug("audio", "Decoding audio " + path);

    int channels, rate;
    uint8_t* decoded;
    this->len = stb_vorbis_decode_filename(path.c_str(), &channels, &rate, (short**)&decoded);
    if(!this->len)
        CXX_THROW(Eng3D::AudioException, path, "0 length audio");
    this->len = this->len * channels * (sizeof(int16_t) / sizeof(uint8_t));
    this->data = decoded;

    // stb already loads OGG as a series of U16 nodes, so we only have to use AUDIO_S16
    // and the rest is already given by stb
    SDL_BuildAudioCVT(&cvt, AUDIO_S16, channels, rate, AUDIO_S16, 1, 44100);
    cvt.buf = (Uint8*)malloc(this->len * cvt.len_mult);
    if(cvt.buf == nullptr)
        CXX_THROW(Eng3D::AudioException, path, "Can't allocate memory");
    std::memcpy(cvt.buf, this->data, this->len);
    cvt.len = this->len;
    SDL_ConvertAudio(&cvt);
    std::memcpy(this->data, cvt.buf, this->len);
    free(cvt.buf);
    this->len = cvt.len_cvt;
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
    fmt.channels = 1;
    fmt.samples = 16 * fmt.channels;
    fmt.callback = &Eng3D::AudioManager::mixaudio;
    fmt.userdata = this;
    if(SDL_OpenAudio(&fmt, NULL) < 0)
        CXX_THROW(std::runtime_error, "Unable to open audio: " + std::string(SDL_GetError()));
    SDL_PauseAudio(0);
}

void Eng3D::AudioManager::mixaudio(void* userdata, uint8_t* stream, int len) {
    Eng3D::AudioManager& audio_man = *(reinterpret_cast<Eng3D::AudioManager*>(userdata));
    std::memset(stream, 0, len);

    const std::scoped_lock lock(audio_man.sound_lock);
    for(size_t i = 0; i < audio_man.music_queue.size(); ) {
        auto& music = *audio_man.music_queue[i];
        const auto amount = std::min<int>(len, music.len - music.pos);
        const float volume = SDL_MIX_MAXVOLUME * audio_man.music_volume;
        const float fade = SDL_MIX_MAXVOLUME * audio_man.music_fade_value;
        SDL_MixAudio(stream, &music.data[music.pos], amount, std::min<int>(SDL_MIX_MAXVOLUME, volume - fade));
        music.pos += amount;
        if(music.pos >= music.len) {
            audio_man.music_queue.erase(audio_man.music_queue.begin() + i);
            continue;
        }
        i++;
    }

    for(size_t i = 0; i < audio_man.sound_queue.size(); ) {
        auto& sound = *audio_man.sound_queue[i];
        const auto amount = std::min<int>(len, sound.len - sound.pos);
        const float volume = SDL_MIX_MAXVOLUME * audio_man.sound_volume;
        SDL_MixAudio(stream, &sound.data[sound.pos], amount, std::min<int>(SDL_MIX_MAXVOLUME, volume));
        sound.pos += amount;
        if(sound.pos >= sound.len) {
            audio_man.sound_queue.erase(audio_man.sound_queue.begin() + i);
            continue;
        }
        i++;
    }

    if(audio_man.music_fade_value > 1.f)
        audio_man.music_fade_value -= 1.f;
}

const Eng3D::Audio& Eng3D::AudioManager::load(const std::string& path) {
    // Find Sound when wanting to be loaded
    auto it = sounds.find(path);
    if(it != sounds.cend())
        return *((*it).second);

    // Otherwise Sound is not in our control, so we create a new one
    sounds[path] = std::unique_ptr<Eng3D::Audio>(new Eng3D::Audio(path));
    Eng3D::Log::debug("audio", "Loaded and cached sound " + path + std::to_string(sounds[path]->len));
    return *sounds[path].get();
}