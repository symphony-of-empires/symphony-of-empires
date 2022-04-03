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
//      sound.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstring>
#include <algorithm>

#ifdef _MSC_VER
#   include <SDL.h>
#   include <SDL_audio.h>
#else
#   include <SDL2/SDL.h>
#   include <SDL2/SDL_audio.h>
#endif

#include <filesystem>
#include "stb_vorbis.c"

#include "unified_render/audio.hpp"
#include "unified_render/print.hpp"

//
// Audio
//
UnifiedRender::Audio::Audio() {
    
}

UnifiedRender::Audio::Audio(const std::string& path) {
    SDL_AudioSpec wave;
    SDL_AudioCVT cvt;
    
    int channels, rate;
    uint8_t* decoded;
    this->len = stb_vorbis_decode_filename(path.c_str(), &channels, &rate, (short**)&decoded);
    if(!this->len) {
        throw UnifiedRender::AudioException(path, "0 length audio");
    }
    this->len = this->len * channels * (sizeof(int16_t) / sizeof(uint8_t));
    this->data = decoded;
    this->pos = 0;

    // stb already loads OGG as a series of U16 nodes, so we only have to use AUDIO_S16
    // and the rest is already given by stb
    SDL_BuildAudioCVT(&cvt, AUDIO_S16, channels, rate, AUDIO_S16, 1, 11050);
    cvt.buf = (Uint8*)malloc(this->len * cvt.len_mult);
    if(cvt.buf == nullptr) {
        throw UnifiedRender::AudioException(path, "Cannot allocate memory");
    }
    std::memcpy(cvt.buf, this->data, this->len);
    cvt.len = this->len;
    SDL_ConvertAudio(&cvt);

    this->data = cvt.buf;
    this->len = cvt.len_cvt;
    this->pos = 0;
}

UnifiedRender::Audio::~Audio() {
    free(this->data);
}

//
// Audio manager
//
UnifiedRender::AudioManager::~AudioManager(void) {
    for(const auto& sound : sounds) {
        delete sound.second;
    }
    sounds.clear();
}

const UnifiedRender::Audio& UnifiedRender::AudioManager::load(const std::string& path) {
    // Find Sound when wanting to be loaded
    std::map<std::string, UnifiedRender::Audio*>::const_iterator it = sounds.find(path);
    if(it != sounds.cend()) {
        return *((*it).second);
    }

    // Otherwise Sound is not in our control, so we create a new one
    UnifiedRender::Audio* sound = new UnifiedRender::Audio(path);
    sounds[path] = sound;
    print_info("Loaded and cached sound %s (%zu b)", path.c_str(), sound->len);

    return *(static_cast<const UnifiedRender::Audio*>(sound));
}