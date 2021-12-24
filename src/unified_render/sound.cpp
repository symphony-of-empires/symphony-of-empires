#include "unified_render/sound.hpp"
#include "unified_render/print.hpp"

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

using namespace UnifiedRender;

SoundManager* g_sound_manager = nullptr;

Sound::Sound(const std::string& path) {
    SDL_AudioSpec wave;
    SDL_AudioCVT cvt;
    
    if(SDL_LoadWAV(path.c_str(), &wave, &this->data, &this->len) == nullptr)
        throw SoundException(path, SDL_GetError());

    SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq, AUDIO_S16, 1, 11050);
    cvt.buf = (Uint8*)malloc(this->len * cvt.len_mult);
    std::memcpy(cvt.buf, this->data, this->len);
    cvt.len = this->len;
    SDL_ConvertAudio(&cvt);
    SDL_FreeWAV(this->data);

    SDL_LockAudio();
    this->data = cvt.buf;
    this->len = cvt.len_cvt;
    this->pos = 0;
    SDL_UnlockAudio();
}

Sound::Sound() {
    
}

Sound::~Sound() {
    free(this->data);
}

const Sound& SoundManager::load(const std::string& path) {
    // Find Sound when wanting to be loaded
    auto it = std::find_if(this->sounds.begin(), this->sounds.end(), [&path](const std::pair<UnifiedRender::Sound*, std::string>& element) {
        return (element.second == path);
    });

    // Load Sound from cached Sound list
    if(it != this->sounds.end())
        return *((*it).first);

    print_info("Loaded and cached sound %s", path.c_str());

    // Otherwise Sound is not in our control, so we create a new one
    auto* sound = new UnifiedRender::Sound();
    sound->data = nullptr;
    sound->len = 0;
    sound->pos = 0;
    this->sounds.insert(std::make_pair(sound, path));
    return *((const Sound*)sound);
}