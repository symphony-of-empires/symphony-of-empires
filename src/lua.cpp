#ifdef WIN32
#include <lua.hpp>
#else
#include <lua5.4/lua.hpp>
#endif
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
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
#include "print.hpp"

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
	if(!lua_isstring(L, 1)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Good * good = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->goods.size(); i++) {
		if(g_world->goods[i].ref_name != ref_name) continue;
		good = &g_world->goods[i];
		break;
	} if(good == nullptr) {
		print_error("good %s not found", ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, good->ref_name.c_str());
	lua_pushstring(L, good->name.c_str());
	return 3;
}

int LuaAPI::add_industry_type(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	IndustryType industry = IndustryType();

	industry.ref_name = lua_tostring(L, 1);
	industry.name = lua_tostring(L, 2);
	g_world->industry_types.push_back(industry);

	printf("industry_type: %s (%s)\n", industry.name.c_str(), industry.ref_name.c_str());
	return 0;
}

int LuaAPI::get_industry_type(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error("lua argument type mismatch");
		return 0;
	}
	
	std::string ref_name = lua_tostring(L, 1);
	IndustryType * industry = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->industry_types.size(); i++) {
		if(g_world->industry_types[i].ref_name != ref_name) continue;
		industry = &g_world->industry_types[i];
		break;
	} if(industry == nullptr) {
		print_error("industry %s not found\n", ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, industry->ref_name.c_str());
	lua_pushstring(L, industry->name.c_str());
	return 3;
}

int LuaAPI::add_input_to_industry_type(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	std::string ref_name;
	
	IndustryType * industry = nullptr;
	ref_name = lua_tostring(L, 1);
	const size_t n_industry_types = g_world->industry_types.size();
	for(size_t i = 0; i < n_industry_types; i++) {
		if(ref_name == g_world->industry_types[i].ref_name) continue;
		industry = &g_world->industry_types[i];
		break;
	} if(industry == nullptr) {
		print_error("industry %s not found", ref_name.c_str());
		return 0;
	}

	size_t good_id = (size_t)-1;
	ref_name = lua_tostring(L, 2);
	const size_t n_goods = g_world->goods.size();
	for(size_t i = 0; i < n_goods; i++) {
		if(ref_name == g_world->goods[i].ref_name) continue;
		good_id = i;
		break;
	} if(good_id == (size_t)-1) {
		print_error("good %s not found", ref_name.c_str());
		return 0;
	}
	industry->inputs.push_back(good_id);
	return 0;
}

int LuaAPI::add_output_to_industry_type(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	std::string ref_name;

	IndustryType * industry = nullptr;
	ref_name = lua_tostring(L, 1);
	const size_t n_industry_types = g_world->industry_types.size();
	for(size_t i = 0; i < n_industry_types; i++) {
		if(ref_name != g_world->industry_types[i].ref_name) continue;
		industry = &g_world->industry_types[i];
		break;
	} if(industry == nullptr) {
		print_error("industry %s not found", ref_name.c_str());
		return 0;
	}

	size_t good_id = (size_t)-1;
	ref_name = lua_tostring(L, 2);
	const size_t n_goods = g_world->goods.size();
	for(size_t i = 0; i < n_goods; i++) {
		if(ref_name != g_world->goods[i].ref_name) continue;
		good_id = i;
		break;
	} if(good_id == (size_t)-1) {
		print_error("good %s not found", ref_name.c_str());
		return 0;
	}
	industry->outputs.push_back(good_id);
	return 0;
}

int LuaAPI::add_nation(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) || !lua_isstring(L, 4)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	Nation * nation = new Nation();

	nation->ref_name = lua_tostring(L, 1);

	nation->color = lua_tonumber(L, 2);
	nation->color = bswap_32(nation->color);
	nation->color >>= 8;
	nation->color |= 0xff000000;

	const char * default_flag = lua_tostring(L, 3);
	nation->default_flag.from_file(default_flag);

	nation->name = lua_tostring(L, 4);

	printf("nation: %s (%s)\n", nation->name.c_str(), nation->ref_name.c_str());
	g_world->nations.push_back(*nation);
	return 0;
}

int LuaAPI::get_nation(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Nation * nation = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->nations.size(); i++) {
		if(g_world->nations[i].ref_name != ref_name) continue;
		nation = &g_world->nations[i];
		break;
	} if(nation == nullptr) {
		print_error("nation %s not found\n", ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, nation->ref_name.c_str());
	lua_pushstring(L, nation->name.c_str());
	lua_pushnumber(L, nation->color);
	return 4;
}

int LuaAPI::add_province(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3)) {
		print_error("lua argument type mismatch");
		return 0;
	}

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
	if(!lua_isstring(L, 1)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Province * province = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->provinces.size(); i++) {
		if(g_world->provinces[i].ref_name != ref_name) continue;
		province = &g_world->provinces[i];
		break;
	} if(province == nullptr) {
		print_error("province %s not found\n", ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, province->ref_name.c_str());
	lua_pushstring(L, province->name.c_str());
	lua_pushnumber(L, province->color);
	return 4;
}

