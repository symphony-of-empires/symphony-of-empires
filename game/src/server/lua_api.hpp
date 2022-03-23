// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      server/lua_api.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#ifdef windows
#  include <lua.hpp>
#else
#  ifdef LUA54
#    include <lua5.4/lua.hpp>
#  else
#    include <lua5.3/lua.hpp>
#  endif
#endif
#include <string>
#include <vector>
#include <exception>

namespace LuaAPI {
    class Exception: public std::exception {
        std::string buffer;
    public:
        Exception(const std::string& message) {
            buffer = message;
        }
        virtual const char* what(void) const noexcept {
            return buffer.c_str();
        }
    };

    int register_new_table(lua_State* L, const std::string& name, const luaL_Reg meta[], const luaL_Reg methods[]);

    int add_terrain_type(lua_State* L);
    int get_terrain_type(lua_State* L);
    
    int set_nation_mod_to_invention(lua_State* L);

    int add_technology(lua_State* L);
    int get_technology(lua_State* L);
    int add_req_tech_to_tech(lua_State* L);

    int add_building_type(lua_State* L);
    int get_building_type(lua_State* L);

    int add_good(lua_State* L);
    int get_good(lua_State* L);

    int add_input_to_industry_type(lua_State* L);
    int add_output_to_industry_type(lua_State* L);
    int add_req_good_to_industry_type(lua_State* L);
    int add_req_technology_to_industry_type(lua_State* L);

    int add_nation(lua_State* L);
    int get_nation(lua_State* L);
    int get_nation_by_id(lua_State* L);
    int get_all_nations(lua_State* L);

    int get_provinces_owned_by_nation(lua_State* L);
    int get_provinces_with_nucleus_by_nation(lua_State* L);
    int set_nation_primary_culture(lua_State* L);
    int set_nation_capital(lua_State* L);
    int add_accepted_culture(lua_State* L);
    int add_accepted_religion(lua_State* L);
    int add_nation_client_hint(lua_State* L);
    int get_nation_policies(lua_State* L);
    int set_nation_policies(lua_State* L);
    int set_nation_ideology(lua_State* L);
    int nation_declare_unjustified_war(lua_State* L);

    int get_nation_relation(lua_State* L);
    int set_nation_relation(lua_State* L);

    int add_nation_mod(lua_State* L);
    int get_nation_mod(lua_State* L);

    int add_province(lua_State* L);
    int update_province(lua_State* L);
    int get_province(lua_State* L);
    int get_province_by_id(lua_State* L);
    int update_province_building(lua_State* L);
    int give_province_to(lua_State* L);
    int give_hard_province_to(lua_State* L);
    int get_province_owner(lua_State* L);
    int get_province_controller(lua_State* L);
    int get_province_neighbours(lua_State* L);
    int get_province_nuclei(lua_State* L);
    int get_province_pop(lua_State* L);
    int set_province_pop(lua_State* L);
    int get_province_pop_ideology_approval(lua_State* L);
    int set_province_pop_ideology_approval(lua_State* L);

    int add_province_pop(lua_State* L);
    int rename_province(lua_State* L);
    int add_province_nucleus(lua_State* L);
    int add_province_owner(lua_State* L);

    int add_event(lua_State* L);
    int get_event(lua_State* L);
    int update_event(lua_State* L);
    int add_event_receivers(lua_State* L);

    int add_descision(lua_State* L);

    int add_pop_type(lua_State* L);
    int get_pop_type(lua_State* L);
    int get_pop_type_by_id(lua_State* L);

    int add_culture(lua_State* L);
    int get_culture(lua_State* L);
    int get_culture_by_id(lua_State* L);

    int add_religion(lua_State* L);
    int get_religion(lua_State* L);
    int get_religion_by_id(lua_State* L);

    int add_unit_type(lua_State* L);
    int get_unit_type(lua_State* L);
    int add_req_good_unit_type(lua_State* L);

    int add_ideology(lua_State* L);
    int get_ideology(lua_State* L);
    int get_ideology_by_id(lua_State* L);

    // Functions not part of the API
    void check_events(lua_State* L);
};