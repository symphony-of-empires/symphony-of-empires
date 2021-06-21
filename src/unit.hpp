#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include "economy.hpp"

/**
* Defines a type of unit, it can be a tank, garrison, infantry, etc
* this is moddable via a lua script and new unit types can be added
 */
typedef uint8_t UnitTypeId;
class UnitType {
public:
	std::string name;
	std::string ref_name;
	
	float supply_consumption;
	float speed;
	float max_health;
	float defense;
	float attack;

	// Required goods, first describes the id of the good and the second describes how many
	std::vector<std::pair<Good *, size_t>> req_goods;
};

/**
* Defines the type of a naval unit
 */
typedef uint8_t BoatTypeId;
class BoatType {
public:
	std::string name;
	std::string ref_name;
	
	float speed;
	float max_health;
	float defense;
	float attack;
	
	// Capacity of stuff (units) that can be carried here
	size_t capacity;

	std::vector<std::pair<Good *, size_t>> req_goods;
};

#include "nation.hpp"
#include "world.hpp"
class Tile;
/**
* Roughly a batallion, consisting of approximately 500 soldiers each
 */
class Unit {
public:
	// Type of unit
	UnitType* type;

	// Size of the unit (soldiers in unit)
	size_t size;

	// Target coordinates of this unit
	size_t tx;
	size_t ty;

	// Current position
	float x;
	float y;

	// Who owns this unit
	Nation* owner;

	// Used to "ignore" an unit when doing any check, this allows other units to
	// attack this unit
	bool ignore_tag;

	std::vector<std::pair<Good *, size_t>> req_goods;
};

/**
* A military outpost, on land serves as a "spawn" place for units
* When adjacent to a water tile this serves as a shipyard for spawning naval units
 */
class Outpost {
public:
	// Position of outpost
	size_t x;
	size_t y;

	// Determines if this outpost is a shipyard or if it's barracks
	bool is_naval;

	// Owner of the outpost
	Nation* owner;

	// Unit that is currently being built here (nullptr indicates no unit)
	Unit* working_unit;

	// Remaining ticks until the unit is built
	uint16_t build_time;

	std::vector<std::pair<Good *, size_t>> req_goods;
};

#endif
