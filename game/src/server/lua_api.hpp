// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      server/lua_api.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <lua.hpp>
extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

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
        virtual const char* what() const noexcept {
            return buffer.c_str();
        }
    };

    int register_new_table(lua_State* L, const std::string& name, const std::vector<luaL_Reg> meta, const std::vector<luaL_Reg> methods);

    int add_terrain_type(lua_State* L);
    int get_terrain_type(lua_State* L);
    int get_terrain_type_by_id(lua_State* L);

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
    int switch_nation_soul(lua_State* L);
    int nation_declare_war_no_cb(lua_State* L);

    int get_provinces_owned_by_nation(lua_State* L);
    int get_provinces_with_nucleus_by_nation(lua_State* L);
    int set_nation_primary_language(lua_State* L);
    int set_nation_capital(lua_State* L);
    int add_accepted_language(lua_State* L);
    int add_accepted_religion(lua_State* L);
    int add_nation_client_hint(lua_State* L);
    int nation_declare_unjustified_war(lua_State* L);

    int get_nation_relation(lua_State* L);
    int set_nation_relation(lua_State* L);

    int add_province(lua_State* L);
    int update_province(lua_State* L);
    int get_province(lua_State* L);
    int get_province_by_id(lua_State* L);
    int province_add_unit(lua_State* L);
    int update_province_building(lua_State* L);
    int give_province_to(lua_State* L);
    int give_hard_province_to(lua_State* L);
    int get_province_owner(lua_State* L);
    int get_province_controller(lua_State* L);
    int get_province_neighbours(lua_State* L);
    int get_province_nuclei(lua_State* L);

    int add_province_pop(lua_State* L);
    int set_province_language(lua_State* L);
    int set_province_religion(lua_State* L);
    int rename_province(lua_State* L);
    int add_province_nucleus(lua_State* L);
    int add_province_owner(lua_State* L);

    int add_event(lua_State* L);
    int get_event(lua_State* L);
    int update_event(lua_State* L);
    int add_event_receivers(lua_State* L);

    int add_decision(lua_State* L);

    int add_pop_type(lua_State* L);
    int get_pop_type(lua_State* L);
    int get_pop_type_by_id(lua_State* L);

    int add_language(lua_State* L);
    int get_language(lua_State* L);
    int get_language_by_id(lua_State* L);

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

    // UI stuff
    int ui_new_button(lua_State* L);
    int ui_new_image(lua_State* L);
    int ui_new_group(lua_State* L);
    int ui_new_div(lua_State* L);
    int ui_new_window(lua_State* L);
    int ui_new_checkbox(lua_State* L);
    int ui_set_checkbox_value(lua_State* L);
    int ui_get_checkbox_value(lua_State* L);
    int ui_new_slider(lua_State* L);
    int ui_get_slider_value(lua_State* L);
    int ui_set_slider_value(lua_State* L);
    int ui_new_label(lua_State* L);
    int ui_set_text(lua_State* L);
    int ui_get_image(lua_State* L);
    int ui_set_image(lua_State* L);
    int ui_set_scroll(lua_State* L);
    int ui_set_on_click(lua_State* L);
    int ui_set_window_on_click_close_btn(lua_State* L);
    int ui_get_widget(lua_State* L);
    int ui_widget_kill(lua_State* L);
    int ui_widget_set_tooltip(lua_State* L);
    int ui_register_callback(lua_State* L);
    int ui_widget_set_flex(lua_State* L);
    int ui_call_builtin(lua_State* L);

    void invoke_registered_callback(lua_State* L, const std::string& name);
};