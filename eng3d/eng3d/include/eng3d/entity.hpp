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
//      entity.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include "eng3d/string.hpp"

// An entity which can only be made referenced by an id
template<typename IdType>
class Entity {
public:
    using Id = IdType;

    // Id used to speed up Id lookups on any context
    // NOTE: This depends that the container is sequential (as if it was
    // a contigous array) - Otherwise this optimization **WILL** break
    Id cached_id = (Id)-1;

    constexpr Id invalid(void) const {
        return (Id)-1;
    };
    
    constexpr bool is_invalid(Id id) const {
        return id == (Id)-1;
    };
    
    constexpr bool is_invalid(void) const {
        return this->is_invalid(this->id);
    };
};

template<typename IdType>
class IdEntity : public Entity<IdType> {
public:
    
};

// An entity which can be referenced via a ref_name and also via id
template<typename IdType>
class RefnameEntity : public IdEntity<IdType> {
public:
    Eng3D::StringRef ref_name;
    Eng3D::StringRef name;
};