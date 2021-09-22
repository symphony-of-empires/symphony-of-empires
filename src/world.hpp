#ifndef WORLD_H
#define WORLD_H

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
#include "server/lua_api.hpp"

#define MAX_INFRA_LEVEL 			8
#define MAX_ELEVATION				255
#define RIVER_ELEVATION(a)			a + 1

typedef unsigned int	uint;
/**
* A single tile unit, contains all needed information for tile-scale wars
* and other non-war stuff (like province ownership).
* This is the smallest territorial unit in the game and it cannot be divided (and it shouldn't)
 */
class Tile {
public:
    // ID of the nation who owns this tile
    Nation::Id owner_id;

    // ID of the province where this tile belongs to
    Province::Id province_id;

    // Elevation level of this tile (from 0 to 255), take in account that
    // the sea level of the world can change the meaning of this value drastically
    uint8_t elevation;

    // Level of infrastructure in this tile (from 0 to MAX_INFRA_LEVEL)
    uint8_t infra_level;
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
};
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

/**
 * A job request
 */
class JobRequest {
public:
    size_t amount;
    Province* province;
    Pop* pop;
};

/**
 * Represents a delivery, 
 */
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

/**
* Contains the main world class object, containing all the data relevant for the simulation
 */
class World {
    // Template for obtaining the ID of an element from a vector
    // @tparam T type of the element to lookup
    // @tparam C STL-compatible container where the pointer *should* be located in
    template<typename T, typename C>
    inline typename T::Id get_id_from_pvector(const T* ptr, C table) const {
        typename C::iterator it = std::find(table.begin(), table.end(), ptr);
        if(it == table.end()) {
            // -1 is used as an invalid index
            return (typename T::Id)-1;
        }
        return (typename T::Id)std::distance(table.begin(), it);
    }
public:
    World();
    World& operator=(const World&) = default;
    ~World();

    static World& get_instance(void);

    // Function that "simulates" the world by an entire tick
    void do_tick(void);
    void load_mod(void);

    inline const std::vector<Nation*>& get_list(const Nation* ptr) const {
        return nations;
    };
    inline std::vector<Nation*>& get_list(const Nation* ptr) {
        return nations;
    };

    inline const std::vector<Province*>& get_list(const Province* ptr) const {
        return provinces;
    };
    inline std::vector<Province*>& get_list(const Province* ptr) {
        return provinces;
    };

    inline const std::vector<Product*>& get_list(const Product* ptr) const {
        return products;
    };
    inline std::vector<Product*>& get_list(const Product* ptr) {
        return products;
    };

    inline const std::vector<Good*>& get_list(const Good* ptr) const {
        return goods;
    };
    inline std::vector<Good*>& get_list(const Good* ptr) {
        return goods;
    };

    inline const std::vector<Culture*>& get_list(const Culture* ptr) const {
        return cultures;
    };
    inline std::vector<Culture*>& get_list(const Culture* ptr) {
        return cultures;
    };

    inline const std::vector<Company*>& get_list(const Company* ptr) const {
        return companies;
    };
    inline std::vector<Company*>& get_list(const Company* ptr) {
        return companies;
    };

    inline const std::vector<PopType*>& get_list(const PopType* ptr) const {
        return pop_types;
    };
    inline std::vector<PopType*>& get_list(const PopType* ptr) {
        return pop_types;
    };

    inline const std::vector<Building*>& get_list(const Building* ptr) const {
        return buildings;
    };
    inline std::vector<Building*>& get_list(const Building* ptr) {
        return buildings;
    };

    inline const std::vector<Event*>& get_list(const Event* ptr) const {
        return events;
    };
    inline std::vector<Event*>& get_list(const Event* ptr) {
        return events;
    };

    inline const std::vector<UnitType*>& get_list(const UnitType* ptr) const {
        return unit_types;
    };
    inline std::vector<UnitType*>& get_list(const UnitType* ptr) {
        return unit_types;
    };

    inline const std::vector<BoatType*>& get_list(const BoatType* ptr) const {
        return boat_types;
    };
    inline std::vector<BoatType*>& get_list(const BoatType* ptr) {
        return boat_types;
    };

    inline const std::vector<UnitTrait*>& get_list(const UnitTrait* ptr) const {
        return unit_traits;
    };
    inline std::vector<UnitTrait*>& get_list(const UnitTrait* ptr) {
        return unit_traits;
    };

    inline const std::vector<Unit*>& get_list(const Unit* ptr) const {
        return units;
    };
    inline std::vector<Unit*>& get_list(const Unit* ptr) {
        return units;
    };

    inline const std::vector<Boat*>& get_list(const Boat* ptr) const {
        return boats;
    };
    inline std::vector<Boat*>& get_list(const Boat* ptr) {
        return boats;
    };

