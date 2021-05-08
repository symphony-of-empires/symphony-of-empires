#ifndef WORLD_H
#define WORLD_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include "nation.hpp"
#include "economy.hpp"

class Tile {
public:
	size_t owner_id;
	size_t province_id;
	uint8_t elevation;
};

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

	Tile * tiles;

	Good * goods;
	size_t n_goods;

	IndustryType * industry_types;
	size_t n_industry_types;

	Nation * nations;
	size_t n_nations;

	std::vector<Province> provinces;
	std::vector<Company> companies;

	uint64_t time;
};

#endif
