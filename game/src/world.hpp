// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
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
//      world.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <mutex>
#include <string>
#include <memory>
#include <array>

#include <glm/vec2.hpp>

#include "eng3d/profiler.hpp"
#include "eng3d/string.hpp"
#include "eng3d/log.hpp"

#include "nation.hpp"
#include "product.hpp"
#include "good.hpp"
#include "technology.hpp"
#include "building.hpp"
#include "ideology.hpp"
#include "terrain.hpp"
#include "server/lua_api.hpp"
#include "province.hpp"
#include "unit.hpp"
#include "technology.hpp"
#include "event.hpp"
#include "diplomacy.hpp"
#include "culture.hpp"
#include "religion.hpp"
#include "pop.hpp"

// A single tile unit this is the smallest territorial unit in the game and it cannot be divided (and it shouldn't)
class World;
class Tile {
public:
    std::vector<Tile> get_neighbours(const World& world) const;
    // ID of the province where this tile belongs to
    Province::Id province_id;
};

/// @todo Make bookmark.hpp?
class Bookmark {
public:
    Eng3D::StringRef ref_name;
    Eng3D::StringRef name;
    uint64_t start_time;
    std::vector<Nation*> interest_picks;
};

// Create a new list from a type, with helper functions
#define LIST_FOR_TYPE(type, list, list_type)\
    inline const list_type<type*>& get_list(const type* = nullptr) const {\
        return list;\
    };\
    inline list_type<type*>& get_list(const type* = nullptr) {\
        return list;\
    };\
    list_type<type*> list;

