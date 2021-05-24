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

#include <cmath>
void World::do_economy_tick_1() {
	const size_t n_provinces = this->provinces.size();

	// All factories will place their orders for their inputs
	// All RGOs will do deliver requests
	for(size_t i = 0; i < n_provinces; i++) {
		// Reset remaining supplies
		this->provinces[i]->supply_rem = this->provinces[i]->supply_limit;
		
		// This new tick, we will start the chain starting with RGOs producing deliver
		// orders and the factories that require inputs will put their orders in the
		// table so they are served by transport companies.
		// This is the start of the factory chain.
		for(size_t j = 0; j < this->provinces[i]->industries.size(); j++) {
			Industry * industry = this->provinces[i]->industries[j];
			if(!this->provinces[i]->worker_pool) {
				industry->ticks_unoperational++;
				continue;
			}

			industry->ticks_unoperational = 0;

			size_t available_manpower;
			available_manpower = 1500 % this->provinces[i]->worker_pool;
			this->provinces[i]->worker_pool -= available_manpower;
			if(!available_manpower)
				continue;
			
			IndustryType * it = this->industry_types[industry->type_id];
			for(const auto& input: it->inputs) {
				OrderGoods order;
				order.quantity = available_manpower / 24;
				order.payment = 500.f;
				order.good_id = input;
				order.requester_industry_id = j;
				order.requester_province_id = i;
				orders.push_back(order);
			}

			if(industry->can_do_output(this) == false)
				continue;

			// Now produce anything as we can!
			// Take a constant of workers needed for factories, 1.5k workers!
			available_manpower = 1500 % this->provinces[i]->worker_pool;
			this->provinces[i]->worker_pool -= available_manpower;
			if(!available_manpower)
				continue;
				
			// Place deliver orders (we are a RGO)
			for(size_t k = 0; k < it->outputs.size(); k++) {
				DeliverGoods deliver;
				deliver.quantity = available_manpower / 32;
				deliver.payment = 1500.f;
				deliver.good_id = it->outputs[k];
				deliver.sender_industry_id = j;
				deliver.sender_province_id = i;
				deliver.product_id = industry->output_products[k];

				// Do not overproduce
				Product * product = this->products[deliver.product_id];
				if(product->supply > product->demand) {
					continue;
				}

				// Cannot be below production cost, so we can be profitable
				if(product->price < industry->production_cost * 1.2f) {
					product->price = industry->production_cost * 1.2f;
				}
				
				if(!deliver.quantity)
					continue;
				
				delivers.push_back(deliver);
			}
		}
	}
}

