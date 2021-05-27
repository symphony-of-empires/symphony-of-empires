#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <cmath>
#include <set>

#include "province.hpp"
#include "economy.hpp"
#include "world.hpp"
#include "texture.hpp"
#include "lua.hpp"
#include "path.hpp"
#include "print.hpp"
#include "pathfinding.hpp"

// Mostly used by clients and lua API
World * g_world;

/**
  * Creates a new world
  */
World::World() {
	g_world = this;
	
	Texture topo(Path::get("map_topo.png").c_str());
	Texture pol(Path::get("map_pol.png").c_str());
	Texture div(Path::get("map_div.png").c_str());
	Texture infra(Path::get("map_infra.png").c_str());

	this->width = topo.width;
	this->height = topo.height;

	// Check that size of all maps match
	if(topo.width != this->width || topo.height != this->height) {
		print_error("topographic map size mismatch");
		exit(EXIT_FAILURE);
	} else if(pol.width != this->width || pol.height != this->height) {
		print_error("political map size mismatch");
		exit(EXIT_FAILURE);
	} else if(div.width != this->width || div.height != this->height) {
		print_error("province map size mismatch");
		exit(EXIT_FAILURE);
	}

	const size_t total_size = this->width * this->height;

	// Sea is	<= sea_level
	// Rivers	== sea_level + 1
	// Land is	> sea_level + 1
	this->sea_level = 126;
	this->tiles = new Tile[total_size];
	if(this->tiles == nullptr) {
		perror("out of mem\n");
		exit(EXIT_FAILURE);
	}

	this->lua = luaL_newstate();
	luaL_openlibs(this->lua);

	// Register our API functions
	lua_register(this->lua, "_", LuaAPI::get_text);

	lua_register(this->lua, "add_good", LuaAPI::add_good);
	lua_register(this->lua, "get_good", LuaAPI::get_good);

	lua_register(this->lua, "add_industry_type", LuaAPI::add_industry_type);
	lua_register(this->lua, "get_industry_type", LuaAPI::get_industry_type);
	lua_register(this->lua, "add_input_to_industry_type", LuaAPI::add_input_to_industry_type);
	lua_register(this->lua, "add_output_to_industry_type", LuaAPI::add_output_to_industry_type);

	lua_register(this->lua, "add_nation", LuaAPI::add_nation);
	lua_register(this->lua, "get_nation", LuaAPI::get_nation);
	lua_register(this->lua, "set_nation_primary_culture", LuaAPI::set_nation_primary_culture);

	lua_register(this->lua, "add_province", LuaAPI::add_province);
	lua_register(this->lua, "get_province", LuaAPI::get_province);
	lua_register(this->lua, "add_province_pop", LuaAPI::add_province_pop);
	lua_register(this->lua, "give_province_to", LuaAPI::give_province_to);
	lua_register(this->lua, "rename_province", LuaAPI::rename_province);
	lua_register(this->lua, "add_province_nucleus", LuaAPI::add_province_nucleus);

	lua_register(this->lua, "add_company", LuaAPI::add_company);

	lua_register(this->lua, "add_event", LuaAPI::add_event);
	lua_register(this->lua, "get_event", LuaAPI::get_event);
	lua_register(this->lua, "add_event_receivers", LuaAPI::add_event_receivers);

	lua_register(this->lua, "add_pop_type", LuaAPI::add_pop_type);
	lua_register(this->lua, "get_pop_type", LuaAPI::get_pop_type);

	lua_register(this->lua, "add_culture", LuaAPI::add_culture);
	lua_register(this->lua, "get_culture", LuaAPI::get_culture);

	lua_register(this->lua, "add_religion", LuaAPI::add_religion);
	lua_register(this->lua, "get_religion", LuaAPI::get_religion);

	lua_register(this->lua, "add_unit_type", LuaAPI::add_unit_type);
	lua_register(this->lua, "get_unit_type", LuaAPI::get_unit_type);

	lua_register(this->lua, "get_hour", LuaAPI::get_hour);
	lua_register(this->lua, "get_day", LuaAPI::get_day);
	lua_register(this->lua, "get_month", LuaAPI::get_month);
	lua_register(this->lua, "get_year", LuaAPI::get_year);

	// TODO: The. name. is. fucking. long.
	lua_register(this->lua, "add_op_province_to_company", LuaAPI::add_op_province_to_company);

	// Set path for `require` statements in lua, this will allow us to require
	// without using data/scripts
	lua_getglobal(this->lua, "package");
	lua_getfield(this->lua, -1, "path");
	std::string curr_path = lua_tostring(this->lua, -1);
	curr_path.append(";");
	curr_path.append(Path::get("scripts/api.lua"));
	lua_pop(this->lua, 1);
	lua_pushstring(this->lua, curr_path.c_str());
	lua_setfield(this->lua, -2, "path");
	lua_pop(this->lua, 1);

	int ret = luaL_dofile(this->lua, Path::get("scripts/init.lua").c_str());
	if(ret) {
		print_error("lua error %s", lua_tostring(this->lua, -1));
		exit(EXIT_FAILURE);
	}

	// Shrink normally-not-resized vectors to give back memory to the OS
	this->provinces.shrink_to_fit();
	this->nations.shrink_to_fit();
	this->goods.shrink_to_fit();
	this->industry_types.shrink_to_fit();
	this->unit_types.shrink_to_fit();
	this->cultures.shrink_to_fit();
	this->religions.shrink_to_fit();
	this->pop_types.shrink_to_fit();

	for(auto& province: this->provinces) {
		province->max_x = 0;
		province->max_y = 0;
		province->min_x = UINT32_MAX;
		province->min_y = UINT32_MAX;
	}

	// Translate all div, pol and topo maps onto this single tile array
	const size_t n_nations = this->nations.size();
	const size_t n_provinces = this->provinces.size();
	for(size_t i = 0; i < total_size; i++) {
		// Set coordinates for the tiles
		this->tiles[i].owner_id = (NationId)-1;
		this->tiles[i].province_id = (ProvinceId)-1;
		this->tiles[i].elevation = topo.buffer[i] & 0x000000ff;
		if(topo.buffer[i] == 0xffff0000) {
			this->tiles[i].elevation = this->sea_level + 1;
		}

		// Set infrastructure level
		if(infra.buffer[i] == 0xffffffff
		|| infra.buffer[i] == 0xff000000) {
			this->tiles[i].infra_level = 0;
		} else {
			this->tiles[i].infra_level = 1;
		}

		// Associate tiles with nations
		for(size_t j = 0; j < n_nations; j++) {
			Nation * nation = this->nations[j];
			if(pol.buffer[i] != nation->color)
				continue;
			
			this->tiles[i].owner_id = (NationId)j;
		}

		for(size_t j = 0; j < n_provinces; j++) {
			Province * province = this->provinces[j];
			if(div.buffer[i] != province->color)
				continue;
				
			this->tiles[i].province_id = (ProvinceId)j;
			break;
		}
	}

	// Neighbours
	for(size_t i = 0; i < total_size; i++) {
		const Tile * tile = &this->tiles[i];
		const Tile * other_tile;
		if(tile->owner_id != (NationId)-1) {
			Nation * nation = this->nations[this->tiles[i].owner_id];

			// Up neighbour
			if(i > this->width) {
				other_tile = &this->tiles[i - this->width];
				if(other_tile->owner_id != tile->owner_id
				&& other_tile->owner_id != (NationId)-1) {
					nation->neighbours.push_back(this->nations[other_tile->owner_id]);
				}
			}
			// Down neighbour
			if(i < (this->width * this->height) - this->width) {
				other_tile = &this->tiles[i + this->width];
				if(other_tile->owner_id != tile->owner_id
				&& other_tile->owner_id != (NationId)-1) {
					nation->neighbours.push_back(this->nations[other_tile->owner_id]);
				}
			}
			// Left neighbour
			if(i > 1) {
				other_tile = &this->tiles[i - 1];
				if(other_tile->owner_id != tile->owner_id
				&& other_tile->owner_id != (NationId)-1) {
					nation->neighbours.push_back(this->nations[other_tile->owner_id]);
				}
			}
			// Right neighbour
			if(i < (this->width * this->height) - 1) {
				other_tile = &this->tiles[i + 1];
				if(other_tile->owner_id != tile->owner_id
				&& other_tile->owner_id != (NationId)-1) {
					nation->neighbours.push_back(this->nations[other_tile->owner_id]);
				}
			}
		}
		
		if(tile->province_id != (ProvinceId)-1) {
			Province * province = this->provinces[this->tiles[i].province_id];
			// If province had no owner before - now it has it!
			if(province->owner_id == PROVINCE_NO_ONWER) {
				province->owner_id = this->tiles[i].owner_id;
			}

			// Set provinces as disputed if many countries owns this province
			if(province->owner_id != PROVINCE_DISPUTED
			&& province->owner_id != PROVINCE_NO_ONWER
			&& province->owner_id != this->tiles[i].owner_id) {
				province->owner_id = PROVINCE_DISPUTED;
				province->owners.push_back(this->nations[this->tiles[i].owner_id]);
			}

			// Up neighbour
			if(i > this->width) {
				other_tile = &this->tiles[i - this->width];
				if(other_tile->province_id != tile->province_id
				&& other_tile->province_id != (ProvinceId)-1) {
					province->neighbours.push_back(this->provinces[other_tile->province_id]);
				}
			}
			// Down neighbour
			if(i < (this->width * this->height) - this->width) {
				other_tile = &this->tiles[i + this->width];
				if(other_tile->province_id != tile->province_id
				&& other_tile->province_id != (ProvinceId)-1) {
					province->neighbours.push_back(this->provinces[other_tile->province_id]);
				}
			}
			// Left neighbour
			if(i > 1) {
				other_tile = &this->tiles[i - 1];
				if(other_tile->province_id != tile->province_id
				&& other_tile->province_id != (ProvinceId)-1) {
					province->neighbours.push_back(this->provinces[other_tile->province_id]);
				}
			}
			// Right neighbour
			if(i < (this->width * this->height) - 1) {
				other_tile = &this->tiles[i + 1];
				if(other_tile->province_id != tile->province_id
				&& other_tile->province_id != (ProvinceId)-1) {
					province->neighbours.push_back(this->provinces[other_tile->province_id]);
				}
			}
		}
	}

	// Calculate the edges of the province (min and max x and y coordinates)
	for(size_t i = 0; i < this->width; i++) {
		for(size_t j = 0; j < this->height; j++) {
			const Tile * tile = &this->tiles[i + (j * this->width)];
			if(tile->province_id == (ProvinceId)-1)
				continue;

			Province * province = this->provinces[tile->province_id];
			province->max_x = fmax(province->max_x, i);
			province->max_y = fmax(province->max_y, j);
					
			province->min_x = fmin(province->min_x, i);
			province->min_y = fmin(province->min_y, j);
		}
	}

	// Correct stuff from provinces
	for(auto& province: this->provinces) {
		province->max_x = fmin(this->width, province->max_x);
		province->max_y = fmin(this->height, province->max_y);

		// Remove duplicates
		{
			std::set<Nation *> s;
			const size_t size = province->owners.size();
			for(size_t i = 0; i < size; i++) {
				s.insert(province->owners[i]);
			}
			province->owners.assign(s.begin(), s.end());
		} {
			std::set<Province *> s;
			const size_t size = province->neighbours.size();
			for(size_t i = 0; i < size; i++) {
				s.insert(province->neighbours[i]);
			}
			province->neighbours.assign(s.begin(), s.end());
		}

		// Add stockpile
		for(size_t i = 0; i < this->products.size(); i++) {
			province->stockpile.push_back(0);
		}

		// These will not change in a while
		province->owners.shrink_to_fit();
		province->neighbours.shrink_to_fit();
	}

	// Register all provinces onto the owning nations
	for(size_t i = 0; i < n_provinces; i++) {
		const Province * province = this->provinces[i];
		if(province->owner_id >= n_nations) {
			continue;
		}

		Nation * nation = this->nations[province->owner_id];
		nation->owned_provinces.push_back(this->provinces[i]);
	}

	// Create diplomatic relations between nations
	for(const auto& nation: this->nations) {
		// Relations between nations start at 0
		for(size_t i = 0; i < this->nations.size(); i++) {
			nation->relations.push_back(NationRelation{0.f, false, false, false, false, false, false, false, false, true});
		}

		std::set<Nation *> s;
		const size_t size = nation->neighbours.size();
		for(size_t i = 0; i < size; i++) {
			s.insert(nation->neighbours[i]);
		}
		nation->neighbours.assign(s.begin(), s.end());

		nation->neighbours.shrink_to_fit();
		nation->relations.shrink_to_fit();
		nation->owned_provinces.shrink_to_fit();
	}

	ret = luaL_dofile(this->lua, Path::get("scripts/mod.lua").c_str());
	if(ret) {
		print_error("lua error %s", lua_tostring(this->lua, -1));
		exit(EXIT_FAILURE);
	}

	printf("world fully intiialized\n");
}

