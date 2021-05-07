#ifndef WORLD_H
#define WORLD_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include "nation.hpp"
#include "economy.hpp"

typedef struct {
	size_t owner_id;
	size_t province_id;
	uint8_t elevation;
}World_Tile;

#include <string>
#include "province.hpp"

class World {
public:
	World(const char * topo_map, const char * pol_map, const char * div_map);
	~World();

	void do_tick();
	void add_good(Good * good);
	void add_industry_type(IndustryType * it);
	void add_nation(Nation * nation);
	void add_province(Province * province);

	size_t sea_level;
	
	size_t width;
	size_t height;

	World_Tile * tiles;

	Good * goods;
	size_t n_goods;

	IndustryType * industry_types;
	size_t n_industry_types;

	Nation * nations;
	size_t n_nations;

	Province * provinces;
	size_t n_provinces;

	uint64_t time;
};

extern World * g_world;

static inline void Province_AddIndustry(World * world, Province * province, Industry * industry) {
	IndustryType * type = &world->industry_types[industry->type_id];
	for(size_t i = 0; i < type->n_outputs; i++) {
		size_t good_id = type->outputs[i];


		/* check that product is not already here */
		int is_here = 0;
		uint n_products = province->products.size();
		for(size_t j = 0; j < n_products; j++) {
			if(province->products[j].owner_id == industry->owner_id) {
				is_here = 1;
				break;
			}
		}
		if(is_here) break;

		/* otherwise add it to the province product list */
		Product new_product;
		memset(&new_product, 0, sizeof(Product));
		new_product.industry_id = province->industries.size();
		new_product.good_id = good_id;
		new_product.owner_id = industry->owner_id;
		province->products.push_back(new_product);
	}

	province->industries.push_back(*industry);
}

void World_DoTick(World * world);
void World_Create(World * world, const char * topo_map, const char * pol_map, const char * div_map);

#endif
