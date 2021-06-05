#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

/**
 * Defines a type of unit, it can be a tank, garrison, infantry, etc
 * this is moddable via a lua script and new unit types can be added
 */
class UnitType {
public:
	std::string name;
	std::string ref_name;
	
	float supply_consumption;
	float speed;
	float max_health;
	float defense;
	float attack;
};

/**
 * Defines the type of a naval unit
 */
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
	uint8_t type_id;

	// Size of the unit (soldiers in unit)
	size_t size;

	// Target coordinates of this unit
	size_t tx;
	size_t ty;

	// Current position
	float x;
	float y;

	// Who owns this unit
	NationId owner_id;

	// Used to "ignore" an unit when doing any check, this allows other units to
	// attack this unit
	bool ignore_tag;
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
	NationId owner_id;

	// Unit that is currently being built here (nullptr indicates no unit)
	Unit * working_unit;

	// Remaining ticks until the unit is built
	uint16_t build_time;
};

#endif
