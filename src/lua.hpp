#ifndef LUA_H
#define LUA_H

#ifdef WINDOWS
#   include <lua.hpp>
#else
#   include <lua5.4/lua.hpp>
#endif //WINDOWS

int World_LuaAddGood(lua_State * L);
int World_LuaAddIndustryType(lua_State * L);
int World_LuaAddInputToIndustryType(lua_State * L);
int World_LuaAddOutputToIndustryType(lua_State * L);
int World_LuaAddNation(lua_State * L);
int World_LuaAddProvince(lua_State * L);
int World_LuaAddCompany(lua_State * L);
int World_LuaAddOperationalProvinceToCompany(lua_State * L);
int World_LuaGettext(lua_State * L);

#endif
