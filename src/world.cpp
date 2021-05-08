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
	
	Texture * topo = new Texture();
	topo->from_file(topo_map);

	Texture * pol = new Texture();
	pol->from_file(pol_map);

	Texture * div = new Texture();
	div->from_file(div_map);

	this->width = topo->width;
	this->height = topo->height;

	this->nations.clear();
	this->nations.reserve(4096);
	this->goods.clear();
	this->goods.reserve(4096);
	this->provinces.clear();
	this->provinces.reserve(4096);
	this->industry_types.clear();
	this->industry_types.reserve(4096);

	if(topo->width != this->width || topo->height != this->height
	|| pol->width != this->width || pol->height != this->height
	|| div->width != this->width || div->height != this->height) {
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
	luaL_dofile(L, Resource_GetPath("scripts/mod.lua").c_str());
	lua_close(L);

	// Translate all div, pol and topo maps onto this single tile array
	for(size_t i = 0; i < this->width * this->height; i++) {
		this->tiles[i].elevation = topo->buffer[i] & 0xff;
		
		// Associate tiles with nations
		this->tiles[i].owner_id = (size_t)-1;
		size_t n_nations = this->nations.size();
		for(size_t j = 0; j < n_nations; j++) {
			if(pol->buffer[i] == this->nations[j].color) {
				this->tiles[i].owner_id = j;
				break;
			}
		}

		// Associate tiles with provinces
		this->tiles[i].province_id = (size_t)-1;
		uint n_provinces = provinces.size();
		for(size_t j = 0; j < n_provinces; j++) {
			if(div->buffer[i] == this->provinces[j].color) {
				this->tiles[i].province_id = j;
				break;
			}
		}
	}
	return;
}

void World::do_tick() {
	size_t n_provinces = this->provinces.size();
	size_t n_companies = this->companies.size();
	for(size_t i = 0; i < n_provinces; i++) {
		Province * province = &this->provinces[i];
		province->population += rand() % 5;

		// RGO and primary industries are going to produce stuff from thin air
		size_t n_industries = province->industries.size();
		for(size_t j = 0; j < n_industries; j++) {
			Industry * industry = &province->industries[j];

			// Only primary factories can produce
			if(this->industry_types[industry->type_id].inputs.size()) continue;

			// Add supply to products
			for(auto& product: province->products) {
				if(product.industry_id != j) continue;

				// TODO: Whoever is working on pops do something dynamic here
				size_t produced = 150;
				size_t original_produced = produced; // Used to calculate income

				product.supply += produced;

				// We will then contact any friendly transport company to transport our product
				for(size_t k = 0; k < n_companies; k++) {
					Company * company = &this->companies[k];

					// We won't accept to transport if we have bad relations and we
					// can't give stuff to companies who dont transport
					if(company->is_transport == false || company->relations[industry->owner_id] < 25.f) continue;

					// Check that the company we are contacting is operating in our same province
					size_t n_op_provinces = company->operating_provinces.size();
					bool in_range = false;
					for(size_t l = 0; l < n_op_provinces; l++) {
						if(company->operating_provinces[l] == i) {
							in_range = true;
							break;
						}
					}
					// We can't transport in companies who are not in our province, fuck
					if(!in_range) continue;

					// The transport company accepts!, now "they" are looking for customers to send to
					for(size_t l = 0; l < n_op_provinces; l++) {
						// Look on all of our operating provinces
						Province * op_province = &this->provinces[company->operating_provinces[l]];

						// Look for industries that accepts this input :)
						for(auto& accept_industry: op_province->industries) {
							// Check if any of it's inputs accepts our product
							bool is_accept = false;
							size_t idx = 0;
							for(const auto& input: this->industry_types[accept_industry.type_id].inputs) {
								idx++;
								if(input == product.good_id) {
									is_accept = true;
									break;
								}
							}
							if(!is_accept) continue;

							// Give them some of our shit (just a part of it, as many as we choose)
							size_t given = produced / ((rand() % produced) + 1);
							accept_industry.stockpile[idx] += given;
							produced -= given;
							if(!produced) break;
						}
						if(!produced) break;
					}

					// Company wins by number of sold units (75%)
					this->companies[industry->owner_id].money += ((original_produced * product.price) / 4) * 3;

					// Transporting company wins by number of sold units (25%)
					company->money += (original_produced * product.price) / 4;
				}
			}
		}

		// Secondary industries, produce stuff that will be later given to retailers
		for(size_t j = 0; j < n_industries; j++) {
			Industry * industry = &province->industries[j];

			// Only factories that accepts stuff
			if(!this->industry_types[industry->type_id].inputs.size()) continue;

			// Add supply to products
			for(auto& product: province->products) {
				if(product.industry_id != j) continue;

				// TODO: Whoever is working on pops do something dynamic here
				size_t produced = 15000;

				product.supply += produced;

				// We will ... again, contact a transport company
				for(size_t k = 0; k < n_companies; k++) {
					Company * company = &this->companies[k];

					// We won't accept to transport if we have bad relations and we
					// can't give stuff to companies who dont transport
					if(company->is_transport == false || company->relations[industry->owner_id] < 50.f) continue;

					// Check that the company we are contacting is operating in our same province
					size_t n_op_provinces = company->operating_provinces.size();
					bool in_range = false;
					for(size_t l = 0; l < n_op_provinces; l++) {
						if(company->operating_provinces[l] == i) {
							in_range = true;
							break;
						}
					}
					// We can't transport in companies who are not in our province, fuck
					if(!in_range) continue;

					// The transport company accepts!, now "they" are looking for customers to send to
					// (company retailers)

					// TODO: This is a placeholder, REPLACE APPROPRIATELY!
					this->companies[industry->owner_id].money += produced * product.price;
					company->money += produced * product.price;
				}
			}
		}

		// TODO: Replace below code to fit the code above (company economics)
		for(size_t j = 0; j < province->industries.size(); j++) {
			Industry * industry = &province->industries[j];

			// Factories generate 1000 jobs, each paying about 1$
			province->budget += (1000.f) / province->population;
		}
		uint n_products = province->products.size();
		for(size_t j = 0; j < n_products; j++) {
			Product * product = &province->products[j];

			product->demand = 0;

			/* Consumers consume products */
			
			/* 25% of our budget goes to spending in this product (and a random dice roll)
			* we also assign a mini-group of buyers who wants to buy these products */
			size_t buyers = ((rand() % (province->population - 1)) + 1);
			if(province->budget / 4 >= product->price) {
				/* Some small group of buyers cannot take */
				province->budget -= product->price * ((float)buyers / (float)province->population);
				product->demand += buyers;
				product->supply -= buyers % product->supply;
			}

			/* The prices of products change volatily */
			/* More demand == more cost
			* less demand == less cost */
			if(product->demand > product->supply) {
				product->price_vel += 0.2f;
			} else if(product->demand < product->supply) {
				product->price_vel -= 0.2f;
			} else {
				/* Gradually slow down the inflation / devaluation of a product */
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
