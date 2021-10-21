#ifndef LUA_H
#define LUA_H

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
#include "../nation.hpp"

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

    int add_invention(lua_State* L);
    int get_invention(lua_State* L);
    int set_nation_mod_to_invention(lua_State* L);

    int add_technology(lua_State* L);
    int get_technology(lua_State* L);
    int add_req_tech_to_tech(lua_State* L);

    int add_unit_trait(lua_State* L);

    int add_building_type(lua_State* L);

    int add_good(lua_State* L);
    int get_good(lua_State* L);

    int add_input_to_industry_type(lua_State* L);
    int add_output_to_industry_type(lua_State* L);
    int add_req_good_to_industry_type(lua_State* L);

    int add_nation(lua_State* L);
    int get_nation(lua_State* L);

    int get_friends_of_nation(lua_State* L);
    int get_enemies_of_nation(lua_State* L);
    int get_allies_of_nation(lua_State* L);
    int get_warenemies_of_nation(lua_State* L);
    int get_embargoed_of_nation(lua_State* L);

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

    int add_nation_mod(lua_State* L);
    int get_nation_mod(lua_State* L);

    int add_province(lua_State* L);
    int get_province(lua_State* L);
    int get_province_by_id(lua_State* L);
    int add_province_industry(lua_State* L);
    int give_province_to(lua_State* L);
    int get_province_owner(lua_State* L);
    int get_province_controller(lua_State* L);
    int get_province_neighbours(lua_State* L);
    int multiply_province_militancy_global(lua_State* L);
    int multiply_province_militancy_by_culture(lua_State* L);
    int multiply_province_militancy_by_religion(lua_State* L);
    int multiply_province_con_global(lua_State* L);
    int multiply_province_con_by_culture(lua_State* L);
    int multiply_province_con_by_religion(lua_State* L);

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

    int add_ideology(lua_State* L);
    int get_ideology(lua_State* L);

    // Time functions
    int get_hour(lua_State* L);
    int get_day(lua_State* L);
    int get_month(lua_State* L);
    int get_year(lua_State* L);
    int set_date(lua_State* L);

    // Beta functions that exist
    int add_op_province_to_company(lua_State* L);

    // Translation stuff
    int get_text(lua_State* L);

    // Functions not part of the API
    void check_events(lua_State* L);
};

#endif
