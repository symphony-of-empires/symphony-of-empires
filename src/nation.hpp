#ifndef NATION_H
#define NATION_H

#include <stdint.h>
#include <string>
#include <vector>
#include "texture.hpp"

class NationRelation {
public:
	float relation;
	bool has_embargo;
	bool has_war;
	bool has_alliance;
	bool has_defensive_pact;
	bool has_truce;
	bool has_embassy;
	bool has_military_access;
};

class Nation {
public:
	Nation() {};
	Nation& operator=(const Nation&) = default;

	// Default transleted/display name
	std::string name;

	// Default reference name
	std::string ref_name;

	// Default color of the country
	uint32_t color;

	// Default flag texture of the country
	Texture * default_flag = new Texture();

	// Relations with all other countries
	std::vector<NationRelation> relations;

	// Id of the nation that has us on their sphere of influence
	size_t spherer_id;

	// Number of diplomacy points available
	float diplomacy_points;

	// Total number of prestige
	float prestige;

	// Base literacy applied to all pops
	float base_literacy;
};

#endif
