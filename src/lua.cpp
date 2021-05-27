#ifdef WIN32
#	include <lua.hpp>
#else
#	include <lua5.4/lua.hpp>
#endif
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#	include <stdlib.h>
#	define bswap_32(x) _byteswap_ulong(x)
#	define bswap_64(x) _byteswap_uint64(x)
#else
#	include <byteswap.h>
#endif
#include "lua.hpp"
#include "world.hpp"
#include "nation.hpp"
#include "economy.hpp"
#include "print.hpp"

#ifndef SERVER_HEADLESS
#	include "ui.hpp"
#endif

#include <libintl.h>
#include <locale.h>
#include "path.hpp"

// Global world - do not use too much!
extern World * g_world;
extern std::vector<Texture *> g_texture_cache;

int LuaAPI::add_good(lua_State * L) {
	Good * good = new Good();

	good->ref_name = lua_tostring(L, 1);
	good->name = lua_tostring(L, 2);
	good->is_edible = lua_toboolean(L, 3);

	std::string path;
	path = "icons/goods/" + good->ref_name + ".png";
	good->icon = new Texture(Path::get(path.c_str()).c_str());
	g_world->goods.push_back(good);

	printf(gettext("good: %s (%s)\n"), good->name.c_str(), good->ref_name.c_str());

	lua_pushnumber(L, g_world->goods.size() - 1);
	return 1;
}

