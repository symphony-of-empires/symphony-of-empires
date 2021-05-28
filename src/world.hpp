#ifndef WORLD_H
#define WORLD_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include "nation.hpp"
#include "economy.hpp"
#include "lua.hpp"

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
class OrderGoods {
public:
	// How many we are willing to pay for the goods to be sent to us
	float payment;

	// The ID of the required product
	GoodId good_id;

	// Quantity of desired goods
	size_t quantity;

	// ID of the industry (inside the province) who requested this good
	IndustryId requester_industry_id;

	// ID of the province where the industry (who requested this) is located in
	ProvinceId requester_province_id;
};

/**
 * Represents a delivery, 
 */
class DeliverGoods {
public:
	// How many we are willing to pay to deliver this
	float payment;

	// ID of the good we are sending
	GoodId good_id;

	// Quantity available to send
	size_t quantity;

	// Product ID of the product to be sent
	ProductId product_id;

	// ID of the industry (inside the province) who is sending this product
	IndustryId sender_industry_id;

	// ID of the province where the industry (who is sending this) is located in
	ProvinceId sender_province_id;
};

/**
 * (UNUSED) A commercial convoy that goes from A to B to transport products
 */
class CommercialConvoy {
public:
	// Path that this convoy should follow to reach it's destination
	std::vector<Tile *> path;

	// Pointer to the (destination) order this convoy is serving
	OrderGoods * order;

	// Pointer to the (source) deliver this convoy is serving
	DeliverGoods * deliver;
};

#include <algorithm>
#include "event.hpp"

/**
 * Contains the main world class object, containing all the data relevant for the simulation
 */
class World {
	// 2-Dimensional Array of tiles
	Tile * tiles;
public:
	World();
	World& operator=(const World&) = default;
	~World();

	// Function that "simulates" the world by an entire tick
	void do_tick();

	void add_good(Good * good);
	void add_industry_type(IndustryType * it);
	void add_nation(Nation * nation);
	void add_province(Province * province);

	// Level at which sea dissapears, all sea is capped to sea_level - 1, and rivers are at sea_level.
	// Anything above is considered land
	size_t sea_level;

	// The height and width of the world
	size_t width, height;

	// List of units present in the world
	std::vector<Unit *> units;

	// List of goods (product types)
	std::vector<Good *> goods;

	// List of industry types where industries can be created from
	std::vector<IndustryType *> industry_types;

	// List of nations
	std::vector<Nation *> nations;

	// List of provinces
	std::vector<Province *> provinces;

	// List of companies
	std::vector<Company *> companies;

	// List of products
	std::vector<Product *> products;

	// List of POPs types
	std::vector<PopType *> pop_types;

	// List of religions
	std::vector<Religion *> religions;

	// List of cultures
	std::vector<Culture *> cultures;

	// List of unit types where units can be created from
	std::vector<UnitType *> unit_types;

	// List of registered events
	std::vector<Event *> events;

	// Lua state - for lua scripts
	lua_State * lua;

	// A list of orders (what factories want to be sent to them)
	std::vector<OrderGoods> orders;

	// A deliver list (what factories need to send)
	std::vector<DeliverGoods> delivers;

	// List of convoys on the world (unused)
	std::vector<CommercialConvoy> convoys;

	// Current time (in ticks)
	uint64_t time;

	// Array containing a list of tile pointers that have changed owners
	std::vector<Tile *> nation_changed_tiles;

	// Array containing a list of tile pointers that have changed elevation
	std::vector<Tile *> elevation_changed_tiles;

	/**
	 * Template for obtaining the ID of an element
	 * @tparam S return index type
	 * @tparam T type of the element to lookup
	 * @tparam C STL-compatible container where the pointer *should* be located in
	 */
	template<typename S, typename T, typename C>
	S get_id(T * ptr, C table) const {
		typename C::iterator it = std::find(table.begin(), table.end(), ptr);
		if(it == table.end()) {
			throw "Element not found";
		}
		return (S)std::distance(table.begin(), it);
	}

	size_t get_id(const Tile * ptr) const {
		return ptr_to_index(tiles, ptr);
	}

	// Obtains a tile from the world safely, and makes sure that it is in bounds
	Tile& get_tile(size_t x, size_t y) const {
		if(x >= width) {
			throw "Tile X exceeds width";
		} else if(y >= height) {
			throw "Tile Y exceeds height";
		}

		return tiles[x + y * width];
	}

	Tile& get_tile(size_t idx) const {
		if(idx >= width * height) {
			throw "Tile index exceeds boundaries";
		}
		return tiles[idx];
	}
};

extern World * g_world;

#endif
