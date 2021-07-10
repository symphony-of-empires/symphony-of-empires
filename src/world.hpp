#ifndef WORLD_H
#define WORLD_H

#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <ctime>
#include "nation.hpp"
#include "economy.hpp"
#include "lua_api.hpp"

#define MAX_INFRA_LEVEL 			8
#define MAX_ELEVATION				255
#define RIVER_ELEVATION(a)			a + 1

/**
* A single tile unit, contains all needed information for tile-scale wars
* and other non-war stuff (like province ownership).
* This is the smallest territorial unit in the game and it cannot be divided (and it shouldn't)
 */
class Tile {
public:
	// ID of the nation who owns this tile
	NationId owner_id;

	// ID of the province where this tile belongs to
	ProvinceId province_id;

	// Elevation level of this tile (from 0 to 255), take in account that
	// the sea level of the world can change the meaning of this value drastically
	uint8_t elevation;

	// Level of infrastructure in this tile (from 0 to MAX_INFRA_LEVEL)
	uint8_t infra_level;
};

#include <string>
#include "province.hpp"
#include "unit.hpp"

/**
* Represents an order, something an industry wants and that should be
* fullfilled by transport companies
 */
enum OrderType {
	ORDER_INDUSTRIAL,
	ORDER_OUTPOST_BUILD,
	ORDER_UNIT_BUILD,
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

	union {
		// ID of the industry (inside the province) who requested this good
		Industry* industry;
		// It can also be a outpost who requests this
		Outpost* outpost;
	};

	// ID of the province where the industry (who requested this) is located in
	Province* province;
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
	Industry* industry;

	// ID of the province where the industry (who is sending this) is located in
	Province* province;
};

/**
* (UNUSED) A commercial convoy that goes from A to B to transport products
 */
class CommercialConvoy {
public:
	// Path that this convoy should follow to reach it's destination
	std::vector<Tile *> path;

	// Pointer to the (destination) order this convoy is serving
	OrderGoods* order;

	// Pointer to the (source) deliver this convoy is serving
	DeliverGoods* deliver;
};

#include <algorithm>
#include <mutex>
#include "event.hpp"

/**
* Contains the main world class object, containing all the data relevant for the simulation
 */
class World {
	// Lua state - for lua scripts, this is only used by the server and should not be
	// accesible to the client
	lua_State* lua;

	/**
	* Template for obtaining the ID of an element
	* @tparam S return index type
	* @tparam T type of the element to lookup
	* @tparam C STL-compatible container where the pointer *should* be located in
	 */
	template<typename S, typename T, typename C>
	inline S get_id_ptr(const T* ptr, C table) const {
		typename C::iterator it = std::find(table.begin(), table.end(), ptr);
		if(it == table.end()) {
			// -1 is used as an invalid index
			return (S)-1;
		}
		return (S)std::distance(table.begin(), it);
	}
public:
	World(bool empty);
	World& operator=(const World&) = default;
	~World();

	// Function that "simulates" the world by an entire tick
	void do_tick();

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

	// List of units present in the world
	std::vector<Unit*> units;
	mutable std::mutex units_mutex;

	// List of goods (product types)
	std::vector<Good*> goods;
	mutable std::mutex goods_mutex;

	// List of industry types where industries can be created from
	std::vector<IndustryType*> industry_types;
	mutable std::mutex industry_types_mutex;

	// List of nations
	std::vector<Nation*> nations;
	mutable std::mutex nations_mutex;

	// List of provinces
	std::vector<Province*> provinces;
	mutable std::mutex provinces_mutex;

	// List of companies
	std::vector<Company*> companies;
	mutable std::mutex companies_mutex;

	// List of products
	std::vector<Product*> products;
	mutable std::mutex products_mutex;

	// List of POPs types
	std::vector<PopType*> pop_types;
	mutable std::mutex pop_types_mutex;

	// List of religions
	std::vector<Religion*> religions;
	mutable std::mutex religions_mutex;

	// List of cultures
	std::vector<Culture*> cultures;
	mutable std::mutex cultures_mutex;

	// List of unit types where units can be created from
	std::vector<UnitType*> unit_types;
	mutable std::mutex unit_types_mutex;
	
	// List of naval unit types
	std::vector<BoatType*> boat_types;
	mutable std::mutex boat_types_mutex;

	// List of traits that can be given to units
	std::vector<UnitTrait*> unit_traits;
	mutable std::mutex unit_traits_mutex;

	// List of registered events
	std::vector<Event *> events;
	mutable std::mutex events_mutex;

	// A list of orders (what factories want to be sent to them)
	std::vector<OrderGoods> orders;
	mutable std::mutex orders_mutex;

	// A deliver list (what factories need to send)
	std::vector<DeliverGoods> delivers;
	mutable std::mutex delivers_mutex;

	// List of convoys on the world (unused)
	std::vector<CommercialConvoy> convoys;

	// Array containing a list of tile pointers that have changed owners
	std::vector<Tile*> nation_changed_tiles;
	mutable std::mutex nation_changed_tiles_mutex;

	// Array containing a list of tile pointers that have changed elevation
	std::vector<Tile*> elevation_changed_tiles;
	mutable std::mutex elevation_changed_tiles_mutex;

	// A list containing descisions taken by countries
	std::vector<Descision*> taken_descisions;
	mutable std::mutex taken_descisions_mutex;

	// Dinarray containing types of outposts
	std::vector<OutpostType*> outpost_types;
	mutable std::mutex outpost_types_mutex;

	// Dinarray containing all outposts
	std::vector<Outpost*> outposts;
	mutable std::mutex outposts_mutex;

	Nation::Id get_id(const Nation* ptr) const;
	Province::Id get_id(const Province* ptr) const;
	Product::Id get_id(const Product* ptr) const;
	Good::Id get_id(const Good* ptr) const;
	Culture::Id get_id(const Culture* ptr) const;
	Company::Id get_id(const Company* ptr) const;
	IndustryType::Id get_id(const IndustryType* ptr) const;
	Industry::Id get_id(const Province& province, const Industry* ptr) const;
	Event::Id get_id(const Event* ptr) const;
	UnitType::Id get_id(const UnitType* ptr) const;
	BoatType::Id get_id(const BoatType* ptr) const;
	UnitTrait::Id get_id(const UnitTrait* ptr) const;
	Unit::Id get_id(const Unit* ptr) const;
	OutpostType::Id get_id(const OutpostType* ptr) const;
	Outpost::Id get_id(const Outpost* ptr) const;
	
	size_t get_id(const Tile* ptr) const;
	
	// Obtains a tile from the world safely, and makes sure that it is in bounds
	Tile& get_tile(size_t x, size_t y) const;
	Tile& get_tile(size_t idx) const;
	
	// Used by client to update anything each tick (i.e a graph)
	void (*client_update)(void);
};

extern World* g_world;

#endif
