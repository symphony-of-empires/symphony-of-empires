// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      server/emigration.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <cstdio>
#include <cstdio>

#include "server/emigration.hpp"
#include "world.hpp"
#include "disc_dist.hpp"

class Emigrated {
public:
	Emigrated(Pop& pop) : emigred{pop} {};
	~Emigrated() {};

	Province* origin;
	Province* target;
	Pop emigred;
	size_t size;
};

void conlonial_migration(World& world);
void internal_migration(World& world);
void external_migration(World& world);

void do_emigration(World& world) {
	external_migration(world);
	internal_migration(world);
	conlonial_migration(world);
}

void conlonial_migration(World&) {

}

void internal_migration(World&) {
	
}

// Basic 
float nation_attraction(Nation* nation, Culture* culture) {
	float attraction = nation->get_immigration_attraction_mod();
	if(nation->is_accepted_culture(*culture)) {
		// Linearized version, instead of using if-else trees we just
		// multiply the attractive by the scale; EXTERMINATE = 3, so 3 - 3 is 0 which nullifies the attractivenes
		// and the more open the borders are the more lenient the "scale" becomes
		const int scale = 3 - nation->current_policy.treatment;
		attraction *= scale;
	}
	return attraction;
}

float province_attraction(Province* province) {
	float attraction = province->base_attractive;
	return attraction;
}

void external_migration(World& world) {
	std::vector<DiscreteDistribution<Province*>*> province_distributions;
	for(auto nation : world.nations) {
		std::vector<float> attractions;
		std::vector<Province*> viable_provinces;
		for(auto province : nation->owned_provinces) {
			float attraction = province_attraction(province);
			if(attraction <= 0) {
				continue;
			}

			attractions.push_back(attraction);
			viable_provinces.push_back(province);
		}

		DiscreteDistribution<Province*>* distribution = nullptr;
		if(viable_provinces.size() > 0) {
			distribution = new DiscreteDistribution(viable_provinces, attractions);
		}
		province_distributions.push_back(distribution);
	}

	std::vector<DiscreteDistribution<Nation*>*> nation_distributions;
	for(auto culture : world.cultures) {
		std::vector<float> attractions;
		std::vector<Nation*> viable_nations;
		for(auto nation : world.nations) {
			if(nation->owned_provinces.size() == 0) {
				continue;
			}

			if(nation->current_policy.migration == ALLOW_NOBODY) {
				continue;
			} else if(nation->current_policy.migration == ALLOW_ACCEPTED_CULTURES) {
				if(!nation->accepted_cultures.count(culture)) {
					continue;
				}
			}

			float attraction = nation_attraction(nation, culture);
			if(attraction <= 0) {
				continue;
			}

			attractions.push_back(attraction);
			viable_nations.push_back(nation);
		}
		DiscreteDistribution<Nation*>* distribution = nullptr;
		if(viable_nations.size() > 0) {
			distribution = new DiscreteDistribution(viable_nations, attractions);
		}
		nation_distributions.push_back(distribution);
	}

	std::vector<Emigrated> emigration = std::vector<Emigrated>();
	std::for_each(world.provinces.begin(), world.provinces.end(), [&province_distributions, &nation_distributions, &emigration, &world](auto& province) {
		if(province->controller == nullptr) {
			return;
		}

		if(province->terrain_type->is_water_body) {
			return;
		}

		// Randomness factor to emulate a pseudo-imperfect economy
		const float fuzz = std::fmod((float)(std::rand() + 1) / 1000.f, 2.f) + 1.f;

		for(size_t i = 0; i < province->pops.size(); i++) {
			Pop& pop = province->pops[i];

			// Depending on how much not our life needs are being met is how many we
			// want to get out of here
			// And literacy determines "best" spot, for example a low literacy will
			// choose a slightly less desirable location
			const float emigration_willing = std::max<float>(-pop.life_needs_met * std::fmod(fuzz, 10), 0);
			const ssize_t emigreers = std::fmod(pop.size * (emigration_willing / (std::rand() % 8)), pop.size) / 100;
			if(emigreers > 0) {
				// Check that laws on the province we are in allows for emigration
				if(province->controller->current_policy.migration == ALLOW_NOBODY) {
					continue;
				}

				auto nation_distribution = nation_distributions[pop.culture->cached_id];
				if(nation_distribution == nullptr) {
					continue;
				}

				auto nation = nation_distribution->get_item();
				auto province_distribution = province_distributions[nation->cached_id];
				if(province_distribution == nullptr) {
					continue;
				}

				auto choosen_province = province_distribution->get_item();
				Emigrated emigrated = Emigrated(province->pops[i]);
				emigrated.target = choosen_province;
				emigrated.size = emigreers;
				emigrated.origin = province;

				emigration.push_back(emigrated);
				pop.size -= emigreers;
			}
		}
	});

	// Now time to do the emigration - we will create a new POP on the province
	// if a POP with similar culture, religion and type does not exist - and we
	// will also subtract the amount of emigrated from the original POP to not
	// create clones
	for(const auto& target : emigration) {
		auto new_pop = std::find(target.target->pops.begin(), target.target->pops.end(), target.emigred);
		if(new_pop == target.target->pops.end()) {
			Pop i_pop( target.emigred );
			i_pop.size = target.size;
			target.target->pops.push_back(i_pop);
		} else {
			new_pop->size += target.size;
			new_pop->budget += target.emigred.budget;
		}
	}
	emigration.clear();
}