#include "pathfinding.hpp"
void World::do_economy_tick_2() {
	// Now we will deliver stuff accordingly
	while(delivers.size() && orders.size()) {
		// Now transport companies will check and transport accordingly
		for(auto& company: this->companies) {
			if(!company->is_transport)
				continue;
			
			// Check all delivers
			for(size_t i = 0; i < delivers.size(); i++) {
				DeliverGoods * deliver = &delivers[i];
				deliver->rejections++;
				
				if(!company->in_range(deliver->sender_province_id))
					continue;
				
				// Check all orders
				for(size_t j = 0; j < orders.size(); j++) {
					OrderGoods * order = &orders[j];
					
					// Do they want this goods?
					if(order->good_id != deliver->good_id)
						continue;
					
					// Are we in the range to deliver?
					if(!company->in_range(order->requester_province_id))
						continue;
					
					// Province receives a small supply buff from commerce
					this->provinces[order->requester_province_id]->supply_rem += 5.f;
					
					// Obtain number of goods that we could satisfy
					size_t count = order->quantity % deliver->quantity;
					deliver->quantity -= count;
					order->quantity -= count;

					// Yes - we go and deliver their stuff
					this->provinces[order->requester_province_id]->industries[order->requester_industry_id]->add_to_stock(this, order->good_id, count);
					
					// Increment demand
					this->products[deliver->product_id]->demand += count;
					// Decrement supply
					if(this->products[deliver->product_id]->supply) {
						size_t satisfied = count % this->products[deliver->product_id]->supply;
						this->products[deliver->product_id]->supply -= satisfied;
						this->products[deliver->product_id]->demand += satisfied;
					}

					this->provinces[order->requester_province_id]->industries[order->requester_industry_id]->production_cost += this->products[deliver->product_id]->price;
					
					// On 0 it overflows to -1, on 1 it goes to zero, but after this loop it's
					// incremented again (see incrementing below). So we don't take a rejection hit
					deliver->rejections--;
					
					// Delete this deliver and order tickets from the system since
					// they are now fullfilled (only delete when no quanity left)
					if(!order->quantity) {
						orders.erase(orders.begin() + j);
					} if(!deliver->quantity) {
						delivers.erase(delivers.begin() + i);
						i--;
					}
					break;
				}
			}
		}
		
		// Get number of transporter companies
		size_t n_transporters = 0;
		for(const auto& company: this->companies) {
			if(company->is_transport)
				n_transporters++;
		}
		
		// Drop all rejected delivers
		for(size_t i = 0; i < delivers.size(); i++) {
			DeliverGoods * deliver = &delivers[i];
			if(deliver->rejections >= n_transporters) {
				// Add up to province stockpile
				this->provinces[deliver->sender_province_id]->stockpile[deliver->product_id] += deliver->quantity;
				this->products[deliver->product_id]->supply += deliver->quantity;
				if(this->products[deliver->product_id]->demand) {
					this->products[deliver->product_id]->demand -= deliver->quantity % this->products[deliver->product_id]->demand;
				}
				delivers.erase(delivers.begin() + i);
				i--;
			}
		}

		// No orders if no delivers! (and can't deliver if no orders)
		if(!delivers.size() || !orders.size())
			break;
	}
	delivers.clear();
	orders.clear();
}

