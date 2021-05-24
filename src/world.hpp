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
	float max_health;
	float defense;
	float attack;
};

class Unit {
public:
	uint8_t type_id;
	float x;
	float y;
	float tx;
	float ty;
	float health;
	uint16_t owner_id;
};

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

class World {
public:
	World();
	World& operator=(const World&) = default;
	~World();

	void do_economy_tick_1();
	void do_economy_tick_2();
	void do_economy_tick_3();
	void do_economy_tick_4();
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
	std::vector<UnitType *> unit_types;
	std::vector<LuaAPI::Event *> events;
	lua_State * lua;

	std::vector<OrderGoods> orders;
	std::vector<DeliverGoods> delivers;

	uint64_t time;

	NationId get_nation_id(Nation * nation);
	ProvinceId get_province_id(Province * province);
};

#endif
