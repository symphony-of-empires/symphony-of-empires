#ifndef LUA_H
#define LUA_H

#ifdef windows
extern "C" {
#   include <lua.hpp>
#   include <lua.h>
#	include <lualib.h>
#	include <lauxlib.h>
}
#else
#   include <lua5.4/lua.hpp>
#endif
#include <string>
#include <vector>
#include "nation.hpp"

namespace LuaAPI {
	int add_unit_trait(lua_State* L);

	int add_outpost_type(lua_State* L);

	int add_good(lua_State* L);
	int get_good(lua_State* L);

	int add_industry_type(lua_State* L);
	int get_industry_type(lua_State* L);
	int add_input_to_industry_type(lua_State* L);
	int add_output_to_industry_type(lua_State* L);

	int add_nation(lua_State* L);
	int get_nation(lua_State* L);
	int set_nation_primary_culture(lua_State* L);
	int set_nation_capital(lua_State* L);
	int set_industry_output_mod(lua_State* L);
	int set_industry_input_mod(lua_State* L);
	int set_workers_needed_mod(lua_State* L);
	int set_salary_paid_mod(lua_State* L);
	int set_delivery_cost_mod(lua_State* L);
	int set_literacy_learn_mod(lua_State* L);
	int set_reproduction_mod(lua_State* L);
	int set_death_mod(lua_State* L);
	int set_militancy_mod(lua_State* L);
	int set_consciousness_mod(lua_State* L);
	int set_life_needs_met_mod(lua_State* L);
	int set_everyday_needs_met_mod(lua_State* L);
	int set_luxury_needs_met_mod(lua_State* L);
	int add_accepted_culture(lua_State* L);
	
	int add_province(lua_State* L);
	int get_province(lua_State* L);
	int add_province_industry(lua_State* L);
	int give_province_to(lua_State* L);
	int add_province_pop(lua_State* L);
	int rename_province(lua_State* L);
	int add_province_nucleus(lua_State* L);
	int add_province_owner(lua_State* L);

	int add_company(lua_State* L);

	int add_event(lua_State* L);
	int get_event(lua_State* L);
	int add_event_receivers(lua_State* L);

	int add_descision(lua_State* L);

	int add_pop_type(lua_State* L);
	int get_pop_type(lua_State* L);

	int add_culture(lua_State* L);
	int get_culture(lua_State* L);

	int add_religion(lua_State* L);
	int get_religion(lua_State* L);

	int add_unit_type(lua_State* L);
	int get_unit_type(lua_State* L);
	int add_req_good_unit_type(lua_State* L);

	int add_boat_type(lua_State* L);
	int get_boat_type(lua_State* L);
	int add_req_good_boat_type(lua_State* L);

	// Time functions
	int get_hour(lua_State* L);
	int get_day(lua_State* L);
	int get_month(lua_State* L);
	int get_year(lua_State* L);

	// Beta functions that exist
	int add_op_province_to_company(lua_State* L);

	// Translation stuff
	int get_text(lua_State* L);

	// Functions not part of the API
	void check_events(lua_State* L);
};

#endif
