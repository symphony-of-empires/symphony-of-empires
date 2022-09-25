// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      entity.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/string.hpp"

/// @brief An entity which can only be referenced by an (presumably) unique Id
/// this is the base class for the other entity types.
/// @tparam IdType The type to use for the Id
template<typename IdType>
class Entity {
public:
    using Id = IdType;

};

/// @brief Alias for Entity that uses a cached_id for faster lookups
/// @tparam IdType The type used for the Id
template<typename IdType>
class IdEntity : public Entity<IdType> {
public:
    using Id = IdType;

    /// @brief Returns an invalid id
    /// @return constexpr Id The invalid id
    constexpr static Id invalid() {
        return (Id)-1;
    }

    /// @brief Checks if the id is not valid
    /// @param id Id to check
    /// @return true 
    /// @return false 
    constexpr static bool is_invalid(Id id) {
        return id == invalid();
    }

    /// @brief Checks if the id is valid
    /// @param id Id to check
    /// @return true 
    /// @return false 
    constexpr static bool is_valid(Id id) {
        return id != invalid();
    }

    /// @brief Id used to speed up Id lookups on any context
    /// @note This depends that the container is sequential (as if it was
    /// a contigous array) - Otherwise this optimization **will** break
    Id cached_id = (Id)-1;

    /// @brief Checks if the current id is invalid
    /// @return true 
    /// @return false 
    constexpr bool is_invalid() const {
        return is_invalid(cached_id);
    }

    /// @brief Checks if the current id is valid
    /// @return true 
    /// @return false 
    constexpr bool is_valid() const {
        return !is_invalid();
    }

    constexpr operator Id() const {
        return cached_id;
    }

    constexpr Id get_id() const {
        return cached_id;
    }
};

/// @brief An entity which can be referenced via a ref_name and also via id
/// @tparam IdType The type used for the Id
template<typename IdType>
class RefnameEntity : public IdEntity<IdType> {
public:
    Eng3D::StringRef ref_name;
};
