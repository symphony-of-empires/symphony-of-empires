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
	Good good;
	memset(&good, 0, sizeof(Good));

	const char * ref_name = lua_tostring(L, 1);
	good.ref_name = (char *)malloc(strlen(ref_name) + 1);
	strcpy(good.ref_name, ref_name);

	const char * name = lua_tostring(L, 2);

	good.name = (char *)malloc(strlen(name) + 1);
	strcpy(good.name, name);

	World_AddGood(g_world, &good);
	return 0;
}

int World_LuaAddIndustryType(lua_State * L) {
	IndustryType industry;
	memset(&industry, 0, sizeof(IndustryType));

	const char * ref_name = lua_tostring(L, 1);
	industry.ref_name = (char *)malloc(strlen(ref_name) + 1);
	strcpy(industry.ref_name, ref_name);

	const char * name = lua_tostring(L, 2);
	industry.name = (char *)malloc(strlen(name) + 1);
	strcpy(industry.name, name);

	World_AddIndustryType(g_world, &industry);
	return 0;
}

int World_LuaAddInputToIndustryType(lua_State * L) {
	const char * ref_name;
	
	IndustryType * industry = NULL;
	ref_name = lua_tostring(L, 1);
	for(size_t i = 0; i < g_world->n_industry_types; i++) {
		if(strcmp(ref_name, g_world->industry_types[i].ref_name)) continue;
		industry = &g_world->industry_types[i];
	} if(industry == NULL) {
		fprintf(stderr, "industry not found %s\n", ref_name);
		return 0;
	}

	size_t good_id = (size_t)-1;
	ref_name = lua_tostring(L, 2);
	for(size_t i = 0; i < g_world->n_goods; i++) {
		if(strcmp(ref_name, g_world->goods[i].ref_name)) continue;
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
		if(strcmp(ref_name, g_world->industry_types[i].ref_name)) continue;
		industry = &g_world->industry_types[i];
	} if(industry == NULL) {
		fprintf(stderr, "industry not found %s\n", ref_name);
		return 0;
	}

	size_t good_id = (size_t)-1;
	ref_name = lua_tostring(L, 2);
	for(size_t i = 0; i < g_world->n_goods; i++) {
		if(strcmp(ref_name, g_world->goods[i].ref_name)) continue;
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
	Nation nation;
	memset(&nation, 0, sizeof(Nation));

	const char * ref_name = lua_tostring(L, 1);
	nation.ref_name = (char *)malloc(strlen(ref_name) + 1);
	strcpy(nation.ref_name, ref_name);

	nation.color = lua_tonumber(L, 2);
	nation.color = bswap_32(nation.color);
	nation.color >>= 8;
	nation.color |= 0xff000000;

	const char * default_flag = lua_tostring(L, 3);
	Texture_FromFile(&nation.default_flag, default_flag);

	const char * name = lua_tostring(L, 4);
	nation.name = (char *)malloc(strlen(name) + 1);
	strcpy(nation.name, name);
	
	World_AddNation(g_world, &nation);
	return 0;
}

int World_LuaAddProvince(lua_State * L) {
	Province province;
	memset(&province, 0, sizeof(Province));

	const char * ref_name = lua_tostring(L, 1);
	province.ref_name = (char *)malloc(strlen(ref_name) + 1);
	strcpy(province.ref_name, ref_name);

	province.color = lua_tonumber(L, 2);
	province.color = bswap_32(province.color);
	province.color >>= 8;
	province.color |= 0xff000000;

	const char * name = lua_tostring(L, 3);
	province.name = (char *)malloc(strlen(name) + 1);
	strcpy(province.name, name);

	province.population = 1000;
	province.budget = 500.f;

	World_AddProvince(g_world, &province);

	// TODO: this is NOT good
	Industry industry;
	industry.owner_id = 0;
	industry.type_id = rand() % g_world->n_industry_types;
	Province_AddIndustry(g_world, &g_world->provinces[g_world->n_provinces - 1], &industry);
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
