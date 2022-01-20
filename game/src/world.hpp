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
#include <ctime>
#include "nation.hpp"
#include "product.hpp"
#include "good.hpp"
#include "technology.hpp"
#include "building.hpp"
#include "ideology.hpp"
#include "terrain.hpp"
#include "server/lua_api.hpp"
#include <glm/vec2.hpp>
#include "unified_render/profiler.hpp"

#define MAX_ELEVATION				255
#define RIVER_ELEVATION(a)			a + 1

typedef unsigned int	uint;
// A single tile unit this is the smallest territorial unit in the game and it cannot be divided (and it shouldn't)
class World;
class Tile {
public:
    const std::vector<const Tile*> get_neighbours(const World& world) const;

    // ID of the province where this tile belongs to
    Province::Id province_id;
};

#include <string>
#include "province.hpp"
#include "unit.hpp"
#include "technology.hpp"

enum class OrderType {
    INDUSTRIAL,
    BUILDING,
    UNIT,
    POP,
};

// Represents an order, something an industry wants and that should be
// fullfilled by transport tick
class OrderGoods {
public:
    enum OrderType type;

    // How many we are willing to pay for the goods to be sent to us
    float payment;

    // The ID of the required product
    Good* good;

    // Quantity of desired goods
    size_t quantity;

    // A building who requests materials
    Building* building;

    // ID of the province where the industry (who requested this) is located in
    Province* province;
};

// A job request
class JobRequest {
public:
    size_t amount;
    Province* province;
    Pop pop;
};

// Represents a delivery,
class DeliverGoods {
public:
    // How many we are willing to pay to deliver this
    float payment;

    // ID of the good we are sending
    Good* good;

    // Quantity available to send
    size_t quantity;

    // Product ID of the product to be sent
    Product* product;

    // ID of the industry (inside the province) who is sending this product
    Building* building;

    // ID of the province where the industry (who is sending this) is located in
    Province* province;
};

#include <algorithm>
#include <mutex>
#include "event.hpp"
#include "diplomacy.hpp"

// Create a new list from a type, with helper functions
#define LIST_FOR_TYPE(type, list, list_type)\
    inline const list_type<type*>& get_list(const type*) const {\
        return list;\
    };\
    inline list_type<type*>& get_list(const type*) {\
        return list;\
    };\
    list_type<type*> list;

// Contains the main world class object, containing all the data relevant for the simulation
class World {
    std::vector<Event*> daily_check_events;
    std::vector<Event*> monthly_check_events;
    std::vector<Event*> yearly_check_events;
    std::vector<Event*> economical_check_events;
    std::vector<std::pair<Event*, int>> rand_chance_events;

public:
    static constexpr unsigned int ticks_per_month = 30;

    World();
    World& operator=(const World&) = default;
    ~World();
    static World& get_instance(void);
    void do_tick(void);
    void load_initial(void);
    void load_mod(void);
    UnifiedRender::Profiler profiler;

    LIST_FOR_TYPE(Nation, nations, std::vector);
    LIST_FOR_TYPE(Province, provinces, std::vector);
    LIST_FOR_TYPE(Product, products, std::vector);
    LIST_FOR_TYPE(Good, goods, std::vector);
    LIST_FOR_TYPE(Culture, cultures, std::vector);
    LIST_FOR_TYPE(PopType, pop_types, std::vector);
    LIST_FOR_TYPE(Building, buildings, std::vector);
    LIST_FOR_TYPE(Event, events, std::vector);
    LIST_FOR_TYPE(UnitType, unit_types, std::vector);
    LIST_FOR_TYPE(UnitTrait, unit_traits, std::vector);
    LIST_FOR_TYPE(Unit, units, std::vector);
    LIST_FOR_TYPE(BuildingType, building_types, std::vector);
    LIST_FOR_TYPE(Treaty, treaties, std::vector);
    LIST_FOR_TYPE(Ideology, ideologies, std::vector);
    LIST_FOR_TYPE(Religion, religions, std::vector);
    LIST_FOR_TYPE(Technology, technologies, std::vector);
    LIST_FOR_TYPE(NationModifier, nation_modifiers, std::vector);
    LIST_FOR_TYPE(TerrainType, terrain_types, std::vector);
    LIST_FOR_TYPE(War, wars, std::vector);

    template<typename T>
    inline void insert(T* ptr) {
        auto& list = this->get_list(ptr);
        ptr->cached_id = list.size();
        list.push_back(ptr);
    };

    template<typename T>
    inline void remove(T* ptr) {
        // Decrease the cache_id counter for the elements after the removed element
        typename T::Id cached_id = this->get_id<T>(ptr);
        auto& list = this->get_list(ptr);
        for(typename T::Id i = cached_id + 1; i < list.size(); i++) {
            list[i]->cached_id--;
        }

        // Remove the element itself
        list.erase(list.begin() + cached_id);
    };

    inline size_t get_id(const Tile* ptr) const {
        return ((ptrdiff_t)ptr - (ptrdiff_t)tiles) / sizeof(Tile);
    };

    // Template for all types except for tiles (we can do this because we can
    // obtain the list from the type) with get_list helper functions
    template<typename T>
    inline typename T::Id get_id(const T* ptr) const {
        return ptr->cached_id;
    };

    template<typename T>
    inline float get_dist_from_equator(T x) const {
        return std::fabs(std::fabs(x) - (width / 2.0));
    };

    // Obtains a tile from the world safely, and makes sure that it is in bounds
    Tile& get_tile(size_t x, size_t y) const;
    Tile& get_tile(size_t idx) const;

    // Lua state - for lua scripts, this is only used by the server and should not be
    // accesible to the client
    lua_State* lua;

    // 2-Dimensional Array of tiles
    Tile* tiles;
    mutable std::mutex tiles_mutex;

    // Level at which sea dissapears, all sea is capped to sea_level - 1, and rivers are at sea_level.
    // Anything above is considered land
    size_t sea_level;

    // The height and width of the world
    size_t width, height;

    // Current time (in ticks)
    uint64_t time = 0;

    bool needs_to_sync = false;

    mutable std::mutex world_mutex;

    // A list of orders (what factories want to be sent to them)
    std::vector<OrderGoods> orders;
    mutable std::mutex orders_mutex;

    // A deliver list (what factories need to send)
    std::vector<DeliverGoods> delivers;
    mutable std::mutex delivers_mutex;
    
    std::vector<std::pair<Descision*, Nation*>> taken_descisions;
};

extern World* g_world;