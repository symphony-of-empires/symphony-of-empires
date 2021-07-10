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
	using Id = UnitTypeId;
	std::string name;
	std::string ref_name;
	
	float supply_consumption;
	float speed;
	float max_health;
	float defense;
	float attack;

	// Max ticks allowed for defensive bonus - this basically prevents an unit from staying 200
	// years on the same spot and be fucking indestructible
	uint64_t max_defensive_ticks;

	// The defense provided by the unit each tick for staying on the same position
	// aka. digging trenches
	float position_defense;

	// Time needed to build
	size_t build_time;

	// Required goods, first describes the id of the good and the second describes how many
	std::vector<std::pair<Good *, size_t>> req_goods;
};

/**
* Defines the type of a naval unit
 */
typedef uint8_t BoatTypeId;
class BoatType {
public:
	using Id = BoatTypeId;
	std::string name;
	std::string ref_name;
	
	float speed;
	float max_health;
	float defense;
	float attack;
	
	// Capacity of stuff (units) that can be carried here
	size_t capacity;

	// Time needed to build
	size_t build_time;

	// Required goods, first describes the id of the good and the second describes how many
	std::vector<std::pair<Good *, size_t>> req_goods;
};

/** A trait for an unit; given randomly per each recruited unit
 */
typedef uint8_t UnitTraitId;
class UnitTrait {
public:
	using Id = UnitTraitId;
	
	std::string ref_name;
	
	float supply_consumption_mod;
	float speed_mod;
	float max_health_mod;
	float defense_mod;
	float attack_mod;
	float morale_mod;
};

#include "nation.hpp"
#include "world.hpp"
class Tile;
typedef uint32_t UnitId;
/**
* Roughly a batallion, consisting of approximately 500 soldiers each
 */
class Unit {
public:
	using Id = UnitId;
	
	// Type of unit
	UnitType* type;

	// Size of the unit (soldiers in unit)
	size_t size;

	// Base size of the unit (max size due to anti-attrition)
	size_t base;

	// Target coordinates of this unit
	size_t tx;
	size_t ty;

	// Current position
	float x;
	float y;

	// Who owns this unit
	Nation* owner;

	float morale;

	// For perspective, 0.5 is the normal unit (i.e a soldier POP)
	float experience;

	// Used to "ignore" an unit when doing any check, this allows other units to
	// attack this unit
	bool ignore_tag;

	// The ticks the unit has not been moved
	uint64_t defensive_ticks;

	// Available supplies, 1.0 is all supplies fullfilled, lower than that and the unit starts shrinking
	float supply;

	// Money that the unit has
	float budget;
	
	std::vector<UnitTrait*> traits;
};

/**
 * Type for military outposts
 */
typedef uint16_t OutpostTypeId;
class OutpostType {
public:
	using Id = OutpostTypeId;

	std::string ref_name;

	bool is_naval;
	bool is_build_land_units;
	bool is_build_naval_units;

	// Defensive bonus given to units on the outpost
	float defense_bonus;

	// Required goods, first describes the id of the good and the second describes how many
	std::vector<std::pair<Good *, size_t>> req_goods;
};

/**
* A military outpost, on land serves as a "spawn" place for units
* When adjacent to a water tile this serves as a shipyard for spawning naval units
 */
typedef uint16_t OutpostId;
class Outpost {
public:
	using Id = OutpostId;

	// Position of outpost
	size_t x;
	size_t y;

	OutpostType* type;

	// Owner of the outpost
	Nation* owner;

	// Unit that is currently being built here (nullptr indicates no unit)
	UnitType* working_unit_type;
	BoatType* working_boat_type;

	// Remaining ticks until the unit is built
	uint16_t build_time;

	// Required goods for building the working unit
	std::vector<std::pair<Good *, size_t>> req_goods_for_unit;

	// Required goods for building (current) outpost, first describes the id of the good
	// and the second describes how many this is for the working unit
	std::vector<std::pair<Good *, size_t>> req_goods;
};

#endif
