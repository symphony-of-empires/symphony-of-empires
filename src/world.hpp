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
#include "company.hpp"
#include "ideology.hpp"
#include "terrain.hpp"
#include "server/lua_api.hpp"
#include <glm/vec2.hpp>

#define MAX_ELEVATION				255
#define RIVER_ELEVATION(a)			a + 1

typedef unsigned int	uint;
/**
* A single tile unit, contains all needed information for tile-scale wars
* and other non-war stuff (like province ownership).
* This is the smallest territorial unit in the game and it cannot be divided (and it shouldn't)
 */
class World;
struct Tile {
public:
    // ID of the province where this tile belongs to
    Province::Id province_id;
    const std::vector<const Tile*> get_neighbours(const World& world) const;
};

#include <string>
#include "province.hpp"
#include "unit.hpp"
#include "technology.hpp"

/**
* Represents an order, something an industry wants and that should be
* fullfilled by transport companies
 */
enum class OrderType {
    INDUSTRIAL,
    BUILDING,
    UNIT,
    POP,
};
struct OrderGoods {
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

/**
 * A job request
 */
struct JobRequest {
public:
    size_t amount;
    Province* province;
    Pop pop;
};

/**
 * Represents a delivery,
 */
struct DeliverGoods {
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
    inline const list_type<type*>& get_list(const type* ptr) const {\
        return list;\
    };\
    inline list_type<type*>& get_list(const type* ptr) {\
        return list;\
    };\
    list_type<type*> list;

/**
* Contains the main world class object, containing all the data relevant for the simulation
 */
class World {
    std::vector<Event*> daily_check_events;
    std::vector<Event*> monthly_check_events;
    std::vector<Event*> yearly_check_events;
    std::vector<Event*> economical_check_events;
    std::vector<std::pair<Event*, int>> rand_chance_events;

public:
    static constexpr unsigned int ticks_per_day = 48;

    World();
    World& operator=(const World&) = default;
    ~World();

    static World& get_instance(void);

    // Function that "simulates" the world by an entire tick
    void do_tick(void);
    void load_mod(void);

    LIST_FOR_TYPE(Nation, nations, std::vector);
    LIST_FOR_TYPE(Province, provinces, std::vector);
    LIST_FOR_TYPE(Product, products, std::vector);
    LIST_FOR_TYPE(Good, goods, std::vector);
    LIST_FOR_TYPE(Culture, cultures, std::vector);
    LIST_FOR_TYPE(Company, companies, std::vector);
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
        auto& list = this->get_list(ptr);

        // Decrease the cache_id counter for the elements after the removed element
        typename T::Id cached_id = this->get_id<T>(ptr);
        for(typename T::Id i = cached_id; i < list.size(); i++) {
            list[i]->cached_id -= 1;
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