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
	Texture div(Path::get("map_div.png").c_str());
	Texture infra(Path::get("map_infra.png").c_str());

	this->width = topo.width;
	this->height = topo.height;

	// Check that size of all maps match
	if(topo.width != this->width || topo.height != this->height) {
		print_error("topographic map size mismatch");
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
	lua_register(this->lua, "add_province_owner", LuaAPI::add_province_owner);

	lua_register(this->lua, "add_company", LuaAPI::add_company);

	lua_register(this->lua, "add_event", LuaAPI::add_event);
	lua_register(this->lua, "get_event", LuaAPI::get_event);
	lua_register(this->lua, "add_event_receivers", LuaAPI::add_event_receivers);

	lua_register(this->lua, "add_descision", LuaAPI::add_descision);

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

	// Constants for ease of readability
	lua_pushboolean(lua, true);
	lua_setglobal(lua, "EVENT_CONDITIONS_MET");
	lua_pushboolean(lua, false);
	lua_setglobal(lua, "EVENT_CONDITIONS_UNMET");

	lua_pushboolean(lua, true);
	lua_setglobal(lua, "EVENT_DO_MANY_TIMES");
	lua_pushboolean(lua, false);
	lua_setglobal(lua, "EVENT_DO_ONE_TIME");

	// TODO: The. name. is. fucking. long.
	lua_register(lua, "add_op_province_to_company", LuaAPI::add_op_province_to_company);

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
	printf("Shrink normally-not-resized vectors to give back memory to the OS\n");
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
	printf("Translate all div, pol and topo maps onto this single tile array\n");
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
	}

	// Associate tiles with provinces
	printf("Associate tiles with provinces\n");
	for(size_t i = 0; i < total_size; i++) {
		const uint32_t color = div.buffer[i];
		const auto it = std::find_if(provinces.begin(), provinces.end(), [&color](const auto& element) {
			return (color == element->color);
		});
		if(it != provinces.end()) {
			const ProvinceId province_id = std::distance(provinces.begin(), it);
			while(div.buffer[i] == (*it)->color) {
				tiles[i].province_id = province_id;
				provinces[province_id]->n_tiles++;
				i++;
			}
			i--;
		}
	}

	// Remove provinces with no tiles
	printf("Remove provinces with no tiles\n");
	for(auto& province: provinces) {
		if(!province->n_tiles) {
			print_error("Province %s has no tiles present on the map", province->ref_name.c_str());
		}
	}

	// Calculate the edges of the province (min and max x and y coordinates)
	printf("Calculate the edges of the province (min and max x and y coordinates)\n");
	for(size_t i = 0; i < this->width; i++) {
		for(size_t j = 0; j < this->height; j++) {
			const Tile * tile = &this->tiles[i + (j * this->width)];
			if(tile->province_id == (ProvinceId)-1)
				continue;

			Province * province = this->provinces[tile->province_id];
			province->max_x = std::max(province->max_x, i);
			province->max_y = std::max(province->max_y, j);
			province->min_x = std::min(province->min_x, i);
			province->min_y = std::min(province->min_y, j);
		}
	}

	// Correct stuff from provinces
	printf("Correcting values for provinces\n");
	for(auto& province: this->provinces) {
		province->max_x = std::min(this->width, province->max_x);
		province->max_y = std::min(this->height, province->max_y);

		// Remove duplicates
		std::set<Province *> s;
		const size_t size = province->neighbours.size();
		for(size_t i = 0; i < size; i++) {
			s.insert(province->neighbours[i]);
		}
		province->neighbours.assign(s.begin(), s.end());

		// Add stockpile
		for(size_t i = 0; i < this->products.size(); i++) {
			province->stockpile.push_back(0);
		}

		// These will not change in a while
		province->neighbours.shrink_to_fit();
	}

	// Give owners the entire provinces
	printf("Give owners the entire provinces\n");
	for(auto& nation: nations) {
		for(auto& province: nation->owned_provinces) {
			print_info("Nation %s owns %s", nation->name.c_str(), province->name.c_str());
			const ProvinceId province_id = get_id<ProvinceId>(province, provinces);
			for(size_t x = province->min_x; x <= province->max_x; x++) {
				for(size_t y = province->min_y; y <= province->max_y; y++) {
					Tile& tile = get_tile(x, y);

					if(tile.province_id != province_id)
						continue;

					const NationId nation_id = get_id<NationId>(province->owner, nations);
					tile.owner_id = nation_id;
				}
			}
		}
		nation_changed_tiles.clear();

		if(!nation->owned_provinces.size()) {
			nation->exists = false;
		}
	}

	// Neighbours
	printf("Neighbours\n");
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

	// Create diplomatic relations between nations
	printf("Creating diplomatic relations\n");
	for(const auto& nation: this->nations) {
		// Relations between nations start at 0 (and latter modified by lua scripts)
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

	printf("World fully intiialized\n");
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
			const float min_prestige = std::max<float>(0.5f, ((nation->naval_score + nation->military_score + nation->economy_score) / 2));

			// Prestige cannot go below min prestige
			nation->prestige = std::max<float>(nation->prestige, min_prestige);
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
	for(size_t i = 0; i < units.size(); i++) {
		Unit * unit = units[i];
		if(unit->size <= 0) {
			g_world->units.erase(units.begin() + i);
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
				if(std::abs(unit->x - other_unit->x) >= 8.f && std::abs(unit->y - other_unit->y) >= 8.f)
					continue;
				
				n_friends++;
				
				if(nearest_friend == nullptr) {
					nearest_friend = other_unit;
				}
				
				// Find nearest friend
				if(std::abs(unit->x - other_unit->x) < std::abs(unit->x - nearest_friend->x)
				&& std::abs(unit->y - other_unit->y) < std::abs(unit->y - nearest_friend->y)) {
					nearest_friend = other_unit;
				}
			} else {
				// Foes from many ranges counts
				if(std::abs(unit->x - other_unit->x) >= 15 && std::abs(unit->y - other_unit->y) >= 15)
					continue;
				
				n_foes++;
				
				if(nearest_foe == nullptr) {
					nearest_foe = other_unit;
				}

				// Find nearest foe
				if(std::abs(unit->x - other_unit->x) < std::abs(unit->x - nearest_foe->x)
				&& std::abs(unit->y - other_unit->y) < std::abs(unit->y - nearest_foe->y)) {
					nearest_foe = other_unit;
				}
			}
		}

		float new_tx, new_ty;
		
		// Cant be too close of friends (due to supply limits)
		if(nearest_friend != nullptr
		&& std::abs(nearest_friend->x - unit->x) <= 4.f && std::abs(nearest_friend->y - unit->y) <= 4.f) {
			// Get away away from friend, we can't take too much attrition
			new_tx = (unit->x < nearest_friend->tx) ? (nearest_friend->tx + 4.f) : (nearest_friend->tx - 4.f);
			new_ty = (unit->y < nearest_friend->ty) ? (nearest_friend->ty + 4.f) : (nearest_friend->ty - 4.f);
		}
		// Too much enemies, retreat
		if(nearest_foe != nullptr && (n_foes / 3) > n_friends) {
			// Go away from foes
			new_tx = (nearest_foe->x > unit->x) ? nearest_foe->x : nearest_foe->x;
			new_ty = (nearest_foe->y > unit->y) ? nearest_foe->y : nearest_foe->y;
			
			// Attack nearest foe when possible
			if(std::abs(unit->x - nearest_foe->x) <= 1.f && std::abs(unit->y - nearest_foe->y) <= 1.f) {
				nearest_foe->size -= 10;
			}
		}
		// The gang is able to attack, so we attack
		else if(nearest_foe != nullptr) {
			// Attack enemies
			new_tx = (nearest_foe->x > unit->x) ? nearest_foe->x : nearest_foe->x;
			new_ty = (nearest_foe->y > unit->y) ? nearest_foe->y : nearest_foe->y;
			
			// If in distance, do attack
			if(std::abs(unit->x - nearest_foe->x) <= 1.f && std::abs(unit->y - nearest_foe->y) <= 1.f) {
				nearest_foe->size -= 10;
			}
		}
		// Nothing to do - we just die
		else {
			new_tx = unit->x;
			new_ty = unit->y;
		}

		if(new_tx == unit->x && new_ty == unit->y) {
			continue;
		} else if(unit->tx != new_tx && unit->ty != new_ty) {
			// Tiles below sea, as targets, are unacceptable (thus we don't have a target at all!)!
			if(get_tile(new_tx, new_ty).elevation < sea_level) {
				continue;
			}
		}

		unit->tx = new_tx;
		unit->ty = new_ty;

		float end_x, end_y;
		const float speed = 0.1f;

		end_x = unit->x;
		end_y = unit->y;

		// Move towards target
		if(unit->x > unit->tx)
			end_x -= speed;
		else if(unit->x < unit->tx)
			end_x += speed;

		if(unit->y > unit->ty)
			end_y -= speed;
		else if(unit->y < unit->ty)
			end_y += speed;

		// This code prevents us from stepping onto water tiles (but allows for rivers)
		if(get_tile(end_x, end_y).elevation < sea_level) {
			continue;
		}

		unit->x = end_x;
		unit->y = end_y;

		// North and south do not wrap
		unit->y = std::max<float>(0.f, unit->y);
		unit->y = std::min<float>(height, unit->y);

		// West and east do wrap
		if(unit->x <= 0.f) {
			unit->x = width - 1.f;
		} else if(unit->x >= width) {
			unit->x = 0.f;
		}

		// Set nearby tiles as owned
		// TODO: Make it conquer multiple tiles
		Tile& tile = get_tile(unit->x, unit->y);
		if(tile.owner_id != unit->owner_id) {
			std::unique_lock<std::mutex> lock(nation_changed_tiles_mutex);
			tile.owner_id = unit->owner_id;
			nation_changed_tiles.push_back(&get_tile(unit->x, unit->y));
		}
	}

	LuaAPI::check_events(lua);

	time++;
}

void World::add_good(Good * good) {
	goods.push_back(good);
}

void World::add_industry_type(IndustryType * it) {
	industry_types.push_back(it);
}

void World::add_nation(Nation * nation) {
	nations.push_back(nation);
}

void World::add_province(Province * province) {
	provinces.push_back(province);
}
