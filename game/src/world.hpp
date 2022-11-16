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
#include "eng3d/luavm.hpp"

#include "nation.hpp"
#include "product.hpp"
#include "technology.hpp"
#include "building.hpp"
#include "ideology.hpp"
#include "terrain.hpp"
#include "server/lua_api.hpp"
#include "server/economy.hpp"
#include "province.hpp"
#include "unit.hpp"
#include "technology.hpp"
#include "event.hpp"
#include "diplomacy.hpp"
#include "pop.hpp"

// A single tile unit this is the smallest territorial unit in the game and it cannot be divided (and it shouldn't)
struct World;

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
        auto& type_list = this->get_list((type*)nullptr);\
        std::scoped_lock lock(list_mutex);\
        ptr.cached_id = type::Id(type_list.size());\
        Eng3D::Log::debug("world_insert", "Inserting object " #type " with ID=" + std::to_string(static_cast<size_t>(ptr.cached_id)));\
        type_list.push_back(ptr);\
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
        auto& type_list = this->get_list((type*)nullptr);\
        std::scoped_lock lock(list_mutex);\
        ptr.cached_id = type::Id(type_list.size());\
        Eng3D::Log::debug("world_insert", "Inserting object " #type " with ID=" + std::to_string(static_cast<size_t>(ptr.cached_id)));\
        type_list.push_back(ptr);\
    };\
    inline void remove(type& ptr) {\
        size_t cached_id = static_cast<size_t>(this->get_id<type>(ptr));\
        auto& type_list = this->get_list((type*)nullptr);\
        std::scoped_lock lock(list_mutex);\
        cached_id = type_list.size();\
        Eng3D::Log::debug("world_remove", "Removing object " #type " with ID=" + std::to_string(static_cast<size_t>(cached_id)));\
        for(size_t i = cached_id + 1; i < type_list.size(); i++)\
            type_list[i].cached_id = type::Id(static_cast<size_t>(type_list[i].cached_id) - 1);\
        type_list.erase(type_list.begin() + cached_id);\
    };\
    list_type<type> list;

// Contains the main world class object, containing all the data relevant for the simulation
class World {
public:
    constexpr static unsigned int ticks_per_month = 30;

    World() = default;
    World& operator=(const World&) = delete;
    ~World() = default;
    static World& get_instance() {
        extern World g_world;
        return g_world;
    }
    void do_tick();
    void init_lua();
    void load_initial();
    void load_mod();
    Eng3D::Profiler profiler;

    LIST_FOR_LOCAL_TYPE(Commodity, commodities, std::vector)
    LIST_FOR_LOCAL_TYPE(Language, languages, std::vector)
    LIST_FOR_LOCAL_TYPE(PopType, pop_types, std::vector)
    LIST_FOR_LOCAL_TYPE(UnitType, unit_types, std::vector)
    LIST_FOR_LOCAL_TYPE(BuildingType, building_types, std::vector)
    LIST_FOR_LOCAL_TYPE(Ideology, ideologies, std::vector)
    LIST_FOR_LOCAL_TYPE(Religion, religions, std::vector)
    LIST_FOR_LOCAL_TYPE(Technology, technologies, std::vector)
    LIST_FOR_LOCAL_TYPE(TerrainType, terrain_types, std::vector)
    CONST_LIST_FOR_LOCAL_TYPE(Province, provinces, std::vector)
    CONST_LIST_FOR_LOCAL_TYPE(Nation, nations, std::vector)
    LIST_FOR_LOCAL_TYPE(Event, events, std::vector)
    LIST_FOR_LOCAL_TYPE(Treaty, treaties, std::vector)
    LIST_FOR_LOCAL_TYPE(War, wars, std::vector)
    UnitManager unit_manager;
    ProvinceManager province_manager;
    Economy::EconomyState economy_state;

    template<typename T>
    inline void insert(T& ptr) {
        auto& list = this->get_list((T*)nullptr);
        list_mutex.lock();
        ptr.cached_id = list.size();
        assert(ptr.cached_id < static_cast<typename T::Id>(-2));
        list.push_back((T*)&ptr);
        list_mutex.unlock();
    }

    template<typename T>
    inline void remove(T& ptr) {
        size_t cached_id = static_cast<size_t>(this->get_id<T>(ptr));
        auto& list = this->get_list((T*)nullptr);
        list_mutex.lock();
        for(size_t i = cached_id + 1; i < list.size(); i++)
            list[i].cached_id = T::Id(static_cast<size_t>(list[i].cached_id) - 1);
        // Remove the element itself
        list.erase(list.begin() + cached_id);
        list_mutex.unlock();
    }

    /// @brief Get the id of an object, this is a template for all types except
    /// for tiles and locally-stored types (we can do this because we can obtain the list
    /// from the type) with get_list helper functions. Please do not store the value of this
    /// function on a local variable because said variable could potentially get invalidated!
    /// @tparam T Type of object
    /// @param obj Reference to the object
    /// @return TId Id of the object
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
    
    std::vector<Nation::Relation> relations;

    Nation::Relation& get_relation(NationId a, NationId b) {
        assert(a != b);
        if(b > a) std::swap(a, b);
        return relations[a + b * nations.size()];
    }

    const Nation::Relation& get_relation(NationId a, NationId b) const {
        assert(a != b);
        if(b > a) std::swap(a, b);
        return relations[a + b * nations.size()];
    }

    int get_year(void) const {
        return this->time / this->ticks_per_month / 12;
    }

    int get_month(void) const {
        return (this->time / this->ticks_per_month) % 12;
    }

    int get_day(void) const {
        return this->time % this->ticks_per_month;
    }

    Eng3D::LuaVM lua;

    // 2D Array of tiles
    std::unique_ptr<ProvinceId[]> tiles;
    size_t width, height;
    int time;
    
    /// @brief Used to signal the lua scripts of invalid operations (eg. adding a country midgame)
    bool needs_to_sync = false;
    std::mutex world_mutex;
    std::mutex list_mutex;
    std::mutex inbox_mutex;
    std::vector<std::pair<Decision, NationId>> taken_decisions;
};
template<>
struct Eng3D::Deser::Serializer<World> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, World>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.width);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.height);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.time);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.commodities);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.unit_types);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.religions);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.languages);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.pop_types);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.terrain_types);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.building_types);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ideologies);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.technologies);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.nations);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.provinces);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.events);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.wars);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.treaties);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.unit_manager);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.relations);

        // Savefiles do not contain the tiles
        /// @todo Handle dynamic tiles (provinces changing shape for ex.)
        bool has_tiles = obj.tiles.get() != nullptr;
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, has_tiles);
        if(has_tiles) {
            if constexpr(is_serialize) {
                // Serialize all tiles
                ar.expand(obj.width * obj.height * sizeof(ProvinceId));
                ar.copy_from(obj.tiles.get(), obj.width * obj.height * sizeof(ProvinceId));
            } else {
                // In order to avoid post-deserialization relational patcher, we will simply allocate everything with "empty" objects,
                // then we will fill those spots as we deserialize
                obj.tiles.reset(new ProvinceId[obj.width * obj.height]);
                // Deserialize all tiles
                ar.copy_to(obj.tiles.get(), obj.width * obj.height * sizeof(ProvinceId));
            }
        }
    }
};

#include "eng3d/serializer.hpp"
/// @brief Used as a template for serializable objects on the global world context
template<typename T>
concept SerializerPointer = std::is_pointer_v<T>;
template<SerializerPointer T>
struct Eng3D::Deser::Serializer<T> {
    typedef std::remove_pointer_t<T> type_no_p; // Pointerless type
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, type_no_p>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>*const& obj) {
        typename type_no_p::Id id = !is_serialize ? type_no_p::invalid() : obj->get_id();
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, id);
        if constexpr(!is_serialize) {
            if(static_cast<size_t>(id) >= World::get_instance().get_list((T)nullptr).size()) {
                const_cast<T&>(obj) = nullptr;
            } else {
                const_cast<T&>(obj) = id != type_no_p::invalid() ? &(World::get_instance().get_list((T)nullptr)[id]) : nullptr;
            }
        }
    }
};

extern World g_world;
