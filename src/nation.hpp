#ifndef NATION_H
#define NATION_H

#include <stdint.h>
#include <string>
#include <vector>
#include "texture.hpp"

class Nation {
public:
	Nation() {};
	Nation& operator=(const Nation&) = default;

	std::string name; // Default transleted/display name
	std::string ref_name; // Default reference name
	uint32_t color; // Default color of the country
	Texture * default_flag = new Texture(); // Default flag texture of the country

	std::vector<float> relations; // Relations with all other countries
	size_t spherer_id; // Id of the nation that has us on their sphere of influence

	float diplomacy_points; // Number of diplomacy points available

	float prestige; // Total number of prestige
	float base_literacy; // Base literacy applied to all pops
};

#endif
