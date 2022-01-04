// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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

// An entity which can only be made referenced by an id
template<typename IdType>
class Entity {
public:
    using Id = IdType;

    // Id used to speed up Id lookups on any context
    // NOTE: This depends that the container is sequential (as if it was
    // a contigous array) - Otherwise this optimization **WILL** break
    mutable Id cached_id = (Id)-1;
};

template<typename IdType>
class IdEntity : public Entity<IdType> {
public:
};

// An entity which can be referenced via a ref_name and also via id
template<typename IdType>
class RefnameEntity : public IdEntity<IdType> {
public:
    std::string ref_name;
    std::string name;
};