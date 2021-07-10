#include <cstdio>
#include <cstdlib>
#include <sys/cdefs.h>
#include <filesystem>
#include <cmath>
#include <set>

#include "province.hpp"
#include "economy.hpp"
#include "world.hpp"
#include "binary_image.hpp"
#include "lua_api.hpp"
#include "path.hpp"
#include "print.hpp"
#include "pathfinding.hpp"

#include "serializer.hpp"
#include "io_impl.hpp"
#include "network.hpp"

// Mostly used by clients and lua API
World* g_world;

/**
 * Creates a new world
  */
World::World(bool empty) {
	g_world = this;
	if(empty == true) {
		return;
	}
	
	BinaryImage topo(Path::get("map_topo.png").c_str());
	BinaryImage div(Path::get("map_div.png").c_str());
	BinaryImage infra(Path::get("map_infra.png").c_str());

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

	const size_t total_size = this->width* this->height;

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

	lua_register(this->lua, "add_unit_trait", LuaAPI::add_unit_trait);

	lua_register(this->lua, "add_outpost_type", LuaAPI::add_outpost_type);

	lua_register(this->lua, "add_good", LuaAPI::add_good);
	lua_register(this->lua, "get_good", LuaAPI::get_good);

	lua_register(this->lua, "add_industry_type", LuaAPI::add_industry_type);
	lua_register(this->lua, "get_industry_type", LuaAPI::get_industry_type);
	lua_register(this->lua, "add_input_to_industry_type", LuaAPI::add_input_to_industry_type);
	lua_register(this->lua, "add_output_to_industry_type", LuaAPI::add_output_to_industry_type);

	lua_register(this->lua, "add_nation", LuaAPI::add_nation);
	lua_register(this->lua, "get_nation", LuaAPI::get_nation);
	lua_register(this->lua, "set_nation_primary_culture", LuaAPI::set_nation_primary_culture);
	lua_register(this->lua, "set_nation_capital", LuaAPI::set_nation_capital);
	lua_register(this->lua, "set_industry_output_mod", LuaAPI::set_industry_output_mod);
	lua_register(this->lua, "set_industry_input_mod", LuaAPI::set_industry_input_mod);
	lua_register(this->lua, "set_workers_needed_mod", LuaAPI::set_workers_needed_mod);
	lua_register(this->lua, "set_salary_paid_mod", LuaAPI::set_salary_paid_mod);
	lua_register(this->lua, "set_delivery_cost_mod", LuaAPI::set_delivery_cost_mod);
	lua_register(this->lua, "set_literacy_learn_mod", LuaAPI::set_literacy_learn_mod);
	lua_register(this->lua, "set_reproduction_mod", LuaAPI::set_reproduction_mod);
	lua_register(this->lua, "set_death_mod", LuaAPI::set_death_mod);
	lua_register(this->lua, "set_militancy_mod", LuaAPI::set_militancy_mod);
	lua_register(this->lua, "set_consciousness_mod", LuaAPI::set_consciousness_mod);
	lua_register(this->lua, "set_life_needs_met_mod", LuaAPI::set_life_needs_met_mod);
	lua_register(this->lua, "set_everyday_needs_met_mod", LuaAPI::set_everyday_needs_met_mod);
	lua_register(this->lua, "set_luxury_needs_met_mod", LuaAPI::set_luxury_needs_met_mod);
	lua_register(this->lua, "add_nation_accepted_culture", LuaAPI::add_accepted_culture);
	
	lua_register(this->lua, "add_province", LuaAPI::add_province);
	lua_register(this->lua, "get_province", LuaAPI::get_province);
	lua_register(this->lua, "add_province_industry", LuaAPI::add_province_industry);
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
	lua_register(this->lua, "add_req_good_unit_type", LuaAPI::add_req_good_unit_type);

	lua_register(this->lua, "add_boat_type", LuaAPI::add_boat_type);
	lua_register(this->lua, "get_boat_type", LuaAPI::get_boat_type);
	lua_register(this->lua, "add_req_good_boat_type", LuaAPI::add_req_good_boat_type);

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

	// Add all scripts onto the path (with glob operator '?')
	curr_path.append(";");
	std::string path = Path::get("scripts");
	curr_path.append(path + "/?.lua");
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

		if(it == provinces.end()) {
			continue;
		}
		
		const ProvinceId province_id = std::distance(provinces.begin(), it);
		while(div.buffer[i] == (*it)->color) {
			tiles[i].province_id = province_id;
			provinces[province_id]->n_tiles++;
			i++;
		}
		i--;

		while(div.buffer[i] == 0xff000000 || div.buffer[i] == 0xffffffff) {
			i++;
		}
	}

	// Calculate the edges of the province (min and max x and y coordinates)
	printf("Calculate the edges of the province (min and max x and y coordinates)\n");
	for(size_t i = 0; i < this->width; i++) {
		for(size_t j = 0; j < this->height; j++) {
			const Tile* tile = &this->tiles[i + (j* this->width)];
			if(tile->province_id == (ProvinceId)-1)
				continue;

			Province* province = this->provinces[tile->province_id];
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

		// Add stockpile
		for(size_t i = 0; i < this->products.size(); i++) {
			province->stockpile.push_back(0);
		}
	}

	// Give owners the entire provinces
	printf("Give owners the entire provinces\n");
	for(auto& nation: nations) {
		for(auto& province: nation->owned_provinces) {
			const ProvinceId province_id = get_id(province);
			for(size_t x = province->min_x; x <= province->max_x; x++) {
				for(size_t y = province->min_y; y <= province->max_y; y++) {
					Tile& tile = get_tile(x, y);

					if(tile.province_id != province_id)
						continue;

					const NationId nation_id = get_id(province->owner);
					tile.owner_id = nation_id;
				}
			}
		}
		nation_changed_tiles.clear();
	}

	// Neighbours
	printf("Neighbours\n");
	for(size_t i = 0; i < total_size; i++) {
		const Tile* tile = &this->tiles[i];
		const Tile* other_tile;
		if(tile->owner_id != (NationId)-1) {
			Nation* nation = this->nations[this->tiles[i].owner_id];

			// Up neighbour
			if(i > this->width) {
				other_tile = &this->tiles[i - this->width];
				if(other_tile->owner_id != tile->owner_id
				&& other_tile->owner_id != (NationId)-1) {
					nation->neighbours.insert(this->nations[other_tile->owner_id]);
				}
			}
			// Down neighbour
			if(i < (this->width* this->height) - this->width) {
				other_tile = &this->tiles[i + this->width];
				if(other_tile->owner_id != tile->owner_id
				&& other_tile->owner_id != (NationId)-1) {
					nation->neighbours.insert(this->nations[other_tile->owner_id]);
				}
			}
			// Left neighbour
			if(i > 1) {
				other_tile = &this->tiles[i - 1];
				if(other_tile->owner_id != tile->owner_id
				&& other_tile->owner_id != (NationId)-1) {
					nation->neighbours.insert(this->nations[other_tile->owner_id]);
				}
			}
			// Right neighbour
			if(i < (this->width* this->height) - 1) {
				other_tile = &this->tiles[i + 1];
				if(other_tile->owner_id != tile->owner_id
				&& other_tile->owner_id != (NationId)-1) {
					nation->neighbours.insert(this->nations[other_tile->owner_id]);
				}
			}
		}
		
		if(tile->province_id != (ProvinceId)-1) {
			Province* province = this->provinces[this->tiles[i].province_id];

			// Up neighbour
			if(i > this->width) {
				other_tile = &this->tiles[i - this->width];
				if(other_tile->province_id != tile->province_id
				&& other_tile->province_id != (ProvinceId)-1) {
					province->neighbours.insert(this->provinces[other_tile->province_id]);
				}
			}
			// Down neighbour
			if(i < (this->width* this->height) - this->width) {
				other_tile = &this->tiles[i + this->width];
				if(other_tile->province_id != tile->province_id
				&& other_tile->province_id != (ProvinceId)-1) {
					province->neighbours.insert(this->provinces[other_tile->province_id]);
				}
			}
			// Left neighbour
			if(i > 1) {
				other_tile = &this->tiles[i - 1];
				if(other_tile->province_id != tile->province_id
				&& other_tile->province_id != (ProvinceId)-1) {
					province->neighbours.insert(this->provinces[other_tile->province_id]);
				}
			}
			// Right neighbour
			if(i < (this->width* this->height) - 1) {
				other_tile = &this->tiles[i + 1];
				if(other_tile->province_id != tile->province_id
				&& other_tile->province_id != (ProvinceId)-1) {
					province->neighbours.insert(this->provinces[other_tile->province_id]);
				}
			}
		}
	}

	// Create diplomatic relations between nations
	printf("Creating diplomatic relations\n");
	for(const auto& nation: this->nations) {
		// Relations between nations start at 0 (and latter modified by lua scripts)
		for(size_t i = 0; i < this->nations.size(); i++) {
			nation->relations.push_back(NationRelation{0.f, false, false, false, false, false, false, false, false, true, false});
		}
	}

	ret = luaL_dofile(this->lua, Path::get("scripts/mod.lua").c_str());
	if(ret) {
		print_error("lua error %s", lua_tostring(this->lua, -1));
		exit(EXIT_FAILURE);
	}
	
	// Default init
	for(auto& nation: this->nations) {
		nation->budget = 10000.f;
		
		Policies& policy = nation->current_policy;
		
		policy.import_tax = 1.2f;
		policy.export_tax = 1.2f;
		policy.foreign_trade = true;
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
	switch(time % (24* 2)) {
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
			nation->prestige -= (nation->prestige* (decay_per_cent / 100.f))* fmin(fmax(1, nation->prestige - min_prestige) / min_prestige, max_modifier);
		}
		break;
	// 12:00
	case 24:
		Economy::do_phase_3(*this);
		
		for(auto& nation: this->nations) {
			float economy_score = 0.f;
			for(const auto& province: nation->owned_provinces) {
				// Calculate economy score of nations
				for(const auto& pop: province->pops) {
					economy_score += pop.budget;
				}
				
				// Also calculates GDP
				for(const auto& product: g_world->products) {
					nation->gdp += product->price* province->stockpile[g_world->get_id(product)];
				}
			}
			nation->economy_score = economy_score / 100.f;
		}
		break;
	// 18:00
	case 36:
		Economy::do_phase_4(*this);
		break;
	// 24:00, this is where clients are sent all information **at once**
	case 48:
		{
			size_t i;
			
			i = 0;
			nations_mutex.lock();
			for(const auto& nation: g_world->nations) {
				// Broadcast to clients
				Packet packet = Packet(0);
				Archive ar = Archive();
				
				enum ActionType action = ACTION_NATION_UPDATE;
				::serialize(ar, &action);
				
				NationId nation_id = (NationId)i;
				::serialize(ar, &nation_id);
				
				::serialize(ar, &nation);
				
				packet.data(ar.get_buffer(), ar.size());
				g_server->broadcast(packet);
				
				i++;
			}
			nations_mutex.unlock();
			
			i = 0;
			provinces_mutex.lock();
			for(const auto& province: g_world->provinces) {
				// Broadcast to clients
				Packet packet = Packet(0);
				Archive ar = Archive();
				
				enum ActionType action = ACTION_PROVINCE_UPDATE;
				::serialize(ar, &action);
				
				ProvinceId province_id = (ProvinceId)i;
				::serialize(ar, &province_id);
				
				::serialize(ar, &province);
				
				packet.data(ar.get_buffer(), ar.size());
				g_server->broadcast(packet);
				
				i++;
			}
			provinces_mutex.unlock();
		}
		break;
	default:
		break;
	}
	
	// Evaluate units
	units_mutex.lock();
	for(size_t i = 0; i < units.size(); i++) {
		Unit* unit = units[i];
		if(unit->size <= 0) {
			g_world->units.erase(units.begin() + i);
			break;
		}
		
		// Count friends and foes in range (and find nearest foe)
		size_t n_friends = 0;
		size_t n_foes = 0;
		Unit* nearest_foe = nullptr;
		Unit* nearest_friend = nullptr;
		for(size_t j = 0; j < g_world->units.size(); j++) {
			Unit* other_unit = g_world->units[j];
			if(unit->owner == other_unit->owner) {
				// Only when very close
				if(std::abs(unit->x - other_unit->x) >= 4.f && std::abs(unit->y - other_unit->y) >= 4.f)
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
				if(std::abs(unit->x - other_unit->x) >= 1.f && std::abs(unit->y - other_unit->y) >= 1.f)
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
		new_tx = unit->tx;
		new_ty = unit->ty;

		if(new_tx == unit->x && new_ty == unit->y) {
			continue;
		}

		float end_x, end_y;
		const float speed = 0.1f;

		end_x = unit->x;
		end_y = unit->y;

		// Move towards target
		if(unit->x > new_tx)
			end_x -= speed;
		else if(unit->x < new_tx)
			end_x += speed;

		if(unit->y > new_ty)
			end_y -= speed;
		else if(unit->y < new_ty)
			end_y += speed;
		
		// Make the unit attack automatically
		// and we must be at war with the owner of this unit to be able to attack the unit
		if(nearest_foe != nullptr
		&& unit->owner->relations[get_id(nearest_foe->owner)].has_war == false) {
			Unit* enemy = nearest_foe;

			// Calculate the attack of our unit
			float attack_mod = 0.f;
			for(const auto& trait: unit->traits) {
				attack_mod *= trait->attack_mod;
			}
			const float attack = unit->type->attack * attack_mod;

			// Calculate the defense of the enemy
			float defense_mod = 0.f;
			for(const auto& trait: unit->traits) {
				defense_mod *= trait->defense_mod;
			}
			const float enemy_defense = std::max(0.1f, enemy->type->defense * defense_mod
				* (enemy->defensive_ticks * enemy->type->position_defense))
			;

			// Calculate the total damage dealt by our unit to the enemy
			const float damage_dealt = unit->size * std::min(10.f, std::max(.05f, unit->experience))
				* (attack / std::pow(std::min(0.f, enemy_defense), 2))
				* std::max(0.1f, unit->morale) * unit->supply
			;
			
			// Deal with the morale loss of the enemy
			float enemy_fanaticism = 0.f;
			for(const auto& trait: enemy->traits) {
				enemy_fanaticism *= trait->morale_mod;
			}
			enemy->morale -= 10.f * enemy_fanaticism * damage_dealt / enemy->size;

			// Our unit receives half of the morale
			unit->morale += 5.f * enemy_fanaticism * damage_dealt / enemy->size;

			// Deal the damage
			enemy->size -= damage_dealt;
		}

		// Unit is on a non-wasteland part of the map
		if(get_tile(unit->x, unit->y).province_id != (Province::Id)-1) {
			Province* province = provinces[get_tile(unit->x, unit->y).province_id];
			const Nation* nation = province->owner;
			bool free_supplies = false;

			// Unit is on domestic soil, so we have to check the domsetic policy for free military supplies
			if(unit->owner == nation) {
				// No-cost supplies
				if(unit->owner->current_policy.free_supplies) {
					free_supplies = true;
				}
			}
			// Unit is on foreign soil, we check relations to see if we can take free military supplies
			else {
				// No-cost supplies
				if(unit->owner->relations[get_id(nation)].free_supplies) {
					free_supplies = true;
				}
			}

			if(free_supplies == true) {
				// Take anything you want, it's not needed to fucking pay at all! :)
				for(size_t i = 0; i < province->stockpile.size(); i++) {
					if(!province->stockpile[i])
						continue;

					// We will take your food pleseantly
					if(products[i]->good->is_edible
					&& unit->supply <= (unit->type->supply_consumption * 10.f)) {
						float bought = std::min(unit->size, province->stockpile[i]);
						province->stockpile[i] -= bought;

						unit->supply += bought / unit->size;
						unit->morale += bought / unit->size;
					}
					// Fuck you, we are also taking your luxury because it's free
					else {
						float bought = std::min((rand() + 1) % unit->size, province->stockpile[i]);
						province->stockpile[i] -= bought;

						// Yes, we are super happy with your voluntary gifts to the honourable
						// units of some nation
						unit->morale += bought / unit->size;
					}
				}
			} else {
				// Buy stuff and what we are able to buy normally
				for(size_t i = 0; i < province->stockpile.size(); i++) {
					// Must be edible and there must be stock
					if(!products[i]->good->is_edible || !province->stockpile[i])
						continue;

					if(products[i]->price * unit->size <= unit->budget) {
						size_t bought = std::min(province->stockpile[i], unit->size);
						province->stockpile[i] -= bought;
						unit->supply = bought / unit->size;

						// Pay (including taxes)
						const float paid = (products[i]->price * unit->size) * province->owner->current_policy.med_flat_tax;
						province->owner->budget += paid;
						unit->budget -= paid;
					}

					// We will stop buying if we are satisfied
					if(unit->supply >= (unit->type->supply_consumption * 1.f))
						break;
				}
			}
		}

		// This code prevents us from stepping onto water tiles (but allows for rivers)
		if(get_tile(end_x, end_y).elevation <= sea_level) {
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
		if(tile.owner_id != get_id(unit->owner)) {
			std::unique_lock<std::mutex> lock(nation_changed_tiles_mutex);
			tile.owner_id = get_id(unit->owner);
			nation_changed_tiles.push_back(&get_tile(unit->x, unit->y));
		}
	}
	
	size_t i = 0;
	for(const auto& unit: g_world->units) {
		// Broadcast to clients
		Packet packet = Packet(0);
		Archive ar = Archive();
		
		enum ActionType action = ACTION_UNIT_UPDATE;
		::serialize(ar, &action);
		
		UnitId unit_id = (UnitId)i;
		::serialize(ar, &unit_id);
		::serialize(ar, unit);
		
		packet.data(ar.get_buffer(), ar.size());
		g_server->broadcast(packet);
		
		i++;
	}
	units_mutex.unlock();
	
	// Tell clients that this tick has been done
	Packet packet = Packet(0);
	Archive ar = Archive();
	enum ActionType action = ACTION_WORLD_TICK;
	::serialize(ar, &action);
	::serialize(ar, &g_world->time);
	packet.data(ar.get_buffer(), ar.size());
	g_server->broadcast(packet);
	
	LuaAPI::check_events(lua);
	
	time++;
}

// TODO: Do a template or something to remove code duplication

Nation::Id World::get_id(const Nation* ptr) const {
	//std::lock_guard<std::mutex> lock(nations_mutex);
	return get_id_ptr<Nation::Id>(ptr, nations);
}

Province::Id World::get_id(const Province* ptr) const {
	//std::lock_guard<std::mutex> lock(provinces_mutex);
	return get_id_ptr<Province::Id>(ptr, provinces);
}

Product::Id World::get_id(const Product* ptr) const {
	//std::lock_guard<std::mutex> lock(products_mutex);
	return get_id_ptr<Product::Id>(ptr, products);
}

Good::Id World::get_id(const Good* ptr) const {
	//std::lock_guard<std::mutex> lock(goods_mutex);
	return get_id_ptr<Good::Id>(ptr, goods);
}

Culture::Id World::get_id(const Culture* ptr) const {
	//std::lock_guard<std::mutex> lock(cultures_mutex);
	return get_id_ptr<Culture::Id>(ptr, cultures);
}

Company::Id World::get_id(const Company* ptr) const {
	//std::lock_guard<std::mutex> lock(companies_mutex);
	return get_id_ptr<Company::Id>(ptr, companies);
}

IndustryType::Id World::get_id(const IndustryType* ptr) const {
	//std::lock_guard<std::mutex> lock(industry_types_mutex);
	return get_id_ptr<IndustryType::Id>(ptr, industry_types);
}

Industry::Id World::get_id(const Province& province, const Industry* ptr) const {
	//std::lock_guard<std::mutex> lock(provinces_mutex);
	return ((ptrdiff_t)ptr - (ptrdiff_t)&province.industries[0]) / sizeof(Industry);
}

Event::Id World::get_id(const Event* ptr) const {
	//std::lock_guard<std::mutex> lock(events_mutex);
	return get_id_ptr<Event::Id>(ptr, events);
}

UnitType::Id World::get_id(const UnitType* ptr) const {
	//std::lock_guard<std::mutex> lock(unit_types_mutex);
	return get_id_ptr<UnitType::Id>(ptr, unit_types);
}

BoatType::Id World::get_id(const BoatType* ptr) const {
	//std::lock_guard<std::mutex> lock(boat_types_mutex);
	return get_id_ptr<BoatType::Id>(ptr, boat_types);
}

UnitTrait::Id World::get_id(const UnitTrait* ptr) const {
	//std::lock_guard<std::mutex> lock(unit_traits_mutex);
	return get_id_ptr<UnitTrait::Id>(ptr, unit_traits);
}

Unit::Id World::get_id(const Unit* ptr) const {
	//std::lock_guard<std::mutex> lock(units_mutex);
	return get_id_ptr<Unit::Id>(ptr, units);
}

OutpostType::Id World::get_id(const OutpostType* ptr) const {
	//std::lock_guard<std::mutex> lock(outpost_types_mutex);
	return get_id_ptr<OutpostType::Id>(ptr, outpost_types);
}

Outpost::Id World::get_id(const Outpost* ptr) const {
	//std::lock_guard<std::mutex> lock(outposts_mutex);
	return get_id_ptr<Outpost::Id>(ptr, outposts);
}

size_t World::get_id(const Tile* ptr) const {
	//std::lock_guard<std::mutex> lock(tiles_mutex);
	return ((ptrdiff_t)ptr - (ptrdiff_t)tiles) / sizeof(Tile);
}

	// Obtains a tile from the world safely, and makes sure that it is in bounds
Tile& World::get_tile(size_t x, size_t y) const {
	std::lock_guard<std::mutex> lock(tiles_mutex);
	if(x >= width || y >= height) {
		throw "Tile out of bounds";
	}
	return tiles[x + y* width];
}

Tile& World::get_tile(size_t idx) const {
	std::lock_guard<std::mutex> lock(tiles_mutex);
	if(idx >= width* height) {
		throw "Tile index exceeds boundaries";
	}
	return tiles[idx];
}