void World::do_economy_tick_3() {
	// Now, it's like 1 am here, but i will try to write a very nice economic system
	// TODO: There is a lot to fix here, first the economy system commits inverse great depression and goes way too happy
	for(Province *& province: this->provinces) {
		// Reset worker pool
		province->worker_pool = 0;
		for(size_t i = 0; i < province->pops.size(); i++) {
			Pop * pop = province->pops[i];
			// If pop is of size 0, then delete it, it's dead :(
			if(!pop->size) {
				province->pops.erase(province->pops.begin() + i);
				i--;
				continue;
			}

			//printf("%s: %s %zu (%4.f$)\n", province->name.c_str(), this->pop_types[pop->type_id]->name.c_str(), pop->size, pop->budget);
			
			float salary;
			
			// TODO: This is very stupid
			switch(pop->type_id) {
			case POP_TYPE_ENTRPRENEUR:
				salary = 100.f;
				break;
			case POP_TYPE_ARTISAN:
				salary = 12.f;
				break;
			case POP_TYPE_CRAFTSMEN:
				salary = 10.f;
				break;
			case POP_TYPE_BUREAUCRAT:
				salary = 30.f;
				break;
			case POP_TYPE_ARISTOCRAT:
				salary = 50.f;
				break;
			case POP_TYPE_CLERGYMEN:
				salary = 12.f;
				break;
			case POP_TYPE_FARMER:
				salary = 8.f;
				break;
			case POP_TYPE_SOLDIER:
				salary = 10.f;
				break;
			case POP_TYPE_OFFICER:
				salary = 20.f;
				break;
			case POP_TYPE_LABORER:
				salary = 8.f;
				break;
			case POP_TYPE_SLAVE:
				salary = 2.f;
				break;
			default:
				salary = 0.f;
				break;
			}
			
			// TODO: Make this dynamic
			pop->budget += salary;
			
			size_t alloc_budget;
			
			// Use 25% of our budget to buy edibles and life needed stuff
			alloc_budget = pop->budget / 4;
			for(size_t j = 0; j < province->stockpile.size(); j++) {
				// Province must have stockpile
				if(!province->stockpile[j])
					continue;
				
				// Must be an edible
				if(this->goods[this->products[j]->good_id]->is_edible != true)
					continue;
				
				// We can only spend our allocated budget
				// TODO: Base spending on literacy, more literacy == less spending
				size_t bought = alloc_budget / this->products[j]->price;
				bought %= province->stockpile[j];
				if(!bought)
					continue;
				
				// Satisfaction of needs is proportional to bought items
				pop->life_needs_met += (float)pop->size / (float)bought;
				
				// Demand is incremented proportional to items bought
				this->products[j]->demand += bought;
				
				// Reduce supply
				if(this->products[j]->supply) {
					this->products[j]->supply -= bought % this->products[j]->supply;
				}
				
				// Deduct from budget, and remove item from stockpile
				pop->budget -= bought * this->products[j]->price;
				alloc_budget -= bought * this->products[j]->price;
				province->stockpile[j] -= bought;
			}
			
			// Use 10% of our budget for buying uneeded commodities and shit
			// TODO: Should lower spending with higher literacy, and higher
			// TODO: Higher the fullfilment per unit with higher literacy
			alloc_budget = pop->budget / 10;
			for(size_t j = 0; j < province->stockpile.size(); j++) {
				// Province must have stockpile
				if(!province->stockpile[j])
					continue;
				
				// Must be a non-edible; aka. not essential
				if(this->goods[this->products[j]->good_id]->is_edible != false)
					continue;
				
				// We can only spend our allocated budget
				// TODO: Base spending on literacy, more literacy == less spending
				size_t bought = alloc_budget / this->products[j]->price;
				bought %= province->stockpile[j];
				if(!bought)
					continue;
				
				// Satisfaction of needs is proportional to bought items
				pop->everyday_needs_met += (float)pop->size / (float)bought;
				
				// Demand is incremented proportional to items bought
				this->products[j]->demand += bought;
				
				// Reduce supply
				if(this->products[j]->supply) {
					this->products[j]->supply -= bought % this->products[j]->supply;
				}
				
				// Deduct from budget, and remove item from stockpile
				pop->budget -= bought * this->products[j]->price;
				alloc_budget -= bought * this->products[j]->price;
				province->stockpile[j] -= bought;
			}
			
			// x5 life needs met modifier, that is the max allowed
			if(pop->life_needs_met > 5.f) {
				pop->life_needs_met = 5.f;
			}
			
			// POPs cannot shrink below 100<
			if(pop->size <= 100) {
				pop->size += rand() % 100;
			} else {
				// Higher literacy means less births and less deaths
				ssize_t growth = (pop->life_needs_met * (rand() % 80)) / (pop->literacy * 4.f);
				if(growth < 0 && (size_t)abs(growth) >= pop->size) {
					pop->size = 100;
					continue;
				} else {
					pop->size += growth;
				}
			}
			
			// Met life needs means less militancy
			if(pop->life_needs_met >= 1.f) {
				pop->life_needs_met = 1.f;
				if(pop->militancy > 0.f) {
					pop->militancy -= 0.002f;
					pop->consciousness -= 0.001f;
				}
			} else {
				pop->militancy += 0.01f;
				pop->consciousness += 0.01f;
			}
			
			pop->life_needs_met -= 0.5f;
			
			province->worker_pool += pop->size;
		}
		
		// Stockpiles cleaned
		for(size_t i = 0; i < province->stockpile.size(); i++) {
			province->stockpile[i] = 0;
		}
	}
}