World::~World() {
	lua_close(this->lua);
	delete[] this->tiles;

	for(auto& religion: this->religions) {
		delete religion;
	} for(auto& unit_type: this->unit_types) {
		delete unit_type;
	} for(auto& event: this->events) {
		delete event;
	} for(auto& industry_type: this->industry_types) {
		delete industry_type;
	} for(auto& company: this->companies) {
		delete company;
	} for(auto& pop_type: this->pop_types) {
		delete pop_type;
	} for(auto& culture: this->cultures) {
		delete culture;
	} for(auto& good: this->goods) {
		delete good;
	} for(auto& province: this->provinces) {
		delete province;
	} for(auto& nation: this->nations) {
		delete nation;
	}
}

#include <deque>
#include <mutex>
extern std::mutex render_province_mutex;
extern std::deque<size_t> render_province;

#include "economy.hpp"
void World::do_tick() {
	// Each tick == 30 minutes
	switch(time % (24 * 2)) {
	// 3:00
	case 6:
		Economy::do_phase_1(*this);
		break;
	// 7:30
	// Busy hour, newspapers come out and people get mad
	case 15:
		Economy::do_phase_2(*this);

		// Calculate prestige for today (newspapers come out!)
		for(auto& nation: this->nations) {
			const float decay_per_cent = 5.f;
			const float max_modifier = 10.f;
			const float min_prestige = fmax(0.5f, ((nation->naval_score + nation->military_score + nation->economy_score) / 2));

			// Prestige cannot go below min prestige
			nation->prestige = fmax(nation->prestige, min_prestige);
			nation->prestige += (nation->prestige * (decay_per_cent / 100.f)) * fmin(fmax(1, nation->prestige - min_prestige) / min_prestige, max_modifier);
		}
		break;
	// 12:00
	case 24:
		Economy::do_phase_3(*this);

		// Calculate economy score of nations
		for(auto& nation: this->nations) {
			float economy_score = 0.f;
			for(const auto& province: nation->owned_provinces) {
				for(const auto& pop: province->pops) {
					economy_score += pop->budget;
				}
			}
			nation->economy_score = economy_score / 100.f;
		}
		break;
	// 18:00
	case 36:
		Economy::do_phase_4(*this);
		break;
	default:
		break;
	}

	// Do diplomacy

	// Evaluate units
	for(size_t i = 0; i < this->units.size(); i++) {
		Unit * unit = this->units[i];
		if(unit->size <= 0) {
			g_world->units.erase(this->units.begin() + i);
			break;
		}
		
		unit->tx = unit->x;
		unit->ty = unit->y;
		
		// Count friends and foes in range (and find nearest foe)
		size_t n_friends = 0;
		size_t n_foes = 0;
		Unit * nearest_foe = nullptr;
		Unit * nearest_friend = nullptr;
		for(size_t j = 0; j < g_world->units.size(); j++) {
			Unit * other_unit = g_world->units[j];
			if(unit->owner_id == other_unit->owner_id) {
				// Only when very close
				if(fabs(unit->x - other_unit->x) >= 8.f && fabs(unit->y - other_unit->y) >= 8.f)
					continue;
				
				n_friends++;
				
				if(nearest_friend == nullptr) {
					nearest_friend = other_unit;
				}
				
				// Find nearest friend
				if(fabs(unit->x - other_unit->x) < fabs(unit->x - nearest_friend->x)
				&& fabs(unit->y - other_unit->y) < fabs(unit->y - nearest_friend->y)) {
					nearest_friend = other_unit;
				}
			} else {
				// Foes from many ranges counts
				if(fabs(unit->x - other_unit->x) >= 15 && fabs(unit->y - other_unit->y) >= 15)
					continue;
				
				n_foes++;
				
				if(nearest_foe == nullptr) {
					nearest_foe = other_unit;
				}

				// Find nearest foe
				if(fabs(unit->x - other_unit->x) < fabs(unit->x - nearest_foe->x)
				&& fabs(unit->y - other_unit->y) < fabs(unit->y - nearest_foe->y)) {
					nearest_foe = other_unit;
				}
			}
		}

		//unit->tx = unit->x;
		//unit->ty = unit->y;
		
		// Cant be too close of friends (due to supply limits)
		if(nearest_friend != nullptr
		&& fabs(nearest_friend->x - unit->x) <= 4.f && fabs(nearest_friend->y - unit->y) <= 4.f) {
			// Get away away from friend, we can't take too much attrition
			unit->tx = (unit->tx > nearest_friend->tx) ? (nearest_friend->tx + 4.f) : (nearest_friend->tx - 4.f);
			unit->ty = (unit->ty > nearest_friend->ty) ? (nearest_friend->ty + 4.f) : (nearest_friend->ty - 4.f);
		}
		// Too much enemies, retreat
		if(nearest_foe != nullptr && (n_foes / 3) > n_friends) {
			// Go away from foes
			unit->tx = (nearest_foe->x > unit->x) ? nearest_foe->x : nearest_foe->x;
			unit->ty = (nearest_foe->y > unit->y) ? nearest_foe->y : nearest_foe->y;
			
			// Attack nearest foe when possible
			if(fabs(unit->x - nearest_foe->x) <= 1.f && fabs(unit->y - nearest_foe->y) <= 1.f) {
				nearest_foe->size -= 10;
			}
		}
		// The gang is able to attack, so we attack
		else if(nearest_foe != nullptr) {
			// Attack enemies
			unit->tx = (nearest_foe->x > unit->x) ? nearest_foe->x : nearest_foe->x;
			unit->ty = (nearest_foe->y > unit->y) ? nearest_foe->y : nearest_foe->y;
			
			// If in distance, do attack
			if(fabs(unit->x - nearest_foe->x) <= 1.f && fabs(unit->y - nearest_foe->y) <= 1.f) {
				nearest_foe->size -= 10;
			}
		}
		// Nothing to do - we just die
		else {
			unit->tx = unit->x;
			unit->ty = unit->y;
		}

		// Move towards target
		if(unit->x > unit->tx)
			unit->x -= 0.1f;
		else if(unit->x < unit->tx)
			unit->x += 0.1f;

		if(unit->y > unit->ty)
			unit->y -= 0.1f;
		else if(unit->y < unit->ty)
			unit->y += 0.1f;

		// North and south do not wrap
		unit->y = fmax(0.f, unit->y);
		unit->y = fmin(this->height, unit->y);

		// West and east do wrap
		if(unit->x <= 0.f) {
			unit->x = this->width - 1.f;
		} else if(unit->x >= this->width) {
			unit->x = 0.f;
		}

		// Set nearby tiles as owned
		// TODO: Make it conquer multiple tiles
		Tile * tile = &this->tiles[(size_t)unit->x + (size_t)unit->y * this->width];
		if(tile->owner_id != unit->owner_id) {
			tile->owner_id = unit->owner_id;
			render_province_mutex.lock();
			render_province.push_front(unit->x + 64);
			render_province.push_front(unit->y + 64);
			render_province.push_front(unit->x - 64);
			render_province.push_front(unit->y - 64);
			render_province_mutex.unlock();
		}
	}

	this->time++;
}

void World::add_good(Good * good) {
	this->goods.push_back(good);
}

void World::add_industry_type(IndustryType * it) {
	this->industry_types.push_back(it);
}

void World::add_nation(Nation * nation) {
	this->nations.push_back(nation);
}

void World::add_province(Province * province) {
	this->provinces.push_back(province);
}
