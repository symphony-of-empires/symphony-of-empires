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
//      manager.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <stdexcept>
#include <set>

// Generic manager for any resource type, the manager will call "load" if an element
// with the same ident already exists
template<typename T, typename I, typename L = std::set<std::pair<T*, I>>>
class Manager {
private:
    L elems;
public:
    // Global manager object
    static Manager<T, I>* singleton;

    // Singleton pattern ;)
    static Manager<T, I>& get_instance(void) {
        return *singleton;
    };

    // Load an element, this is the function that must be defined by the inheritor
    virtual void load(const I& ident) {};

    // Obtain an element or construct a new one from a provided
    // construct which accepts ident
    virtual const T& get(const I& ident) {
        // TODO: How do we remove this duplicate code? :/
        for(const auto& o : elems) {
            if(o.second == ident) {
                return *(o.first);
            }
        }

        load(ident);

        for(const auto& o : elems) {
            if(o.second == ident) {
                return *(o.first);
            }
        }
        throw std::runtime_error("Can't load " + ident);
    };
};