void World::do_economy_tick_4() {
	// Preparations for the next tick

	// Reset production costs
	const size_t n_provinces = this->provinces.size();
	for(size_t i = 0; i < n_provinces; i++) {
		const size_t n_industries = this->provinces[i]->industries.size();
		for(size_t j = 0; j < n_industries; j++) {
			Industry * industry = this->provinces[i]->industries[j];
			industry->production_cost = 0.f;
		}
	}

	// Close today's price with a change according to demand - supply
	const size_t n_products = this->products.size();
	for(size_t i = 0; i < n_products; i++) {
		Product * product = this->products[i];
		if(product->demand > product->supply) {
			product->price_vel += 0.001f * (product->demand - product->supply);
		} else if(product->demand < product->supply) {
			product->price_vel -= 0.001f * (product->supply - product->demand);
		} else {
			if(product->price_vel > 0.1f) {
				product->price_vel -= 0.1f;
			} else if(product->price_vel < -0.1f) {
				product->price_vel += 0.1f;
			} else {
				product->price_vel = -0.1f;
			}
		}
		product->price += product->price_vel;
		if(product->price <= 0.f) {
			product->price = 0.01f;
		}
			
		// Save prices and stuff onto history (for the charts!)
		product->demand_history.push_back(product->demand);
		if(product->demand_history.size() > 60)
			product->demand_history.pop_front();
		
		product->supply_history.push_back(product->supply);
		if(product->supply_history.size() > 60)
			product->supply_history.pop_front();

		product->price_history.push_back(product->price);
		if(product->price_history.size() > 60)
			product->price_history.pop_front();
			
		// Re-count worldwide supply
		product->supply = 0;
		for(const auto& province: this->provinces) {
			product->supply += province->stockpile[i];
		}
		product->demand = 0;
	}
}

void World::do_tick() {
	// Each tick == 30 minutes
	switch(time % (24 * 2)) {
	// 3:00
	case 6:
		this->do_economy_tick_1();
		break;
	// 7:30
	// Busy hour, newspapers come out and people get mad
	case 15:
		this->do_economy_tick_2();

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
		this->do_economy_tick_3();

		// Calculate economy score of nations
		for(auto& nation: this->nations) {
			double economy_score = 0.f;
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
		this->do_economy_tick_4();
		break;
	default:
		break;
	}

	// Do diplomacy

	// Evaluate units
	for(size_t i = 0; i < this->units.size(); i++) {
		Unit * unit = this->units[i];
		if(unit->health <= 0.f) {
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
				if(fabs(unit->x - other_unit->x) >= 8 && fabs(unit->y - other_unit->y) >= 8)
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
		
		if(nearest_foe == nullptr)
			continue;
		
		// Too much enemies, retreat
		if((n_foes / 3) > n_friends) {
			// Go away from foes
			if(nearest_foe->x < unit->x)
				unit->x += 0.1f;
			if(nearest_foe->y < unit->y)
				unit->y += 0.1f;
			if(nearest_foe->x > unit->x)
				unit->x -= 0.1f;
			if(nearest_foe->y > unit->y)
				unit->y -= 0.1f;
			
			// Attack nearest foe when possible
			if(fabs(unit->x - nearest_foe->x) <= 1.f && fabs(unit->y - nearest_foe->y) <= 1.f) {
				nearest_foe->health -= 10.f;
			}
		}
		// The gang is able to attack, so we attack
		else {
			// Attack enemies
			if(nearest_foe->x > unit->x + 1.f)
				unit->x += 0.1f;
			if(nearest_foe->y > unit->y + 1.f)
				unit->y += 0.1f;
			if(nearest_foe->x < unit->x - 1.f)
				unit->x -= 0.1f;
			if(nearest_foe->y < unit->y - 1.f)
				unit->y -= 0.1f;
			
			unit->tx = nearest_foe->x;
			unit->ty = nearest_foe->y;
			
			// If in distance, do attack
			if(fabs(unit->x - nearest_foe->x) <= 1.f && fabs(unit->y - nearest_foe->y) <= 1.f) {
				nearest_foe->health -= 10.f;
			}
		}

		Tile * tile = &g_world->tiles[(size_t)unit->x + (size_t)unit->y * g_world->width];
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