    inline const std::vector<BuildingType*>& get_list(const BuildingType* ptr) const {
        return building_types;
    };
    inline std::vector<BuildingType*>& get_list(const BuildingType* ptr) {
        return building_types;
    };

    inline const std::vector<Treaty*>& get_list(const Treaty* ptr) const {
        return treaties;
    };
    inline std::vector<Treaty*>& get_list(const Treaty* ptr) {
        return treaties;
    };

    inline const std::vector<Ideology*>& get_list(const Ideology* ptr) const {
        return ideologies;
    };
    inline std::vector<Ideology*>& get_list(const Ideology* ptr) {
        return ideologies;
    };

    inline const std::vector<Religion*>& get_list(const Religion* ptr) const {
        return religions;
    };
    inline std::vector<Religion*>& get_list(const Religion* ptr) {
        return religions;
    };

    inline const std::vector<Technology*>& get_list(const Technology* ptr) const {
        return technologies;
    };
    inline std::vector<Technology*>& get_list(const Technology* ptr) {
        return technologies;
    };
    
    inline const std::vector<Invention*>& get_list(const Invention* ptr) const {
        return inventions;
    };
    inline std::vector<Invention*>& get_list(const Invention* ptr) {
        return inventions;
    };

    inline const std::vector<NationModifier*>& get_list(const NationModifier* ptr) const {
        return nation_modifiers;
    };
    inline std::vector<NationModifier*>& get_list(const NationModifier* ptr) {
        return nation_modifiers;
    };
    
    inline size_t get_id(const Tile* ptr) const {
        std::lock_guard<std::recursive_mutex> lock(tiles_mutex);
        return ((ptrdiff_t)ptr - (ptrdiff_t)tiles) / sizeof(Tile);
    };

    template<typename T>
    inline typename T::Id get_id(const T* ptr) const {
        return get_id_from_pvector<T>(ptr, get_list(ptr));
    };
    
    // Obtains a tile from the world safely, and makes sure that it is in bounds
    Tile& get_tile(size_t x, size_t y) const;
    Tile& get_tile(size_t idx) const;

    // Lua state - for lua scripts, this is only used by the server and should not be
    // accesible to the client
    lua_State* lua;

    // 2-Dimensional Array of tiles
    Tile* tiles;
    mutable std::recursive_mutex tiles_mutex;

    // Level at which sea dissapears, all sea is capped to sea_level - 1, and rivers are at sea_level.
    // Anything above is considered land
    size_t sea_level;

    // The height and width of the world
    size_t width, height;

    // Current time (in ticks)
    uint64_t time = 0;

    std::vector<Unit*> units;
    std::vector<Boat*> boats;
    std::vector<Good*> goods;
    std::vector<Nation*> nations;
    std::vector<Province*> provinces;
    std::vector<Company*> companies;
    std::vector<Product*> products;
    std::vector<PopType*> pop_types;
    std::vector<Religion*> religions;
    std::vector<Culture*> cultures;
    std::vector<UnitType*> unit_types;
    std::vector<BoatType*> boat_types;
    std::vector<UnitTrait*> unit_traits;
    std::vector<Event*> events;
    std::vector<Technology*> technologies;
    std::vector<Invention*> inventions;
    std::vector<NationModifier*> nation_modifiers;
    mutable std::recursive_mutex world_mutex;

    // A list of orders (what factories want to be sent to them)
    std::vector<OrderGoods> orders;
    mutable std::recursive_mutex orders_mutex;

    // A deliver list (what factories need to send)
    std::vector<DeliverGoods> delivers;
    mutable std::recursive_mutex delivers_mutex;

    // A list of jobs required
    std::vector<JobRequest> job_requests;
    mutable std::recursive_mutex job_requests_mutex;

    // Array containing a list of tile coord that have changed owners
    std::vector<std::pair<uint, uint>> changed_tile_coords;
    mutable std::recursive_mutex changed_tiles_coords_mutex;

    // Array containing a list of tile pointers that have changed owners
    std::vector<Tile*> nation_changed_tiles;
    mutable std::recursive_mutex nation_changed_tiles_mutex;

    // Array containing a list of tile pointers that have changed elevation
    std::vector<Tile*> elevation_changed_tiles;
    mutable std::recursive_mutex elevation_changed_tiles_mutex;

    // A list containing descisions taken by countries
    std::vector<Descision*> taken_descisions;
    std::vector<BuildingType*> building_types;
    std::vector<Building*> buildings;
    std::vector<Treaty*> treaties;
    std::vector<Ideology*> ideologies;
};

extern World* g_world;

#endif
