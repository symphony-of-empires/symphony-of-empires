// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
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
//      terrain.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include "eng3d/string.hpp"
#include "eng3d/color.hpp"
#include "objects.hpp"

struct TerrainType : RefnameEntity<TerrainTypeId> {
    Eng3D::StringRef name;
    uint32_t color; // Associative color (with bitmap)
    bool is_water_body; // Should this terrain be treated as sea?
    float penalty = 1.f; // Penalty imposed to various contexts
    std::string get_icon_path() const {
        return string_format("gfx/terraintype/%s.png", ref_name.c_str());
    }
};
template<>
struct Eng3D::Deser::Serializer<TerrainType> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, TerrainType>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.color);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.penalty);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.is_water_body);
    }
};
