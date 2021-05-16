#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>
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

	// Sea is	<= sea_level
	// Rivers	== sea_level + 1
	// Land is	> sea_level + 1
	this->sea_level = 126;
	this->tiles = new Tile[this->width * this->height];
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

	lua_register(this->lua, "get_hour", LuaAPI::get_hour);
	lua_register(this->lua, "get_day", LuaAPI::get_day);
	lua_register(this->lua, "get_month", LuaAPI::get_month);
	lua_register(this->lua, "get_year", LuaAPI::get_year);

	// TODO: The. name. is. fucking. long.
	lua_register(this->lua, "add_op_province_to_company", LuaAPI::add_op_province_to_company);

	// Set path for `require` statements in lua
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

	// Translate all div, pol and topo maps onto this single tile array
	const size_t n_nations = this->nations.size();
	const size_t n_provinces = this->provinces.size();
	uint16_t last_nation_color_id = 0;
	for(size_t i = 0; i < this->width * this->height; i++) {
		// Set coordinates for the tiles
		this->tiles[i].x = i % this->width; 
		this->tiles[i].y = i / this->width;
		
		this->tiles[i].elevation = topo.buffer[i] & 0x000000ff;
		if(topo.buffer[i] == 0xffff0000) {
			this->tiles[i].elevation = this->sea_level + 1;
		}

		// Associate tiles with nations
		this->tiles[i].owner_id = (uint16_t)-1;

		if(this->nations[last_nation_color_id]->color != pol.buffer[i]) {
			for(size_t j = 0; j < n_nations; j++) {
				if(pol.buffer[i] == this->nations[j]->color) {
					this->tiles[i].owner_id = (uint16_t)j;
					last_nation_color_id = (uint16_t)j;
					break;
				}
			}
		} else {
			this->tiles[i].owner_id = last_nation_color_id;
		}

		// Associate tiles with provinces
		this->tiles[i].province_id = (uint16_t)-1;
		for(size_t j = 0; j < n_provinces; j++) {
			if(div.buffer[i] != this->provinces[j]->color)
				continue;
			
			this->tiles[i].province_id = j;

			// Only evaluated when valid owner
			if(this->tiles[i].owner_id != (uint16_t)-1) {
				// If province had no owner before - now it has it!
				if(this->provinces[j]->owner_id == PROVINCE_NO_ONWER) {
					this->provinces[j]->owner_id = this->tiles[i].owner_id;
				}

				// Set provinces as disputed if many countries owns this province
				if(this->provinces[j]->owner_id != PROVINCE_DISPUTED
				&& this->provinces[j]->owner_id != PROVINCE_NO_ONWER
				&& this->provinces[j]->owner_id != this->tiles[i].owner_id) {
					this->provinces[j]->owner_id = PROVINCE_DISPUTED;
				}
			}
			break;
		}

		// Set infrastructure level
		if(infra.buffer[i] == 0xffffffff
		|| infra.buffer[i] == 0xff000000) {
			this->tiles[i].infra_level = 0;
		} else {
			this->tiles[i].infra_level = 1;
		}
	}

	// Calculate the edges of the province (min and max x and y coordinates)
	for(size_t i = 0; i < this->width; i++) {
		for(size_t j = 0; j < this->height; j++) {
			Tile * tile = &this->tiles[i + (j * this->width)];
			for(size_t k = 0; k < this->provinces.size(); k++) {
				if(tile->province_id == k) {
					Province * province = this->provinces[k];
					if(i < province->min_x)
						province->min_x = i;
					if(j < province->min_y)
						province->min_y = j;
					if(i > province->max_x)
						province->max_x = i;
					if(j > province->max_y)
						province->max_y = j;
				}
			}
		}
	}

	// Correct stuff from provinces
	for(auto& province: this->provinces) {
		if(province->max_x > this->width)
			province->max_x = this->width;
		if(province->min_x > this->width)
			province->min_x = this->width;
		
		if(province->max_y > this->height)
			province->max_y = this->height;
		if(province->min_y > this->height)
			province->min_y = this->height;

		// Add stockpile
		for(size_t i = 0; i < this->products.size(); i++) {
			province->stockpile.push_back(0);
		}
	}

	// Create diplomatic relations between nations
	for(auto& nation: this->nations) {
		// Relations between nations start at 0
		for(size_t i = 0; i < this->nations.size(); i++) {
			nation->relations.push_back(NationRelation{0.f, false, false, false, false, false, false});
		}
	}

	// Shrink normally-not-resized vectors
	this->provinces.shrink_to_fit();
	this->nations.shrink_to_fit();
	this->goods.shrink_to_fit();
	this->industry_types.shrink_to_fit();

	ret = luaL_dofile(this->lua, Path::get("scripts/mod.lua").c_str());
	if(ret) {
		print_error("lua error %s", lua_tostring(this->lua, -1));
		exit(EXIT_FAILURE);
	}
}

