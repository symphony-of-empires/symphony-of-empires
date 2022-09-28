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
//      technology.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include "objects.hpp"

enum TechnologyType {
    STRATEGIC = 0x01,
    MILITARY = 0x02,
    NAVY = 0x04,
    SOCIAL = 0x08,
    ECONOMIC = 0x10,
    POLITICS = 0x20,
};

struct Technology : public RefnameEntity<TechnologyId> {
    Eng3D::StringRef name;
    Eng3D::StringRef description;
    float cost;
    enum TechnologyType type;
    std::vector<TechnologyId> req_technologies;
};
template<>
struct Serializer<Technology*>: public SerializerReference<World, Technology> {};
template<>
struct Serializer<const Technology*>: public SerializerReference<World, const Technology> {};
template<>
struct Serializer<Technology> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Technology& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.description);
        ::deser_dynamic<is_serialize>(ar, obj.cost);
        ::deser_dynamic<is_serialize>(ar, obj.req_technologies);
    }
};
