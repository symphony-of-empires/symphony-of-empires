#include <stdio.h>
#include <sys/cdefs.h>
#include "economy.hpp"
#include "world.hpp"
#include "texture.hpp"
#include "lua.hpp"
#include "path.hpp"

World * g_world;

World::World(const char * topo_map, const char * pol_map, const char * div_map) {
	g_world = this;
	
	Texture topo = Texture();
	topo.from_file(topo_map);

	Texture pol = Texture();
	pol.from_file(pol_map);

	Texture div = Texture();
	div.from_file(div_map);

	this->width = topo.width;
	this->height = topo.height;

	this->nations.clear();
	this->nations.reserve(4096);
	this->goods.clear();
	this->goods.reserve(4096);
	this->provinces.clear();
	this->provinces.reserve(4096);
	this->industry_types.clear();
	this->industry_types.reserve(4096);

	if(topo.width != this->width || topo.height != this->height
	|| pol.width != this->width || pol.height != this->height
	|| div.width != this->width || div.height != this->height) {
		perror("map size mismatch\n");
		exit(EXIT_FAILURE);
	}

	this->sea_level = 128;
	this->tiles = (Tile *)malloc(sizeof(Tile) * (this->width * this->height));
	if(this->tiles == NULL) {
		perror("out of mem\n");
		exit(EXIT_FAILURE);
	}

	lua_State * L;
	L = luaL_newstate();
	luaL_openlibs(L);
	lua_register(L, "_", World_LuaGettext);
	lua_register(L, "AddGood", World_LuaAddGood);
	lua_register(L, "AddIndustryType", World_LuaAddIndustryType);
	lua_register(L, "AddInputToIndustryType", World_LuaAddInputToIndustryType);
	lua_register(L, "AddOutputToIndustryType", World_LuaAddOutputToIndustryType);
	lua_register(L, "AddNation", World_LuaAddNation);
	lua_register(L, "AddProvince", World_LuaAddProvince);
	lua_register(L, "AddCompany", World_LuaAddCompany);

	// TODO: The. name. is. fucking. long.
	lua_register(L, "AddOperationalProvinceToCompany", World_LuaAddOperationalProvinceToCompany);

	luaL_dofile(L, Resource_GetPath("scripts/good.lua").c_str());
	lua_setglobal(L, "Good");
	luaL_dofile(L, Resource_GetPath("scripts/industry_type.lua").c_str());
	lua_setglobal(L, "IndustryType");
	luaL_dofile(L, Resource_GetPath("scripts/nation.lua").c_str());
	lua_setglobal(L, "Nation");
	luaL_dofile(L, Resource_GetPath("scripts/province.lua").c_str());
	lua_setglobal(L, "Province");
	luaL_dofile(L, Resource_GetPath("scripts/company.lua").c_str());
	lua_setglobal(L, "Company");
	luaL_dofile(L, Resource_GetPath("scripts/mod.lua").c_str());
	lua_close(L);

	// Translate all div, pol and topo maps onto this single tile array
	for(size_t i = 0; i < this->width * this->height; i++) {
		this->tiles[i].elevation = topo.buffer[i] & 0xff;
		
		// Associate tiles with nations
		this->tiles[i].owner_id = (size_t)-1;
		size_t n_nations = this->nations.size();
		for(size_t j = 0; j < n_nations; j++) {
			if(pol.buffer[i] == this->nations[j].color) {
				this->tiles[i].owner_id = j;
				break;
			}
		}

		// Associate tiles with provinces
		this->tiles[i].province_id = (size_t)-1;
		uint n_provinces = provinces.size();
		for(size_t j = 0; j < n_provinces; j++) {
			if(div.buffer[i] == this->provinces[j].color) {
				this->tiles[i].province_id = j;
				break;
			}
		}
	}
	return;
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
	size_t rejections;
};
void World::do_tick() {
	size_t n_provinces = this->provinces.size();
	size_t n_companies = this->companies.size();

	std::vector<OrderGoods> orders;
	std::vector<DeliverGoods> delivers;

	// All factories will place their orders for their inputs
	// All RGOs will do deliver requests
	for(size_t i = 0; i < this->provinces.size(); i++) {
		for(size_t j = 0; j < this->provinces[i].industries.size(); j++) {
			IndustryType * it = &this->industry_types[this->provinces[i].industries[j].type_id];
			for(const auto& input: it->inputs) {
				OrderGoods order;
				order.payment = 500.f;
				order.good_id = input;
				order.requester_industry_id = j;
				order.requester_province_id = i;
				orders.push_back(order);
				printf("We need good: %s (from %s)\n", this->goods[order.good_id].ref_name.c_str(), this->provinces[order.requester_province_id].ref_name.c_str());
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
					printf("Throwing RGO: %s (from %s)\n", this->goods[deliver.good_id].ref_name.c_str(), this->provinces[deliver.sender_province_id].ref_name.c_str());
				}
			}
		}
	}

	while(delivers.size() > 0
	|| orders.size() > 0) {
		// Now transport companies will check and transport accordingly
		for(auto& company: this->companies) {
			if(!company.is_transport) continue;

			// Check all delivers
			for(size_t i = 0; i < delivers.size(); i++) {
				DeliverGoods * deliver = &delivers[i];

				if(!company.in_range(deliver->sender_province_id)) continue;

				printf("we need to deliver? %s (from %s)\n", this->goods[deliver->good_id].ref_name.c_str(), this->provinces[deliver->sender_province_id].ref_name.c_str());

				// Check all orders
				for(size_t j = 0; j < orders.size(); j++) {
					OrderGoods * order = &orders[j];
					
					// Do they want this goods?
					if(order->good_id != deliver->good_id) continue;

					// Are we in the range to deliver?
					if(!company.in_range(order->requester_province_id)) continue;

					printf("delivered from %s to %s some %s\n", this->provinces[deliver->sender_province_id].ref_name.c_str(), this->provinces[order->requester_province_id].ref_name.c_str(), this->goods[order->good_id].ref_name.c_str());

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
