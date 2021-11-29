#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <exception>

namespace UnifiedRender {
    class SoundException : public std::runtime_error {
        std::string msg;
    public:
        SoundException(const std::string& filename, const std::string& message) : runtime_error(filename + message) {};
    };

    class Sound {
    public:
        Sound(const std::string& path);
        Sound();
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

extern UnifiedRender::SoundManager* g_sound_manager;
