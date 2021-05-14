#ifndef WORLD_H
#define WORLD_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include "nation.hpp"
#include "economy.hpp"
#include "lua.hpp"

#define MAX_INFRA_LEVEL 			8
#define MAX_ELEVATION				255
#define RIVER_ELEVATION(a)			a + 1	

class Tile {
public:
	size_t x;
	size_t y;
	uint16_t owner_id;
	uint16_t province_id;
	uint8_t elevation;
	uint8_t infra_level;
	bool is_river;
};

#include <string>
#include "province.hpp"

class UnitType {
public:
	std::string name;
	std::string ref_name;
	float supply_consumption;
	float speed;
	
};

class Unit {
public:
	float x;
	float y;
	size_t tx;
	size_t ty;
	float health;
	uint16_t owner_id;
};

class World {
public:
	World() {};
	World(const char * topo_map, const char * pol_map, const char * div_map, const char * infra_map);
	World& operator=(const World&) = default;
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

	std::vector<Unit *> units;
	std::vector<Good *> goods;
	std::vector<IndustryType *> industry_types;
	std::vector<Nation *> nations;
	std::vector<Province *> provinces;
	std::vector<Company *> companies;
	std::vector<Product *> products;
	std::vector<PopType *> pop_types;
	std::vector<Religion *> religions;
	std::vector<Culture *> cultures;
	std::vector<LuaAPI::Event *> events;
	lua_State * lua;

	uint64_t time;
};

#endif
