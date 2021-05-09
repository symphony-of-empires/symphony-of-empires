#include <stdio.h>
#include <sys/cdefs.h>
#include "economy.hpp"
#include "world.hpp"
#include "texture.hpp"
#include "lua.hpp"
#include "path.hpp"

// Mostly used by clients and lua API
World * g_world;

World::World(const char * topo_map, const char * pol_map, const char * div_map, const char * infra_map) {
	g_world = this;
	
	Texture topo(topo_map);
	Texture pol(pol_map);
	Texture div(div_map);
	Texture infra(infra_map);

	this->width = topo.width;
	this->height = topo.height;

	if(topo.width != this->width || topo.height != this->height
	|| pol.width != this->width || pol.height != this->height
	|| div.width != this->width || div.height != this->height) {
		perror("map size mismatch\n");
		exit(EXIT_FAILURE);
	}

	this->sea_level = 128;
	this->tiles = new Tile[this->width * this->height];
	if(this->tiles == nullptr) {
		perror("out of mem\n");
		exit(EXIT_FAILURE);
	}

	this->lua = luaL_newstate();
	luaL_openlibs(this->lua);
	
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
	lua_register(this->lua, "give_province_to", LuaAPI::give_province_to);
	lua_register(this->lua, "add_company", LuaAPI::add_company);
	lua_register(this->lua, "add_event", LuaAPI::add_event);

	lua_register(this->lua, "get_hour", LuaAPI::get_hour);
	lua_register(this->lua, "get_day", LuaAPI::get_day);
	lua_register(this->lua, "get_month", LuaAPI::get_month);
	lua_register(this->lua, "get_year", LuaAPI::get_year);

	// TODO: The. name. is. fucking. long.
	lua_register(this->lua, "add_op_province_to_company", LuaAPI::add_op_province_to_company);

	luaL_dofile(this->lua, Resource_GetPath("scripts/init.lua").c_str());

	// Translate all div, pol and topo maps onto this single tile array
	const size_t n_nations = this->nations.size();
	const size_t n_provinces = this->provinces.size();
	for(size_t i = 0; i < this->width * this->height; i++) {
		this->tiles[i].elevation = topo.buffer[i] & 0xff;
		
		// Associate tiles with nations
		this->tiles[i].owner_id = (size_t)-1;
		for(size_t j = 0; j < n_nations; j++) {
			if(pol.buffer[i] == this->nations[j].color) {
				this->tiles[i].owner_id = j;
				break;
			}
		}

		// Associate tiles with provinces
		this->tiles[i].province_id = (size_t)-1;
		for(size_t j = 0; j < n_provinces; j++) {
			if(div.buffer[i] == this->provinces[j].color) {
				this->tiles[i].province_id = j;
				break;
			}
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
					Province * province = &this->provinces[k];
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

	for(auto& province: this->provinces) {
		if(province.max_x > this->width)
			province.max_x = this->width;
		if(province.max_y > this->height)
			province.max_y = this->height;
		printf("(%zu, %zu) - (%zu, %zu)\n", province.min_x, province.min_y, province.max_x, province.max_y);
	}

	// Shrink normally-not-resized vectors
	this->provinces.shrink_to_fit();
	this->nations.shrink_to_fit();
	this->goods.shrink_to_fit();
	this->industry_types.shrink_to_fit();

	luaL_dofile(this->lua, Resource_GetPath("scripts/mod.lua").c_str());
}

World::~World() {
	lua_close(this->lua);
	delete[] this->tiles;
}

class OrderGoods {
public:
	float payment;
	size_t good_id;
	size_t requester_industry_id;
	size_t requester_province_id;
};
class DeliverGoods {
public:
	float payment;
	size_t good_id;
	size_t sender_industry_id;
	size_t sender_province_id;
	size_t rejections = 0;
};
void World::do_tick() {
	const size_t n_provinces = this->provinces.size();
	//size_t n_companies = this->companies.size();

	std::vector<OrderGoods> orders;
	std::vector<DeliverGoods> delivers;

	// All factories will place their orders for their inputs
	// All RGOs will do deliver requests
	for(size_t i = 0; i < n_provinces; i++) {
		for(size_t j = 0; j < this->provinces[i].industries.size(); j++) {
			IndustryType * it = &this->industry_types[this->provinces[i].industries[j].type_id];
			for(const auto& input: it->inputs) {
				OrderGoods order;
				order.payment = 500.f;
				order.good_id = input;
				order.requester_industry_id = j;
				order.requester_province_id = i;
				orders.push_back(order);
				//printf("We need good: %s (from %s)\n", this->goods[order.good_id].ref_name.c_str(), this->provinces[order.requester_province_id].ref_name.c_str());
			}

			if(it->inputs.size() == 0) {
				// Place deliver orders (we are a RGO)
				for(const auto& output: it->outputs) {
					DeliverGoods deliver;
					deliver.payment = 1500.f;
					deliver.good_id = output;
					deliver.sender_industry_id = j;
					deliver.sender_province_id = i;
					delivers.push_back(deliver);
					//printf("Throwing RGO: %s (from %s)\n", this->goods[deliver.good_id].ref_name.c_str(), this->provinces[deliver.sender_province_id].ref_name.c_str());
				}
			}
		}
	}

	while(delivers.size() > 0
	|| orders.size() > 0) {
		// Now transport companies will check and transport accordingly
		for(auto& company: this->companies) {
			if(!company.is_transport) continue;

			//printf("Hi, i'm %s\n", company.name.c_str());

			// Check all delivers
			for(size_t i = 0; i < delivers.size(); i++) {
				DeliverGoods * deliver = &delivers[i];

				if(!company.in_range(deliver->sender_province_id)) continue;

				// Check all orders
				for(size_t j = 0; j < orders.size(); j++) {
					OrderGoods * order = &orders[j];
					
					// Do they want this goods?
					if(order->good_id != deliver->good_id) continue;

					// Are we in the range to deliver?
					if(!company.in_range(order->requester_province_id)) continue;

					//printf("Delivered from %s to %s some %s\n", this->provinces[deliver->sender_province_id].ref_name.c_str(), this->provinces[order->requester_province_id].ref_name.c_str(), this->goods[order->good_id].ref_name.c_str());

					// Yes - we go and deliver their stuff
					Industry * industry = &this->provinces[order->requester_province_id].industries[order->requester_industry_id];
					industry->add_to_stock(this, order->good_id, 1);

					// On 0 it overflows to -1, on 1 it goes to zero, but after this loop it's
					// incremented again (see incrementing below). So we don't take a rejection hit
					deliver->rejections--;

					// Delete this deliver and order tickets from the system since
					// they are now fullfilled
					orders.erase(orders.begin() + j); j--;
					delivers.erase(delivers.begin() + i);
					break;
				}
				deliver->rejections++;
			}
		}

		// Get number of transporter companies
		size_t n_transporters = 0;
		for(const auto& company: this->companies) {
			if(company.is_transport) n_transporters++;
		}

		// Drop all rejected delivers
		for(size_t i = 0; i < delivers.size(); i++) {
			DeliverGoods * deliver = &delivers[i];
			if(deliver->rejections >= n_transporters) {
				delivers.erase(delivers.begin() + i);
				i--;
				continue;
			}
		}

		// No orders if no delivers!
		if(delivers.size() == 0) {
			delivers.clear();
			orders.clear();
			break;
		}
	}

	for(auto& product: this->products) {
		if(product.demand > product.supply) {
			product.price_vel += 0.2f;
		} else if(product.demand < product.supply) {
			product.price_vel -= 0.2f;
		} else {
			if(product.price_vel > 0.5f) {
				product.price_vel -= 0.1f;
			} else if(product.price_vel < -0.5f) {
				product.price_vel += 0.1f;
			}
		}
		product.price += product.price_vel;
		if(product.price <= 0.f) {
			product.price = 0.01f;
		}
	}
	this->time++;
}

void World::add_good(Good * good) {
	this->goods.push_back(*good);
}

void World::add_industry_type(IndustryType * it) {
	this->industry_types.push_back(*it);
}

void World::add_nation(Nation * nation) {
	this->nations.push_back(*nation);
}

void World::add_province(Province * province) {
	this->provinces.push_back(*province);
}