World::~World() {
	lua_close(this->lua);
	delete[] this->tiles;

	for (size_t i = 0; i < g_world->nations.size(); i++) {
		delete g_world->nations[i]->default_flag;
	}
}

class OrderGoods {
public:
	float payment;
	size_t good_id;
	size_t requester_industry_id;
	size_t requester_province_id;
	size_t quantity;
};
class DeliverGoods {
public:
	float payment;
	size_t good_id;
	size_t sender_industry_id;
	size_t sender_province_id;
	size_t rejections = 0;
	size_t quantity;
	size_t product_id;
};

#include <deque>
#include <mutex>
extern std::mutex render_province_mutex;
extern std::deque<size_t> render_province;

#include <cmath>
void World::do_tick() {
	const size_t n_provinces = this->provinces.size();
	std::vector<OrderGoods> orders;
	std::vector<DeliverGoods> delivers;
	
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
			if(!this->provinces[i]->worker_pool) {
				this->provinces[i]->industries[j]->ticks_unoperational++;
				continue;
			}
			
			this->provinces[i]->industries[j]->ticks_unoperational = 0;
			
			//printf("Industry (type: %s). Province %zu. n_industry: %zu\n", this->industry_types[this->provinces[i]->industries[j]->type_id]->name.c_str(), i, j);
			
			IndustryType * it = this->industry_types[this->provinces[i]->industries[j]->type_id];
			for(const auto& input: it->inputs) {
				OrderGoods order;
				order.quantity = 500;
				order.payment = 500.f;
				order.good_id = input;
				order.requester_industry_id = j;
				order.requester_province_id = i;
				orders.push_back(order);
				//printf("\t- We need good: %s (from %s)\n", this->goods[order.good_id]->name.c_str(), this->provinces[order.requester_province_id]->name.c_str());
			}
			
			// RGOs deliver first
			if(it->inputs.size() == 0) {
				// Take a constant of workers needed for factories, 1.5k workers!
				size_t available_manpower = 1500 % this->provinces[i]->worker_pool;
				this->provinces[i]->worker_pool -= available_manpower;
				
				// Place deliver orders (we are a RGO)
				for(size_t k = 0; k < it->outputs.size(); k++) {
					DeliverGoods deliver;
					deliver.quantity = 4 * available_manpower;
					deliver.payment = 1500.f;
					deliver.good_id = it->outputs[k];
					deliver.sender_industry_id = j;
					deliver.sender_province_id = i;
					deliver.product_id = this->provinces[i]->industries[j]->output_products[k];
					
					if(!deliver.quantity)
						continue;
					
					delivers.push_back(deliver);
					//printf("\t- Throwing RGO: %s (from %s)\n", this->goods[deliver.good_id]->name.c_str(), this->provinces[deliver.sender_province_id]->name.c_str());
					break;
				}
			}
		}
	}

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
					
					//printf("%s: Delivered from %s to %s some %s\n", company->name.c_str(), this->provinces[deliver->sender_province_id]->name.c_str(), this->provinces[order->requester_province_id]->name.c_str(), this->goods[order->good_id]->name.c_str());
					
					// Yes - we go and deliver their stuff
					Industry * industry = this->provinces[order->requester_province_id]->industries[order->requester_industry_id];
					industry->add_to_stock(this, order->good_id, 1);
					
					// Province receives a small supply buff from commerce
					this->provinces[order->requester_province_id]->supply_rem += 5.f;
					
					// Obtain number of goods that we could satisfy
					size_t count = order->quantity % deliver->quantity;
					deliver->quantity -= count;
					order->quantity -= count;
					
					// Increment demand
					this->products[deliver->product_id]->demand += count;
					// Decrement supply
					if(this->products[deliver->product_id]->supply) {
						this->products[deliver->product_id]->supply -= count % this->products[deliver->product_id]->supply;
					}
					
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
	
	// Now, it's like 1 am here, but i will try to write a very nice economic system
	// TODO: There is a lot to fix here, first the economy system commits inverse great depression and goes way too happy
	for(auto& province: this->provinces) {
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
			
			float salary;
			
			// TODO: This is very stupid
			switch(pop->type_id) {
			case 0:
				salary = 50000.f;
				break;
			case 1:
				salary = 500.f;
				break;
			case 2:
				salary = 500.f;
				break;
			case 3:
				salary = 1500.f;
				break;
			case 4:
				salary = 5000.f;
				break;
			default:
				salary = 1000.f;
				break;
			}
			
			// TODO: Make this dynamic
			pop->budget += salary;
			
			//printf("pop, budget %4.f, size %zu\n", pop->budget, pop->size);
			
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
				
				//printf("Buying life needed %zu %s\n", bought, this->goods[this->products[j]->good_id]->name.c_str());
			}
			
			// Use 50% of our budget for buying uneeded commodities and shit
			// TODO: Should lower spending with higher literacy, and higher
			// TODO: Higher the fullfilment per unit with higher literacy
			alloc_budget = pop->budget / 2;
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
				
				//printf("Buying non-esseintial %zu %s\n", bought, this->goods[this->products[j]->good_id]->name.c_str());
			}
			
			// x5 life needs met modifier, that is the max allowed
			if(pop->life_needs_met > 5.f) {
				pop->life_needs_met = 5.f;
			}
			
			// TODO: Higher literacy should mean lower births
			ssize_t growth = pop->life_needs_met * (rand() % 10);
			if(growth < 0 && (size_t)growth >= pop->size) {
				pop->size = 0;
				continue;
			} else {
				pop->size += growth;
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

	// Close today's price with a change according to demand - supply
	for(size_t i = 0; i < this->products.size(); i++) {
		Product * product = this->products[i];
		if(product->demand > product->supply) {
			product->price_vel += 0.1f;
		} else if(product->demand < product->supply) {
			product->price_vel -= 0.1f;
		} else {
			if(product->price_vel > 0.5f) {
				product->price_vel -= 0.1f;
			} else if(product->price_vel < -0.5f) {
				product->price_vel += 0.1f;
			}
		}
		product->price += product->price_vel;
		if(product->price <= 0.f) {
			product->price = 0.01f;
		}
			
		//printf("%4.f $ - %zu, %zu\n", product->price, product->supply, product->demand);

		// Save prices and stuff onto history (for the charts!)
		product->demand_history.push_back(product->demand);
		if(product->demand_history.size() > 30)
			product->demand_history.pop_front();
		
		product->supply_history.push_back(product->supply);
		if(product->supply_history.size() > 30)
			product->supply_history.pop_front();

		product->price_history.push_back(product->price);
		if(product->price_history.size() > 30)
			product->price_history.pop_front();
			
		// Re-count worldwide supply
		product->supply = 0;
		for(const auto& province: this->provinces) {
			product->supply += province->stockpile[i];
		}
		product->demand = 0;
	}
	
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
