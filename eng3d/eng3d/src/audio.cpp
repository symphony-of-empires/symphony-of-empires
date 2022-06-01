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

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include <filesystem>
#include "stb_vorbis.c"

#include "eng3d/audio.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"

//
// Audio
//
Eng3D::Audio::Audio(const std::string& path) {
    SDL_AudioCVT cvt;

    int channels, rate;
    uint8_t* decoded;
    this->len = stb_vorbis_decode_filename(path.c_str(), &channels, &rate, (short**)&decoded);
    if(!this->len)
        CXX_THROW(Eng3D::AudioException, path, "0 length audio");
    this->len = this->len * channels * (sizeof(int16_t) / sizeof(uint8_t));
    this->data = decoded;
    this->pos = 0;

    // stb already loads OGG as a series of U16 nodes, so we only have to use AUDIO_S16
    // and the rest is already given by stb
    SDL_BuildAudioCVT(&cvt, AUDIO_S16, channels, rate, AUDIO_S16, 1, 11050);
    cvt.buf = (Uint8*)malloc(this->len * cvt.len_mult);
    if(cvt.buf == nullptr)
        CXX_THROW(Eng3D::AudioException, path, "Cannot allocate memory");
    std::memcpy(cvt.buf, this->data, this->len);
    cvt.len = this->len;
    SDL_ConvertAudio(&cvt);

    this->data = cvt.buf;
    this->len = cvt.len_cvt;
    this->pos = 0;
}

//
// Audio manager
//
Eng3D::AudioManager::~AudioManager() {
    for(const auto& sound : sounds)
        delete sound.second;
    sounds.clear();
}

const Eng3D::Audio& Eng3D::AudioManager::load(const std::string& path) {
    // Find Sound when wanting to be loaded
    std::map<std::string, Eng3D::Audio*>::const_iterator it = sounds.find(path);
    if(it != sounds.cend())
        return *((*it).second);

    // Otherwise Sound is not in our control, so we create a new one
    Eng3D::Audio* sound = new Eng3D::Audio(path);
    sounds[path] = sound;
    Eng3D::Log::debug("audio", "Loaded and cached sound " + path + std::to_string(sound->len));
    return *(static_cast<const Eng3D::Audio*>(sound));
}