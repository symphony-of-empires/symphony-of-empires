#ifndef LUA_H
#define LUA_H

#ifdef WIN32
#   include <lua.hpp>
#else
#   include <lua5.4/lua.hpp>
#endif
#include <string>
#include <vector>

namespace LuaAPI {
	class Event {
	public:
		std::string ref_name;
		std::string conditions_function;
		std::string do_event_function;
	};

	int add_good(lua_State * L);
	int get_good(lua_State * L);
	int add_industry_type(lua_State * L);
	int get_industry_type(lua_State * L);
	int add_input_to_industry_type(lua_State * L);
	int add_output_to_industry_type(lua_State * L);
	int add_nation(lua_State * L);
	int get_nation(lua_State * L);
	int add_province(lua_State * L);
	int get_province(lua_State * L);
	int give_province_to(lua_State * L);
	int add_company(lua_State * L);
	int add_event(lua_State * L);
	int add_pop_type(lua_State * L);

	// Time functions
	int get_hour(lua_State * L);
	int get_day(lua_State * L);
	int get_month(lua_State * L);
	int get_year(lua_State * L);

	// Beta functions that exist
	int add_op_province_to_company(lua_State * L);

	// Translation stuff
	int get_text(lua_State * L);

	// Functions not part of the API
	void check_events(lua_State * L);
};

extern std::vector<LuaAPI::Event> g_events;

#endif
