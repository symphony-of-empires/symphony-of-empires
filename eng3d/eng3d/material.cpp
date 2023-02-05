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
//      material.cpp
//
// Abstract:
//      Defines functions for the loading, management, creation and destruction
//      of a material resource.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <fstream>
#include <string>

#include "eng3d/material.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/state.hpp"
#include "eng3d/log.hpp"

//
// Material manager
//
const std::shared_ptr<Eng3D::Material> Eng3D::MaterialManager::load(const std::string_view name) {
    const auto key = std::hash<std::string_view>{}(name);
    auto it = materials.find(key);
    if(it != materials.end()) return it->second;
    
    Eng3D::Log::error("material", Eng3D::translate_format("%s not found", name.data()));
    std::shared_ptr<Eng3D::Material> material = std::make_shared<Eng3D::Material>();
    materials[key] = material;
    return material;
}
