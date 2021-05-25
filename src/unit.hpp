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

#include "nation.hpp"
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
};

class Tile;
/**
 * A set of units who are commanded by a general, generally 1K - 10K in size
 */
class UnitGroup {
public:
	// Units that conform this group
	std::vector<Unit *> units;

	// Target coordinates, saved for multi-group strategies later on
	size_t tx;
	size_t ty;

	// Path the group should follow to reach target
	std::vector<Tile *> path;

	// Who owns this unit group
	NationId owner_id;
};

#endif