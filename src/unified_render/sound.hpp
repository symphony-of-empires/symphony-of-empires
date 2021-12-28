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
