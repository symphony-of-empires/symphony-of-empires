#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace Eng3D {
    namespace IO::Asset {
        class Base;
    }
    class State;
}

namespace Eng3D::TrueType {
    class Font {
    public:
        Font() = delete;
        Font(std::shared_ptr<Eng3D::IO::Asset::Base> asset, int dpi);
        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;
        ~Font();
        void* sdl_font;
    };

    class Manager {
        std::unordered_map<std::string, std::shared_ptr<Eng3D::TrueType::Font>> fonts;
        Eng3D::State& s;
    public:
        Manager() = delete;
        Manager(Eng3D::State& s);
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        ~Manager();
        std::shared_ptr<Eng3D::TrueType::Font> load(std::shared_ptr<Eng3D::IO::Asset::Base> asset);
    };
}
