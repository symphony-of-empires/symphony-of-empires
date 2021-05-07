#include <byteswap.h>
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
	memset(this, 0, sizeof(World));

	Texture * topo = new Texture();
	topo->from_file(topo_map);

	Texture * pol = new Texture();
	pol->from_file(pol_map);

	Texture * div = new Texture();
	div->from_file(div_map);

	this->width = topo->width;
	this->height = topo->height;

	if(topo->width != this->width || topo->height != this->height
	|| pol->width != this->width || pol->height != this->height
	|| div->width != this->width || div->height != this->height) {
		perror("map size mismatch\n");
		exit(EXIT_FAILURE);
	}

	this->sea_level = 128;
	this->tiles = (World_Tile *)malloc(sizeof(World_Tile) * (this->width * this->height));
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

	/* translate all div, pol and topo maps onto this single tile array */
	for(size_t i = 0; i < this->width * this->height; i++) {
		this->tiles[i].elevation = topo->buffer[i] & 0xff;
		
		/* associate tiles with nations */
		this->tiles[i].owner_id = (size_t)-1;
		for(size_t j = 0; j < this->n_nations; j++) {
			if(pol->buffer[i] == this->nations[j].color) {
				this->tiles[i].owner_id = j;
				break;
			}
		}

		/* associate tiles with provinces */
		this->tiles[i].province_id = (size_t)-1;
		for(size_t j = 0; j < this->n_provinces; j++) {
			if(div->buffer[i] == this->provinces[j].color) {
				this->tiles[i].province_id = j;
				break;
			}
		}
	}
	return;
}

void World::do_tick() {
	for(size_t i = 0; i < this->n_provinces; i++) {
		Province * province = &this->provinces[i];
		province->population += rand() % 5;

		/* Producers produce stuff */
		for(size_t i = 0; i < province->n_industries; i++) {
			Industry * industry = &province->industries[i];

			/* Factories generate 1000 jobs, each paying about 1$ */
			province->budget += (1000.f) / province->population;

			//printf("%s industry in %s:\n", world->industry_types[industry->type_id].ref_name, province->ref_name);
			for(size_t j = 0; j < province->n_products; j++) {
				Product * product = &province->products[j];
				if(product->industry_id != i) {
					continue;
				}

				/* Always generate supplies */
				product->supply += 100;

				//printf("\t\t%zu product: %zu, %zu\n", j, product->supply, product->demand);
			}
		}

		for(size_t i = 0; i < province->n_products; i++) {
			Product * product = &province->products[i];

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
	this->goods = (Good *)realloc(this->goods, sizeof(Good) * (this->n_goods + 1));
	memcpy(&this->goods[this->n_goods], good, sizeof(Good));
	this->n_goods++;
}

void World::add_industry_type(IndustryType * it) {
	this->industry_types = (IndustryType *)realloc(this->industry_types, sizeof(IndustryType) * (this->n_industry_types + 1));
	memcpy(&this->industry_types[this->n_industry_types], it, sizeof(IndustryType));
	this->n_industry_types++;
}

void World::add_nation(Nation * nation) {
	this->nations = (Nation *)realloc(this->nations, sizeof(Nation) * (this->n_nations + 1));
	memcpy(&this->nations[this->n_nations], nation, sizeof(Nation));
	this->n_nations++;
}

void World::add_province(Province * province) {
	this->provinces = (Province *)realloc(this->provinces, sizeof(Province) * (this->n_provinces + 1));
	memcpy(&this->provinces[this->n_provinces], province, sizeof(Province));
	this->n_provinces++;
}
