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
//      ttf.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cassert>
#include <stdexcept>
#include <SDL_ttf.h>
#include "eng3d/ttf.hpp"
#include "eng3d/io.hpp"
#include "eng3d/utils.hpp"

Eng3D::TrueType::Font::Font(std::shared_ptr<Eng3D::IO::Asset::Base> asset, int dpi) {
    this->sdl_font = static_cast<void*>(TTF_OpenFont(asset->get_abs_path().c_str(), dpi));
    if(this->sdl_font == nullptr)
        CXX_THROW(std::runtime_error, std::string() + "Failed to load font " + asset->get_abs_path());
}

Eng3D::TrueType::Font::~Font() {
    if(this->sdl_font != nullptr)
        TTF_CloseFont(static_cast<TTF_Font*>(this->sdl_font));
}

Eng3D::TrueType::Manager::Manager(Eng3D::State& _s)
    : s{ _s }
{
    if(TTF_Init() < 0)
        CXX_THROW(std::runtime_error, std::string() + "Failed to init TTF " + TTF_GetError());
}

Eng3D::TrueType::Manager::~Manager() {
    // It's important to first destroy all fonts then quite the TTF subsystem
    // otherwise we will encounter some nasty sigsegvs
    this->fonts.clear();
    TTF_Quit();
}

std::shared_ptr<Eng3D::TrueType::Font> Eng3D::TrueType::Manager::load(std::shared_ptr<Eng3D::IO::Asset::Base> asset) {
    const std::string path = asset.get() != nullptr ? asset->get_abs_path() : "";
    const auto it = this->fonts.find(path);
    if(it != this->fonts.cend())
        return it->second;
    
    auto font = std::make_shared<Eng3D::TrueType::Font>(asset, 16);
    this->fonts[path] = font;
    return this->fonts[path];
}

