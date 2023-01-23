// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      ttf.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace Eng3D {
    namespace IO::Asset {
        struct Base;
    }
    class State;
}

namespace Eng3D::TrueType {
    struct Font {
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
