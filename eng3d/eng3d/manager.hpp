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
//      manager.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <stdexcept>
#include <unordered_set>

// Generic manager for any resource type, the manager will call "load" if an element
// with the same ident already exists
template<typename K, typename V>
class Manager {
    std::unordered_set<std::pair<K, std::shared_ptr<V>>> elems;
public:
    /// @brief Global manager object
    static Manager<K, V>* singleton;

    /// @brief Return the singleton
    inline static Manager<K, V>& get_instance() {
        return *singleton;
    }

    /// @brief Load an element, this is the function that must be defined by the inheritor
    virtual std::shared_ptr<V> load(const K& key) {}

    /// @brief Obtain an element or construct a new one from a provided
    /// construct which accepts key
    virtual const T& get(const K& key) {
        auto it = elems.find(name);
        if(it != elems.end()) return it->second;
        std::shared_ptr<V> new_elem;
        new_elem = elems[key] = load(key);
        return new_elem;
    }
};
