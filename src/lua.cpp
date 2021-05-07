#include <lua5.2/lua.hpp>
#include <string.h>
#include <stdlib.h>
#include <byteswap.h>
#include "lua.hpp"
#include "world.hpp"
#include "nation.hpp"
#include "economy.hpp"

extern World * g_world;

int World_LuaAddGood(lua_State * L) {
	Good * good = new Good;
	memset(good, 0, sizeof(Good));

	good->ref_name = lua_tostring(L, 1);
	good->name = lua_tostring(L, 2);

	g_world->add_good(good);
	return 0;
}

int World_LuaAddIndustryType(lua_State * L) {
	IndustryType * industry = new IndustryType;
	memset(industry, 0, sizeof(IndustryType));

	industry->ref_name = lua_tostring(L, 1);
	industry->name = lua_tostring(L, 2);

	g_world->add_industry_type(industry);
	return 0;
}

int World_LuaAddInputToIndustryType(lua_State * L) {
	const char * ref_name;
	
	IndustryType * industry = NULL;
	ref_name = lua_tostring(L, 1);
	for(size_t i = 0; i < g_world->n_industry_types; i++) {
		if(ref_name == g_world->industry_types[i].ref_name) continue;
		industry = &g_world->industry_types[i];
	} if(industry == NULL) {
		fprintf(stderr, "industry not found %s\n", ref_name);
		return 0;
	}

	size_t good_id = (size_t)-1;
	ref_name = lua_tostring(L, 2);
	for(size_t i = 0; i < g_world->n_goods; i++) {
		if(ref_name == g_world->goods[i].ref_name) continue;
		good_id = i;
	} if(good_id == (size_t)-1) {
		fprintf(stderr, "good not found %s\n", ref_name);
		return 0;
	}

	industry->inputs = (size_t *)realloc(industry->inputs, sizeof(size_t) * (industry->n_inputs + 1));
	industry->inputs[industry->n_inputs] = good_id;
	industry->n_inputs++;
	return 0;
}

int World_LuaAddOutputToIndustryType(lua_State * L) {
	const char * ref_name;
	
	IndustryType * industry = NULL;
	ref_name = lua_tostring(L, 1);
	for(size_t i = 0; i < g_world->n_industry_types; i++) {
		if(ref_name == g_world->industry_types[i].ref_name) continue;
		industry = &g_world->industry_types[i];
	} if(industry == NULL) {
		fprintf(stderr, "industry not found %s\n", ref_name);
		return 0;
	}

	size_t good_id = (size_t)-1;
	ref_name = lua_tostring(L, 2);
	for(size_t i = 0; i < g_world->n_goods; i++) {
		if(ref_name == g_world->goods[i].ref_name) continue;
		good_id = i;	
	} if(good_id == (size_t)-1) {
		fprintf(stderr, "good not found %s\n", ref_name);
		return 0;
	}
	
	industry->outputs = (size_t *)realloc(industry->outputs, sizeof(size_t) * (industry->n_outputs + 1));
	industry->outputs[industry->n_outputs] = good_id;
	industry->n_outputs++;
	return 0;
}

int World_LuaAddNation(lua_State * L) {
	Nation * nation = new Nation;
	memset(nation, 0, sizeof(Nation));

	nation->ref_name = lua_tostring(L, 1);

	nation->color = lua_tonumber(L, 2);
	nation->color = bswap_32(nation->color);
	nation->color >>= 8;
	nation->color |= 0xff000000;

	const char * default_flag = lua_tostring(L, 3);
	nation->default_flag.from_file(default_flag);

	nation->name = lua_tostring(L, 4);
	g_world->add_nation(nation);
	return 0;
}

int World_LuaAddProvince(lua_State * L) {
	Province * province = new Province;
	memset(province, 0, sizeof(Province));

	province->ref_name = lua_tostring(L, 1);
	province->color = lua_tonumber(L, 2);
	province->color = bswap_32(province->color);
	province->color >>= 8;
	province->color |= 0xff000000;

	province->name = lua_tostring(L, 3);
	province->population = 1000;
	province->budget = 500.f;

	g_world->add_province(province);

	// TODO: this is NOT good
	Industry industry;
	industry.owner_id = 0;
	industry.type_id = rand() % g_world->n_industry_types;
	Province_AddIndustry(g_world, &g_world->provinces.back(), &industry);
	return 0;
}

#include <libintl.h>
#include <locale.h>
int World_LuaGettext(lua_State * L) {
	const char * msgid = lua_tostring(L, 1);
	char * end_msg = gettext(msgid);
	lua_pushstring(L, end_msg);
	return 1;
}
