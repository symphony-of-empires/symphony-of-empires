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
#include "unified_render/stb_vorbis.c"

using namespace UnifiedRender;

Sound::Sound(const std::string& path) {
    SDL_AudioSpec wave;
    SDL_AudioCVT cvt;
    
    int channels, rate;
    uint8_t* decoded;
    this->len = stb_vorbis_decode_filename(path.c_str(), &channels, &rate, (short**)&decoded);
    this->len = this->len * channels * (sizeof(int16_t) / sizeof(uint8_t));
    this->data = decoded;
    this->pos = 0;

    /**
     * stb already loads OGG as a series of U16 nodes, so we only have to use AUDIO_S16
     * and the rest is already given by stb
     */
    SDL_BuildAudioCVT(&cvt, AUDIO_S16, channels, rate, AUDIO_S16, 1, 11050);
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
    auto it = std::find_if(this->sounds.begin(), this->sounds.end(), [&path](const auto& element) {
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