// Vectors we don't remove from
#define CONST_LIST_FOR_LOCAL_TYPE(type, list, list_type)\
    inline const list_type<type>& get_list(const type* = nullptr) const {\
        return list;\
    };\
    inline list_type<type>& get_list(const type* = nullptr) {\
        return list;\
    };\
    inline void insert(type& ptr) {\
        auto& list = this->get_list((type*)nullptr);\
        list_mutex.lock();\
        ptr.cached_id = list.size();\
        Eng3D::Log::debug("world_insert", "Inserting object " #type " with ID=%zu" + std::to_string(ptr.cached_id));\
        list.push_back(ptr);\
        list_mutex.unlock();\
    };\
    list_type<type> list;

#define LIST_FOR_LOCAL_TYPE(type, list, list_type)\
    inline const list_type<type>& get_list(const type* = nullptr) const {\
        return list;\
    };\
    inline list_type<type>& get_list(const type* = nullptr) {\
        return list;\
    };\
    inline void insert(type& ptr) {\
        auto& list = this->get_list((type*)nullptr);\
        list_mutex.lock();\
        ptr.cached_id = list.size();\
        Eng3D::Log::debug("world_insert", "Inserting object " #type " with ID=%zu" + std::to_string(ptr.cached_id));\
        list.push_back(ptr);\
        list_mutex.unlock();\
    };\
    inline void remove(type& ptr) {\
        type::Id cached_id = this->get_id<type>(ptr);\
        auto& list = this->get_list((type*)nullptr);\
        list_mutex.lock();\
        cached_id = list.size();\
        Eng3D::Log::debug("world_remove", "Removing object " #type " with ID=" + std::to_string(cached_id));\
        for(type::Id i = cached_id + 1; i < list.size(); i++) {\
            list[i].cached_id--;\
        }\
        list.erase(list.begin() + cached_id);\
        list_mutex.unlock();\
    };\
    list_type<type> list;

// Contains the main world class object, containing all the data relevant for the simulation
class World {
public:
    static constexpr unsigned int ticks_per_month = 30;

    World();
    World& operator=(const World&) = default;
    ~World();
    static World& get_instance();
    void do_tick();
    void load_initial();
    void load_mod();
    Eng3D::Profiler profiler;

    LIST_FOR_LOCAL_TYPE(Good, goods, std::vector);
    LIST_FOR_LOCAL_TYPE(Culture, cultures, std::vector);
    LIST_FOR_LOCAL_TYPE(PopType, pop_types, std::vector);
    LIST_FOR_LOCAL_TYPE(UnitType, unit_types, std::vector);
    LIST_FOR_LOCAL_TYPE(BuildingType, building_types, std::vector);
    LIST_FOR_LOCAL_TYPE(Ideology, ideologies, std::vector);
    LIST_FOR_LOCAL_TYPE(Religion, religions, std::vector);
    LIST_FOR_LOCAL_TYPE(Technology, technologies, std::vector);
    LIST_FOR_LOCAL_TYPE(NationModifier, nation_modifiers, std::vector);
    LIST_FOR_LOCAL_TYPE(TerrainType, terrain_types, std::vector);
    CONST_LIST_FOR_LOCAL_TYPE(Province, provinces, std::vector);
    CONST_LIST_FOR_LOCAL_TYPE(Nation, nations, std::vector);
    LIST_FOR_LOCAL_TYPE(Event, events, std::vector);

    LIST_FOR_TYPE(Treaty, treaties, std::vector);
    LIST_FOR_TYPE(War, wars, std::vector);
    UnitManager unit_manager;
    ProvinceOwnershipManager province_manager;

    template<typename T>
    inline void insert(T& ptr) {
        auto& list = this->get_list((T*)nullptr);

        list_mutex.lock();
        ptr.cached_id = list.size();
        assert(ptr.cached_id < (typename T::Id)-2);
        list.push_back((T*)&ptr);
        list_mutex.unlock();
    };

    template<typename T>
    inline void remove(T& ptr) {
        // Decrease the cache_id counter for the elements after the removed element
        // so we don't lose sync of ids
        typename T::Id cached_id = this->get_id<T>(ptr);
        auto& list = this->get_list((T*)nullptr);

        list_mutex.lock();
        for(typename T::Id i = cached_id + 1; i < list.size(); i++) {
            list[i]->cached_id--;
        }

        // Remove the element itself
        list.erase(list.begin() + cached_id);
        list_mutex.unlock();
    }

    /// @brief Get the id of a tile
    /// @param obj Tile to get id from
    /// @return int The Id of the tile
    inline int get_id(const Tile& obj) const {
        return static_cast<int>(((ptrdiff_t)&obj - (ptrdiff_t)tiles.get()) / sizeof(Tile));
    }

    /// @brief Get the id of an object, this is a template for all types except
    /// for tiles and locally-stored types (we can do this because we can obtain the list
    /// from the type) with get_list helper functions. Please do not store the value of this
    /// function on a local variable because said variable could potentially get invalidated!
    /// @tparam T Type of object
    /// @param obj Reference to the object
    /// @return T::Id Id of the object
    template<typename T>
    inline typename T::Id get_id(const T& obj) const {
        return obj.cached_id;
    }

    /// @brief Get the dist from the equator in respect to Y
    /// @param y Position to get distance from
    /// @return float 
    inline float get_dist_from_equator(float y) const {
        return std::fabs(std::fabs(y) - (this->width / 2.0));
    }

    /// @brief Obtains a tile from the world safely, and makes sure that it is in bounds
    /// @param idx Index of the tile
    /// @return Tile& Returned tile
    inline Tile& get_tile(size_t idx) const {
        assert(idx < width * height); // Tile index exceeds boundaries
        return tiles[idx];
    }

    inline Tile& get_tile(size_t x, size_t y) const {
        assert(x < this->width && y < this->height); // Tile out of bounds
        return this->get_tile(x + y * this->width);
    }

    std::unique_ptr<NationRelation[]> relations;
    // Uses cantor's pairing function
    // https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
    inline NationRelation& get_relation(int a, int b) {
        assert(a != b);
        if(b > a) {
            int tmp;
            tmp = b;
            b = a;
            a = tmp;
        }
        return relations[(a * a - a) / 2 + b];
    }

    inline const NationRelation& get_relation(int a, int b) const {
        assert(a != b);
        if(b > a) {
            int tmp;
            tmp = b;
            b = a;
            a = tmp;
        }
        return relations[(a * a - a) / 2 + b];
    }

    // Lua state - for lua scripts, this is only used by the server and should not be
    // accesible to the client
    lua_State* lua;

    // 2-Dimensional Array of tiles
    std::unique_ptr<Tile[]> tiles;
    mutable std::mutex tiles_mutex;

    // The height and width of the world
    size_t width, height;

    // Current time (in ticks)
    int time;

    bool needs_to_sync = false;

    std::mutex world_mutex;
    std::mutex list_mutex;
    std::mutex wcmap_mutex; // World map on clients require some data to be stable for read-only operations
    std::vector<std::pair<Decision, Nation*>> taken_decisions;
};

extern World* g_world;