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
//      ttf.cpp
//
// Abstract:
//      Provides functions to render TTF fonts and load them as required.
// ----------------------------------------------------------------------------

#include <cassert>
#include <stdexcept>
#include "SDL_ttf.h"
#include "eng3d/ttf.hpp"
#include "eng3d/io.hpp"
#include "eng3d/string.hpp"
#include "eng3d/utils.hpp"

Eng3D::TrueType::Font::Font(std::shared_ptr<Eng3D::IO::Asset::Base> asset, int dpi) {
    this->sdl_font = static_cast<void*>(TTF_OpenFont(asset->abs_path.c_str(), dpi));
    if(this->sdl_font == nullptr)
        CXX_THROW(std::runtime_error, translate_format("Failed to load font %s", asset->abs_path.c_str()));
}

Eng3D::TrueType::Font::~Font() {
    if(this->sdl_font != nullptr)
        TTF_CloseFont(static_cast<TTF_Font*>(this->sdl_font));
}

Eng3D::TrueType::Manager::Manager(Eng3D::State& _s)
    : s{ _s }
{
    if(TTF_Init() < 0)
        CXX_THROW(std::runtime_error, translate_format("Failed to initialize TTF %s", TTF_GetError()));
}

Eng3D::TrueType::Manager::~Manager() {
    // It's important to first destroy all fonts then quite the TTF subsystem
    // otherwise we will encounter some nasty sigsegvs
    this->fonts.clear();
    TTF_Quit();
}

std::shared_ptr<Eng3D::TrueType::Font> Eng3D::TrueType::Manager::load(std::shared_ptr<Eng3D::IO::Asset::Base> asset) {
    const std::string path = asset.get() != nullptr ? asset->abs_path : "";
    const auto it = this->fonts.find(path);
    if(it != this->fonts.cend())
        return it->second;
    
    auto font = std::make_shared<Eng3D::TrueType::Font>(asset, 16);
    this->fonts[path] = font;
    return this->fonts[path];
}

