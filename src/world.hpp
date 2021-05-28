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
#include "unit.hpp"

class OrderGoods {
public:
	float payment;
	size_t good_id;
	size_t requester_industry_id;
	ProvinceId requester_province_id;
	size_t quantity;
};
class DeliverGoods {
public:
	int rejections = 0;
	float payment;
	size_t good_id;
	size_t sender_industry_id;
	ProvinceId sender_province_id;
	size_t quantity;
	size_t product_id;
};

class CommercialConvoy {
public:
	std::vector<Tile *> path;
	OrderGoods * order;
	DeliverGoods * deliver;
};

#include <algorithm>
#include "event.hpp"
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
	std::vector<Event *> events;
	lua_State * lua;

	std::vector<OrderGoods> orders;
	std::vector<DeliverGoods> delivers;
	std::vector<CommercialConvoy> convoys;

	uint64_t time;

	NationId get_nation_id(Nation * nation) {
		std::vector<Nation *>::iterator it = std::find(this->nations.begin(), this->nations.end(), nation);
		return (NationId)std::distance(this->nations.begin(), it);
	}
	ProvinceId get_province_id(Province * province);

	std::vector<Tile *> nation_changed_tiles;
	std::vector<Tile *> elevation_changed_tiles;
};

extern World * g_world;

#endif
