#include <lua5.4/lua.h>
#ifdef WINDOWS
#include <lua.hpp>
#else
#include <lua5.4/lua.hpp>
#endif //WINDOWS
#include <string.h>
#include <stdlib.h>

#ifdef WINDOWS
#include <stdlib.h>
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)
#else
#include <byteswap.h>
#endif
#include "lua.hpp"
#include "world.hpp"
#include "nation.hpp"
#include "economy.hpp"

// Global world - do not use too much!
extern World * g_world;

int LuaAPI::add_good(lua_State * L) {
	Good good = Good();

	good.ref_name = lua_tostring(L, 1);
	good.name = lua_tostring(L, 2);
	g_world->goods.push_back(good);

	printf("good: %s (%s)\n", good.name.c_str(), good.ref_name.c_str());
	return 0;
}

int LuaAPI::get_good(lua_State * L) {
	std::string ref_name = lua_tostring(L, 1);
	Good * good = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->goods.size(); i++) {
		if(g_world->goods[i].ref_name != ref_name) continue;
		good = &g_world->goods[i];
		break;
	} if(good == nullptr) {
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, good->ref_name.c_str());
	lua_pushstring(L, good->name.c_str());
	return 3;
}

int LuaAPI::add_industry_type(lua_State * L) {
	IndustryType industry = IndustryType();

	industry.ref_name = lua_tostring(L, 1);
	industry.name = lua_tostring(L, 2);
	g_world->industry_types.push_back(industry);

	printf("industry_type: %s (%s)\n", industry.name.c_str(), industry.ref_name.c_str());
	return 0;
}

int LuaAPI::get_industry_type(lua_State * L) {
	std::string ref_name = lua_tostring(L, 1);
	IndustryType * industry = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->industry_types.size(); i++) {
		if(g_world->industry_types[i].ref_name != ref_name) continue;
		industry = &g_world->industry_types[i];
		break;
	} if(industry == nullptr) {
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, industry->ref_name.c_str());
	lua_pushstring(L, industry->name.c_str());
	return 3;
}

int LuaAPI::add_input_to_industry_type(lua_State * L) {
	std::string ref_name;
	
	IndustryType * industry = nullptr;
	ref_name = lua_tostring(L, 1);
	size_t n_industry_types = g_world->industry_types.size();
	for(size_t i = 0; i < n_industry_types; i++) {
		if(ref_name == g_world->industry_types[i].ref_name) continue;
		industry = &g_world->industry_types[i];
		break;
	} if(industry == nullptr) {
		fprintf(stderr, "industry not found %s\n", ref_name.c_str());
		return 0;
	}

	size_t good_id = (size_t)-1;
	ref_name = lua_tostring(L, 2);
	size_t n_goods = g_world->goods.size();
	for(size_t i = 0; i < n_goods; i++) {
		if(ref_name == g_world->goods[i].ref_name) continue;
		good_id = i;
		break;
	} if(good_id == (size_t)-1) {
		fprintf(stderr, "good not found %s\n", ref_name.c_str());
		return 0;
	}
	industry->inputs.push_back(good_id);
	return 0;
}

int LuaAPI::add_output_to_industry_type(lua_State * L) {
	std::string ref_name;

	IndustryType * industry = nullptr;
	ref_name = lua_tostring(L, 1);
	size_t n_industry_types = g_world->industry_types.size();
	for(size_t i = 0; i < n_industry_types; i++) {
		if(ref_name != g_world->industry_types[i].ref_name) continue;
		industry = &g_world->industry_types[i];
		break;
	}
	if(industry == nullptr) {
		fprintf(stderr, "industry not found %s\n", ref_name.c_str());
		return 0;
	}

	size_t good_id = (size_t)-1;
	ref_name = lua_tostring(L, 2);
	size_t n_goods = g_world->goods.size();
	for(size_t i = 0; i < n_goods; i++) {
		if(ref_name != g_world->goods[i].ref_name) continue;
		good_id = i;
		break;
	} if(good_id == (size_t)-1) {
		fprintf(stderr, "good not found %s\n", ref_name.c_str());
		return 0;
	}
	industry->outputs.push_back(good_id);
	return 0;
}