int LuaAPI::give_province_to(lua_State * L) {
	if(!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	size_t province_id = lua_tonumber(L, 1);
	Province * province = &g_world->provinces[province_id];
	size_t nation_id = lua_tonumber(L, 2);

	for(size_t i = province->min_x; i < province->max_x; i++) {
		for(size_t j = province->min_y; j < province->max_y; j++) {
			Tile * tile = &g_world->tiles[i + j * g_world->width];
			if(tile->province_id != province_id)
				continue;
			tile->owner_id = nation_id;
		}
	}
	return 0;
}

int LuaAPI::add_province_pop(lua_State * L) {
	if(!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)
	|| !lua_isnumber(L, 4)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	size_t province_id = lua_tonumber(L, 1);
	Province * province = &g_world->provinces[province_id];
	size_t pop_type_id = lua_tonumber(L, 2);
	size_t culture_id = lua_tonumber(L, 3);
	size_t religion_id = lua_tonumber(L, 4);

	Pop pop;
	pop.type_id = pop_type_id;
	pop.culture_id = culture_id;
	pop.religion_id = religion_id;
	return 0;
}

int LuaAPI::add_company(lua_State * L) {
	// TODO: We need to validate this
	//if(!lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isboolean(L, 3)
	//|| !lua_isboolean(L, 4) || !lua_isboolean(L, 5)) {
	if(!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
		print_error("lua argument type mismatch");
		return 0;
	}

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
	if(!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	size_t idx = lua_tonumber(L, 1);
	std::string ref_name = lua_tostring(L, 2);
	for(size_t i = 0; i < g_world->provinces.size(); i++) {
		if(g_world->provinces[i].ref_name != ref_name)
			continue;
		g_world->companies[idx].operating_provinces.push_back(i);
		break;
	}
	return 0;
}

int LuaAPI::add_event(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	Event event;

	event.ref_name = lua_tostring(L, 1);
	event.conditions_function = lua_tostring(L, 2);
	event.do_event_function = lua_tostring(L, 3);

	printf("event: %s\n", event.ref_name.c_str());

	// Add onto vector
	g_world->events.push_back(event);
	return 0;
}

int LuaAPI::add_pop_type(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	PopType pop;

	pop.ref_name = lua_tostring(L, 1);
	pop.name = lua_tostring(L, 2);
	pop.on_tick_fn = lua_tostring(L, 3);

	printf("pop_type: %s\n", pop.ref_name.c_str());

	// Add onto vector
	g_world->pop_types.push_back(pop);
	return 0;
}

int LuaAPI::add_culture(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	Culture culture;

	culture.ref_name = lua_tostring(L, 1);
	culture.name = lua_tostring(L, 2);

	printf("culture: %s\n", culture.ref_name.c_str());
	g_world->cultures.push_back(culture);
	return 0;
}

int LuaAPI::add_religion(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	Religion religion;

	religion.ref_name = lua_tostring(L, 1);
	religion.name = lua_tostring(L, 2);

	printf("religion: %s\n", religion.ref_name.c_str());
	g_world->religions.push_back(religion);
	return 0;
}

int LuaAPI::get_hour(lua_State * L) {
	lua_pushnumber(L, g_world->time % 24);
	return 1;
}

int LuaAPI::get_day(lua_State * L) {
	lua_pushnumber(L, (g_world->time / 24) % 30);
	return 1;
}

int LuaAPI::get_month(lua_State * L) {
	lua_pushnumber(L, ((g_world->time / 24) / 30) % 12);
	return 1;
}

int LuaAPI::get_year(lua_State * L) {
	lua_pushnumber(L, ((g_world->time / 24) / 30) / 12);
	return 1;
}
//g_events

// Checks all events and their condition functions
void LuaAPI::check_events(lua_State * L) {
	// Because of the logic of this loop, only 1 event can happen in the world per tick
	// This is on purpouse ;)
	for(size_t i = 0; i < g_world->events.size(); i++) {
		Event * event = &g_world->events[i];
		lua_getglobal(L, event->conditions_function.c_str());
		lua_call(L, 0, 1);
		int r = lua_tointeger(L, -1);
		lua_pop(L, 1);

		// Conditions met
		if(r) {
			lua_getglobal(L, event->do_event_function.c_str());
			lua_call(L, 0, 1);
			int multi = lua_tointeger(L, -1);
			lua_pop(L, 1);

			// Event is removed if it's not of multiple occurences
			if(!multi) {
				g_world->events.erase(g_world->events.begin() + i);
				break;
			}
		}
		// Conditions not met, continue to next event...
	}
}

#include <libintl.h>
#include <locale.h>
int LuaAPI::get_text(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error("lua argument type mismatch");
		return 0;
	}

	std::string msgid = lua_tostring(L, 1);
	std::string end_msg = gettext(msgid.c_str());
	lua_pushstring(L, end_msg.c_str());
	return 1;
}