int LuaAPI::get_good(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Good * good = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->goods.size(); i++) {
		if(g_world->goods[i]->ref_name != ref_name)
			continue;
		
		good = g_world->goods[i];
		break;
	} if(good == nullptr) {
		print_error(gettext("good %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, good->ref_name.c_str());
	lua_pushstring(L, good->name.c_str());
	return 3;
}

int LuaAPI::add_industry_type(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	IndustryType * industry = new IndustryType();

	industry->ref_name = lua_tostring(L, 1);
	industry->name = lua_tostring(L, 2);
	
	std::string path;
	
	path = "images/" + industry->ref_name + ".png";
	industry->image = new Texture(Path::get(path.c_str()).c_str());
	printf("opening %s\n", path.c_str());
	
	industry->inputs.clear();
	industry->outputs.clear();
	g_world->industry_types.push_back(industry);

	printf(gettext("industry_type: %s (%s)"), industry->name.c_str(), industry->ref_name.c_str());
	printf("\n");
	
	lua_pushnumber(L, g_world->industry_types.size() - 1);
	return 1;
}

int LuaAPI::get_industry_type(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}
	
	std::string ref_name = lua_tostring(L, 1);
	IndustryType * industry = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->industry_types.size(); i++) {
		if(g_world->industry_types[i]->ref_name != ref_name)
			continue;
		
		industry = g_world->industry_types[i];
		break;
	} if(industry == nullptr) {
		print_error(gettext("industry %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, industry->ref_name.c_str());
	lua_pushstring(L, industry->name.c_str());
	return 3;
}

int LuaAPI::add_input_to_industry_type(lua_State * L) {
	IndustryType * industry = g_world->industry_types[lua_tonumber(L, 1)];
	size_t good_id = lua_tonumber(L, 2);
	industry->inputs.push_back(good_id);
	return 0;
}

int LuaAPI::add_output_to_industry_type(lua_State * L) {
	IndustryType * industry = g_world->industry_types[lua_tonumber(L, 1)];
	size_t good_id = lua_tonumber(L, 2);
	industry->outputs.push_back(good_id);
	return 0;
}

int LuaAPI::add_nation(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) || !lua_isstring(L, 4)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	Nation * nation = new Nation();

	nation->ref_name = lua_tostring(L, 1);

	nation->color = bswap_32(lua_tonumber(L, 2));
	nation->color >>= 8;
	nation->color |= 0xff000000;

	const char * default_flag = lua_tostring(L, 3);
	nation->default_flag = new Texture(default_flag);

	nation->name = lua_tostring(L, 4);

	// Check for duplicates
	for(size_t i = 0; i < g_world->nations.size(); i++) {
		if(nation->color == g_world->nations[i]->color) {
			print_error(gettext("%s nation has same colour as %s"), nation->name.c_str(), g_world->nations[i]->name.c_str());
			break;
		} else if(nation->ref_name == g_world->nations[i]->ref_name) {
			print_error(gettext("%s nation has same ref_name as %s thereby we will return the id of the original nation"), nation->name.c_str(), g_world->nations[i]->name.c_str());
			lua_pushnumber(L, i);
			return 1;
		}
	}

	printf("nation: %s (%s)\n", nation->name.c_str(), nation->ref_name.c_str());
	g_world->nations.push_back(nation);
	lua_pushnumber(L, g_world->nations.size() - 1);
	return 1;
}

int LuaAPI::get_nation(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Nation * nation = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->nations.size(); i++) {
		if(g_world->nations[i]->ref_name != ref_name) continue;
		nation = g_world->nations[i];
		break;
	} if(nation == nullptr) {
		print_error(gettext("nation %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, nation->ref_name.c_str());
	lua_pushstring(L, nation->name.c_str());
	lua_pushnumber(L, nation->color);
	return 4;
}

int LuaAPI::set_nation_primary_culture(lua_State * L) {
	Nation * nation = g_world->nations[lua_tonumber(L, 1)];
	nation->primary_culture = g_world->cultures[lua_tonumber(L, 2)];
	return 0;
}

int LuaAPI::add_province(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	Province * province = new Province();

	province->ref_name = lua_tostring(L, 1);
	province->color = bswap_32(lua_tonumber(L, 2));
	province->color >>= 8;
	province->color |= 0xff000000;

	province->name = lua_tostring(L, 3);
	province->population = 1000;
	province->budget = 500.f;

	// Check for duplicates
	for(size_t i = 0; i < g_world->provinces.size(); i++) {
		if(province->color == g_world->provinces[i]->color) {
			print_error(gettext("%s province has same colour as %s"), province->name.c_str(), g_world->provinces[i]->name.c_str());
			break;
		} else if(province->ref_name == g_world->provinces[i]->ref_name) {
			print_error(gettext("%s province has same ref_name as %s thereby we will return the id of the original province"), province->name.c_str(), g_world->provinces[i]->name.c_str());
			lua_pushnumber(L, i);
			return 1;
		}
	}

	g_world->provinces.push_back(province);

	// TODO: this is NOT good
	for(size_t i = 0; i < 5; i++) {
		Industry * industry = new Industry();
		industry->owner_id = rand() % g_world->companies.size();
		industry->type_id = rand() % g_world->industry_types.size();
		g_world->provinces.back()->add_industry(g_world, industry);
	}
	lua_pushnumber(L, g_world->provinces.size() - 1);
	return 1;
}

int LuaAPI::get_province(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Province * province = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->provinces.size(); i++) {
		if(g_world->provinces[i]->ref_name != ref_name) continue;
		province = g_world->provinces[i];
		break;
	} if(province == nullptr) {
		print_error(gettext("province %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, province->ref_name.c_str());
	lua_pushstring(L, province->name.c_str());
	lua_pushnumber(L, province->color);
	return 4;
}

#ifndef SERVER_HEADLESS
#include <deque>
#include <mutex>
extern std::mutex render_province_mutex;
extern std::deque<size_t> render_province;
#endif
int LuaAPI::give_province_to(lua_State * L) {
	if(!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	size_t province_id = lua_tonumber(L, 1);
	if(province_id >= g_world->provinces.size()) {
		print_error(gettext("lua province_id is out of bounds"));
		return 0;
	}

	Province * province = g_world->provinces[province_id];
	size_t nation_id = lua_tonumber(L, 2);
	if(nation_id >= g_world->nations.size()) {
		print_error(gettext("lua nation_id is out of bounds"));
		return 0;
	}

	for(size_t i = province->min_x; i < province->max_x; i++) {
		for(size_t j = province->min_y; j < province->max_y; j++) {
			Tile * tile = &g_world->tiles[i + j * g_world->width];
			if(tile->province_id != province_id)
				continue;
			
			tile->owner_id = nation_id;
		}
	}
#ifndef SERVER_HEADLESS
	render_province_mutex.lock();
	render_province.push_back(province->min_x);
	render_province.push_back(province->min_y);
	render_province.push_back(province->max_x + 64);
	render_province.push_back(province->max_y + 64);
	render_province_mutex.unlock();
#endif
	return 0;
}

int LuaAPI::add_province_pop(lua_State * L) {
	size_t province_id = lua_tonumber(L, 1);
	if(province_id >= g_world->provinces.size()) {
		print_error(gettext("invalid province id %zu"), province_id);
		return 0;
	}

	Province * province = g_world->provinces[province_id];

	Pop * pop = new Pop();
	pop->type_id = lua_tonumber(L, 2);
	pop->culture_id = lua_tonumber(L, 3);
	pop->religion_id = lua_tonumber(L, 4);
	pop->size = lua_tonumber(L, 5);
	pop->literacy = lua_tonumber(L, 6);
	
	if(!pop->size) {
		print_error(gettext("can't create pops with 0 size"));
		return 0;
	} if(pop->culture_id >= g_world->cultures.size()) {
		print_error(gettext("lua culture_id out of bounds"));
		return 0;
	} if(pop->religion_id >= g_world->religions.size()) {
		print_error(gettext("lua religion_id out of bounds"));
		return 0;
	}
	province->pops.push_back(pop);
	return 0;
}

int LuaAPI::rename_province(lua_State * L) {
	size_t province_id = lua_tonumber(L, 1);
	if(province_id >= g_world->provinces.size()) {
		print_error(gettext("invalid province id %zu"), province_id);
		return 0;
	}

	Province * province = g_world->provinces[province_id];
	province->name = lua_tostring(L, 2);
	return 0;
}

int LuaAPI::add_province_nucleus(lua_State * L) {
	ProvinceId province_id = lua_tonumber(L, 1);
	if(province_id >= g_world->provinces.size()) {
		print_error(gettext("invalid province id %zu"), province_id);
		return 0;
	}

	NationId nation_id = lua_tonumber(L, 2);
	if(nation_id >= g_world->nations.size()) {
		print_error(gettext("invalid nation id %zu"), nation_id);
		return 0;
	}

	g_world->provinces[province_id]->nucleuses.push_back(g_world->nations[nation_id]);
	return 0;
}

int LuaAPI::add_company(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	Company * company = new Company();

	company->name = lua_tostring(L, 1);
	company->money = lua_tonumber(L, 2);
	company->is_transport = lua_toboolean(L, 3);
	company->is_retailer = lua_toboolean(L, 4);
	company->is_industry = lua_toboolean(L, 5);

	company->operating_provinces.clear();

	// Add onto vector
	g_world->companies.push_back(company);

	printf(gettext("company: %s"), company->name.c_str());
	printf("\n");

	lua_pushnumber(L, g_world->companies.size() - 1);
	return 1;
}

int LuaAPI::add_op_province_to_company(lua_State * L) {
	if(!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	size_t idx = lua_tonumber(L, 1);
	std::string ref_name = lua_tostring(L, 2);
	for(size_t i = 0; i < g_world->provinces.size(); i++) {
		if(g_world->provinces[i]->ref_name != ref_name)
			continue;
		g_world->companies[idx]->operating_provinces.push_back(i);
		break;
	}
	return 0;
}

int LuaAPI::add_event(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	Event * event = new Event();

	event->ref_name = lua_tostring(L, 1);
	event->conditions_function = lua_tostring(L, 2);
	event->do_event_function = lua_tostring(L, 3);

	printf(gettext("event: %s"), event->ref_name.c_str());
	printf("\n");

	// Add onto vector
	g_world->events.push_back(event);
	lua_pushnumber(L, g_world->events.size() - 1);
	return 1;
}

int LuaAPI::get_event(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Event * event = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->events.size(); i++) {
		if(g_world->events[i]->ref_name != ref_name) continue;
		event = g_world->events[i];
		break;
	} if(event == nullptr) {
		print_error(gettext("event %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, event->ref_name.c_str());
	lua_pushstring(L, event->conditions_function.c_str());
	lua_pushstring(L, event->do_event_function.c_str());
	return 4;
}

int LuaAPI::add_event_receivers(lua_State * L) {
	size_t event_id = lua_tonumber(L, 1);
	if(event_id >= g_world->events.size()) {
		print_error(gettext("invalid event id %zu"), event_id);
		return 0;
	}

	Event * event = g_world->events[event_id];

	// Add receivers of the event by id
	for(size_t i = 0; i < lua_tonumber(L, 2); i++) {
		size_t nation_id = lua_tonumber(L, 3 + i);
		if(nation_id >= g_world->nations.size()) {
			print_error(gettext("invalid nation id %zu"), event_id);
			continue;
		}
		event->receivers.push_back(g_world->nations[nation_id]);
	}
	return 0;
}

int LuaAPI::add_pop_type(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}
	
	PopType * pop = new PopType();
	
	pop->ref_name = lua_tostring(L, 1);
	pop->name = lua_tostring(L, 2);
	
	printf(gettext("pop_type: %s"), pop->ref_name.c_str());
	printf("\n");
	
	// Add onto vector
	g_world->pop_types.push_back(pop);
	lua_pushnumber(L, g_world->pop_types.size() - 1);
	return 1;
}

int LuaAPI::get_pop_type(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	PopType * pop_type = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->pop_types.size(); i++) {
		if(g_world->pop_types[i]->ref_name != ref_name) continue;
		pop_type = g_world->pop_types[i];
		break;
	} if(pop_type == nullptr) {
		print_error(gettext("pop_type %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, pop_type->ref_name.c_str());
	lua_pushstring(L, pop_type->name.c_str());
	return 4;
}

int LuaAPI::add_culture(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	Culture * culture = new Culture();

	culture->ref_name = lua_tostring(L, 1);
	culture->name = lua_tostring(L, 2);

	printf(gettext("culture: %s"), culture->ref_name.c_str());
	printf("\n");

	g_world->cultures.push_back(culture);
	lua_pushnumber(L, g_world->cultures.size() - 1);
	return 1;
}

int LuaAPI::get_culture(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Culture * culture = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->cultures.size(); i++) {
		if(g_world->cultures[i]->ref_name != ref_name) continue;
		culture = g_world->cultures[i];
		break;
	} if(culture == nullptr) {
		print_error(gettext("culture %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, culture->ref_name.c_str());
	lua_pushstring(L, culture->name.c_str());
	return 4;
}

int LuaAPI::add_religion(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	Religion * religion = new Religion();

	religion->ref_name = lua_tostring(L, 1);
	religion->name = lua_tostring(L, 2);

	printf(gettext("religion: %s"), religion->ref_name.c_str());
	printf("\n");

	g_world->religions.push_back(religion);
	lua_pushnumber(L, g_world->religions.size() - 1);
	return 1;
}

int LuaAPI::get_religion(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	Religion * religion = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->religions.size(); i++) {
		if(g_world->religions[i]->ref_name != ref_name) continue;
		religion = g_world->religions[i];
		break;
	} if(religion == nullptr) {
		print_error(gettext("religion %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, religion->ref_name.c_str());
	lua_pushstring(L, religion->name.c_str());
	return 4;
}

int LuaAPI::add_unit_type(lua_State * L) {
	if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	UnitType * unit_type = new UnitType();

	unit_type->ref_name = lua_tostring(L, 1);
	unit_type->name = lua_tostring(L, 2);
	unit_type->attack = lua_tonumber(L, 3);
	unit_type->defense = lua_tonumber(L, 4);
	unit_type->max_health = lua_tonumber(L, 5);

	printf(gettext("unit_type: %s"), unit_type->ref_name.c_str());
	printf("\n");

	g_world->unit_types.push_back(unit_type);
	lua_pushnumber(L, g_world->unit_types.size() - 1);
	return 1;
}

int LuaAPI::get_unit_type(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string ref_name = lua_tostring(L, 1);
	UnitType * unit_type = nullptr;
	
	size_t i;
	for(i = 0; i < g_world->unit_types.size(); i++) {
		if(g_world->unit_types[i]->ref_name != ref_name)
			continue;
		
		unit_type = g_world->unit_types[i];
		break;
	} if(unit_type == nullptr) {
		print_error(gettext("unit_type %s not found"), ref_name.c_str());
		return 0;
	}
	lua_pushnumber(L, i);
	lua_pushstring(L, unit_type->ref_name.c_str());
	lua_pushstring(L, unit_type->name.c_str());
	lua_pushnumber(L, unit_type->attack);
	lua_pushnumber(L, unit_type->defense);
	lua_pushnumber(L, unit_type->max_health);
	return 4;
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

// Checks all events and their condition functions
void LuaAPI::check_events(lua_State * L) {
	// Because of the logic of this loop, only 1 event can happen in the world per tick
	// This is on purpouse ;)
	for(size_t i = 0; i < g_world->events.size(); i++) {
		Event * event = g_world->events[i];
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

int LuaAPI::get_text(lua_State * L) {
	if(!lua_isstring(L, 1)) {
		print_error(gettext("lua argument type mismatch"));
		return 0;
	}

	std::string msgid = lua_tostring(L, 1);
	std::string end_msg = gettext(msgid.c_str());
	lua_pushstring(L, end_msg.c_str());
	return 1;
}
