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
//      entity.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <concepts>
#include <limits>
#include "eng3d/string.hpp"

template<typename T>
struct EntityId {
    using Type = T;
    constexpr static T invalid_id = 0;
    T id = invalid_id;

    constexpr EntityId<T>() noexcept = default;
    constexpr EntityId<T>(T _id) noexcept : id{ _id } { }
    constexpr EntityId<T>(size_t _id) noexcept : id{ static_cast<T>(_id) } { }
    constexpr EntityId<T>(EntityId<T>&&) noexcept = default;
    constexpr EntityId<T>(const EntityId<T>&) noexcept = default;
    constexpr EntityId<T>& operator=(const EntityId<T>&) noexcept = default;
    constexpr ~EntityId() noexcept = default;

    constexpr operator size_t() const noexcept {
        return static_cast<size_t>(id);
    }
    
    constexpr auto operator==(const EntityId<T>& o) const noexcept {
        return id == o.id;
    }

    constexpr auto operator<=>(const EntityId<T>& o) const noexcept = default;

    EntityId<T>& operator++() noexcept {
        this->id++;
        return *this;
    }

    EntityId<T>& operator--() noexcept {
        this->id--;
        return *this;
    }

    EntityId<T> operator++(int) noexcept {
        this->id++;
        return *this;
    }

    EntityId<T> operator--(int) noexcept {
        this->id--;
        return *this;
    }
};

template<typename T>
struct std::hash<EntityId<T>> {
    std::size_t operator()(const EntityId<T>& id) const noexcept {
        return std::hash(id.id);
    }
};

template<typename T>
struct std::equal_to<EntityId<T>> {
    constexpr bool operator()(const EntityId<T>& a, const EntityId<T>& b) const {
        return a == b;
    }
};

/// @brief An entity which can only be referenced by an (presumably) unique Id
/// this is the base class for the other entity types.
/// @tparam T The type used for the Id
template<typename T>
struct Entity {
    using Id = T;

    constexpr Entity() noexcept = default;
    constexpr Entity(Entity&&) noexcept = default;
    constexpr Entity(const Entity&) noexcept = default;
    constexpr Entity& operator=(const Entity&) noexcept = default;
    constexpr ~Entity() noexcept = default;

    /// @brief Id used to speed up Id lookups on any context
    /// @note This depends that the container is sequential (as if it was
    /// a contigous array) - Otherwise this optimization **will** break
    Id cached_id;

    /// @brief Returns an invalid id
    /// @return constexpr Id The invalid id
    constexpr static Id invalid() {
        return Id{ static_cast<T>(-1) };
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

    constexpr operator Id() const noexcept {
        return cached_id;
    }

    constexpr operator size_t() const noexcept {
        return static_cast<size_t>(cached_id);
    }

    constexpr Id get_id() const {
        return cached_id;
    }
};

/// @brief An entity which can be referenced via a ref_name and also via id
/// @tparam T The type used for the Id
template<typename T>
struct RefnameEntity : Entity<T> {
    Eng3D::StringRef ref_name;
};
