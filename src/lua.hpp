#ifndef LUA_H
#define LUA_H

#ifdef WINDOWS
#   include <lua.hpp>
#else
#   include <lua5.4/lua.hpp>
#endif //WINDOWS

namespace LuaAPI {
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
	int add_op_province_to_company(lua_State * L);
	int get_text(lua_State * L);
};

#endif