int LuaAPI::add_nation(lua_State * L) {
	Nation * nation = new Nation();

	nation->ref_name = lua_tostring(L, 1);

	nation->color = lua_tonumber(L, 2);
	nation->color = bswap_32(nation->color);
	nation->color >>= 8;
	nation->color |= 0xff000000;

	const char * default_flag = lua_tostring(L, 3);
	nation->default_flag.from_file(default_flag);

	nation->name = lua_tostring(L, 4);
	g_world->nations.push_back(*nation);
	printf("nation: %s (%s)\n", nation->name.c_str(), nation->ref_name.c_str());
	return 0;
}

int LuaAPI::get_nation(lua_State * L) {
	std::string ref_name = lua_tostring(L, 1);
	Nation * nation = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->nations.size(); i++) {
		if(g_world->nations[i].ref_name != ref_name) continue;
		nation = &g_world->nations[i];
		break;
	} if(nation == nullptr) {
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, nation->ref_name.c_str());
	lua_pushstring(L, nation->name.c_str());
	lua_pushnumber(L, nation->color);
	return 4;
}

int LuaAPI::add_province(lua_State * L) {
	Province province = Province();

	province.ref_name = lua_tostring(L, 1);
	province.color = lua_tonumber(L, 2);
	province.color = bswap_32(province.color);
	province.color >>= 8;
	province.color |= 0xff000000;

	province.name = lua_tostring(L, 3);
	province.population = 1000;
	province.budget = 500.f;

	g_world->provinces.push_back(province);

	// TODO: this is NOT good
	Industry industry;
	if(g_world->companies.size()) {
		industry.owner_id = rand() % g_world->companies.size();
	} else {
		industry.owner_id = 0;
	}
	industry.type_id = rand() % g_world->industry_types.size();
	industry.stockpile = std::vector<size_t>(g_world->industry_types[industry.type_id].inputs.size());
	for(auto& stockpile: industry.stockpile) {
		stockpile = 0;
	}
	g_world->provinces.back().add_industry(g_world, &industry);
	return 0;
}

int LuaAPI::get_province(lua_State * L) {
	std::string ref_name = lua_tostring(L, 1);
	Province * province = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->provinces.size(); i++) {
		if(g_world->provinces[i].ref_name != ref_name) continue;
		province = &g_world->provinces[i];
		break;
	} if(province == nullptr) {
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, province->ref_name.c_str());
	lua_pushstring(L, province->name.c_str());
	lua_pushnumber(L, province->color);
	return 4;
}

int LuaAPI::give_province_to(lua_State * L) {
	size_t province_id = lua_tonumber(L, 1);
	Province * province = &g_world->provinces[province_id];
	size_t nation_id = lua_tonumber(L, 2);

	World * world = g_world;
	//for(size_t i = 0; i < world->width; i++) {
	//	for(size_t j = 0; j < world->height; j++) {
	printf("%zu, %zu -> %zu, %zu\n", province->min_x, province->min_y, province->max_x, province->max_y);
	for(size_t i = province->min_x; i < province->max_x; i++) {
		for(size_t j = province->min_y; j < province->max_y; j++) {
			Tile * tile = &world->tiles[i + j * world->width];
			if(tile->province_id != province_id) continue;
			tile->owner_id = nation_id;
			putchar('.');
		}
	}
	printf("\n");
	return 0;
}

int LuaAPI::add_company(lua_State * L) {
	Company company = Company();

	company.name = lua_tostring(L, 1);
	company.money = lua_tonumber(L, 2);
	company.is_transport = lua_toboolean(L, 3);
	company.is_retailer = lua_toboolean(L, 4);
	company.is_industry = lua_toboolean(L, 5);

	company.operating_provinces.clear();

	// Add onto vector
	g_world->companies.push_back(company);

	printf("company: %s\n", company.name.c_str());
	return 0;
}

int LuaAPI::add_op_province_to_company(lua_State * L) {
	size_t idx = lua_tonumber(L, 1);
	std::string ref_name = lua_tostring(L, 2);
	for(size_t i = 0; i < g_world->provinces.size(); i++) {
		if(g_world->provinces[i].ref_name != ref_name) continue;
		g_world->companies[idx].operating_provinces.push_back(i);
		break;
	}
	return 0;
}

#include <libintl.h>
#include <locale.h>
int LuaAPI::get_text(lua_State * L) {
	std::string msgid = lua_tostring(L, 1);
	std::string end_msg = gettext(msgid.c_str());
	lua_pushstring(L, end_msg.c_str());
	return 1;
}
