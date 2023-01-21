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
//      server/lua_api.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

// NOTE: Use .at() instead of the [] operator since it will
// throw exceptions when an invalid element is accessed so
// the lua exceptions

#include <cstring>
#include <cstdlib>
#include <cassert>

#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"

#include "server/lua_api.hpp"
#include "world.hpp"
#include "nation.hpp"
#include "server/economy.hpp"
#include "event.hpp"
#include "building.hpp"

int LuaAPI::register_new_table(lua_State* L, const std::string& name, const std::vector<luaL_Reg> meta, const std::vector<luaL_Reg> methods) {
    if(luaL_newmetatable(L, name.c_str())) {
        lua_newtable(L);

        // Methods
        for(const auto& reg : methods)
            lua_register(L, reg.name, reg.func);
        lua_setfield(L, -2, "__index");

        // Metatable
        for(const auto& reg : meta)
            lua_register(L, reg.name, reg.func);

        lua_pushstring(L, name.c_str());
        lua_setfield(L, -2, "__metatable");

        lua_setglobal(L, name.c_str());
    }
    return 0;
}

// !!! IMPORTANT !!!
// Doing changes to the world state (like Nation and Province) does NOT require an explicit update as this is done
// after each economical tick
// HOWEVER, adding new elements or changing other states REQUIRES a explicit synchronization!!
template<typename T>
static const T& find_or_throw(const std::string_view ref_name) {
    const auto& list = World::get_instance().get_list((T*)nullptr);
    const auto result = std::find_if(list.begin(), list.end(), [ref_name](const auto& o) {
        return !strcmp(o.ref_name.c_str(), ref_name.data());
    });

    if(result == list.end())
        CXX_THROW(Eng3D::LuaException, translate_format("Object<%s> not found", typeid(T).name()).c_str());
    return *result;
}

void append_to_table(lua_State* L, int* index, float number) {
    lua_pushnumber(L, (*index)++);
    lua_pushnumber(L, number);
    lua_settable(L, -3);
}

void append_to_table(lua_State* L, int index, float number) {
    append_to_table(L, &index, number);
}

void append_to_table(lua_State* L, int* index, const std::string& text) {
    lua_pushnumber(L, (*index)++);
    lua_pushstring(L, text.c_str());
    lua_settable(L, -3);
}

void append_to_table(lua_State* L, int index, const std::string& text) {
    append_to_table(L, &index, text);
}

static float pop_number(lua_State* L) {
    const float amount = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return amount;
}

static std::string pop_string(lua_State* L) {
    const std::string& text = luaL_checkstring(L, -1);
    if(text.empty())
        luaL_error(L, "Expected a text but got empty string");
    lua_pop(L, 1);
    return text;
}

int LuaAPI::add_terrain_type(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    TerrainType terrain_type{};
    terrain_type.ref_name = luaL_checkstring(L, 1);
    terrain_type.name = luaL_checkstring(L, 2);
    terrain_type.color = std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 3))) >> 8;
    terrain_type.color |= 0xff000000;
    terrain_type.penalty = lua_tonumber(L, 4);
    terrain_type.is_water_body = lua_toboolean(L, 5);
    g_world.insert(terrain_type);
    lua_pushnumber(L, g_world.terrain_types.size() - 1);
    return 1;
}

int LuaAPI::get_terrain_type_by_id(lua_State* L) {
    const auto& terrain_type = g_world.terrain_types.at(lua_tonumber(L, 1));

    lua_pushstring(L, terrain_type.ref_name.c_str());
    lua_pushstring(L, terrain_type.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((terrain_type.color & 0x00ffffff) << 8));
    lua_pushnumber(L, terrain_type.penalty);
    lua_pushboolean(L, terrain_type.is_water_body);
    return 5;
}

int LuaAPI::get_terrain_type(lua_State* L) {
    const auto& terrain_type = find_or_throw<TerrainType>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(terrain_type));
    lua_pushstring(L, terrain_type.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((terrain_type.color & 0x00ffffff) << 8));
    lua_pushnumber(L, terrain_type.penalty);
    lua_pushboolean(L, terrain_type.is_water_body);
    return 5;
}

int LuaAPI::add_technology(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    Technology technology{};
    technology.ref_name = luaL_checkstring(L, 1);
    technology.name = luaL_checkstring(L, 2);
    technology.description = lua_tostring(L, 3);
    technology.cost = (lua_tonumber(L, 4));
    technology.type = (TechnologyType)((int)lua_tonumber(L, 5));
    g_world.insert(technology);
    lua_pushnumber(L, g_world.technologies.size() - 1);
    return 1;
}

int LuaAPI::get_technology(lua_State* L) {
    const auto& technology = find_or_throw<Technology>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(technology));
    lua_pushstring(L, technology.name.c_str());
    lua_pushstring(L, technology.description.c_str());
    lua_pushnumber(L, technology.cost);
    lua_pushnumber(L, technology.type);
    return 5;
}

int LuaAPI::add_req_tech_to_tech(lua_State* L) {
    Technology* technology = &g_world.technologies.at(lua_tonumber(L, 1));
    technology->req_technologies.push_back(TechnologyId(lua_tonumber(L, 2)));
    return 0;
}

int LuaAPI::add_building_type(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    BuildingType building_type{};
    building_type.ref_name = luaL_checkstring(L, 1);
    building_type.name = luaL_checkstring(L, 2);
    building_type.can_plot_on_sea(lua_toboolean(L, 3));
    building_type.can_build_land_units(lua_toboolean(L, 4));
    building_type.can_build_naval_units(lua_toboolean(L, 5));
    g_world.insert(building_type);
    lua_pushnumber(L, g_world.building_types.size() - 1);
    return 1;
}

int LuaAPI::get_building_type(lua_State* L) {
    const auto& building_type = find_or_throw<BuildingType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, (size_t)building_type.get_id());
    lua_pushstring(L, building_type.ref_name.c_str());
    lua_pushstring(L, building_type.name.c_str());
    lua_pushboolean(L, building_type.can_plot_on_sea());
    lua_pushboolean(L, building_type.can_build_land_units());
    lua_pushboolean(L, building_type.can_build_naval_units());
    lua_pushnumber(L, 0.f);
    return 7;
}

int LuaAPI::add_good(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    Commodity commodity{};
    commodity.ref_name = luaL_checkstring(L, 1);
    commodity.name = luaL_checkstring(L, 2);
    g_world.insert(commodity);
    lua_pushnumber(L, g_world.commodities.size() - 1);
    return 1;
}

int LuaAPI::get_good(lua_State* L) {
    const auto& commodity = find_or_throw<Commodity>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(commodity));
    lua_pushstring(L, commodity.name.c_str());
    return 2;
}

int LuaAPI::add_input_to_industry_type(lua_State* L) {
    auto& industry_type = g_world.building_types.at(lua_tonumber(L, 1));
    auto& commodity = g_world.commodities.at(lua_tonumber(L, 2));
    industry_type.input_ids.push_back(commodity);
    industry_type.num_req_workers += 100;
    return 0;
}

int LuaAPI::add_output_to_industry_type(lua_State* L) {
    auto& industry_type = g_world.building_types.at(lua_tonumber(L, 1));
    auto& commodity = g_world.commodities.at(lua_tonumber(L, 2));
    industry_type.output_id.emplace(commodity.get_id());
    industry_type.num_req_workers += 100;
    return 0;
}

int LuaAPI::add_req_good_to_industry_type(lua_State* L) {
    auto& industry_type = g_world.building_types.at(lua_tonumber(L, 1));
    auto& commodity = g_world.commodities.at(lua_tonumber(L, 2));
    industry_type.req_goods.emplace_back(commodity, lua_tonumber(L, 3));
    return 0;
}

int LuaAPI::add_req_technology_to_industry_type(lua_State* L) {
    auto& industry_type = g_world.building_types.at(lua_tonumber(L, 1));
    auto& technology = g_world.technologies.at(lua_tonumber(L, 2));
    industry_type.req_technologies.push_back(technology);
    return 0;
}

int LuaAPI::add_nation(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    Nation nation{};
    nation.ref_name = luaL_checkstring(L, 1);
    nation.name = luaL_checkstring(L, 2);
    nation.ideology_id = IdeologyId(0);
    nation.commodity_production.resize(g_world.commodities.size(), 1.f);
    nation.religion_discrim.resize(g_world.religions.size(), 0.f);
    nation.language_acceptance.resize(g_world.languages.size(), 0.f);
    nation.client_hints.resize(g_world.ideologies.size());
    nation.research.resize(g_world.technologies.size());

    // Check for duplicates
    for(const auto& other_nation : g_world.nations) {
        if(nation.ref_name == other_nation.ref_name)
            luaL_error(L, string_format("Duplicate ref_name %s", nation.ref_name.c_str()).c_str());
    }
    g_world.insert(nation);
    lua_pushnumber(L, g_world.nations.size() - 1);
    return 1;
}

int LuaAPI::get_nation(lua_State* L) {
    const auto& nation = find_or_throw<Nation>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(nation));
    lua_pushstring(L, nation.name.c_str());
    return 2;
}

int LuaAPI::get_nation_by_id(lua_State* L) {
    const auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    lua_pushstring(L, nation.name.c_str());
    lua_pushstring(L, nation.ref_name.c_str());
    return 2;
}

int LuaAPI::get_all_nations(lua_State* L) {
    lua_newtable(L);

    size_t i = 0;
    for(const auto& nation : g_world.nations) {
        lua_pushnumber(L, nation);
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::switch_nation_soul(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& target = g_world.nations.at(lua_tonumber(L, 2));
    if(&nation == &target)
        luaL_error(L, string_format("%s can't switch to itself", nation.ref_name.c_str()).c_str());
    /// @todo Broadcast this to all clients?
    return 0;
}

int LuaAPI::nation_declare_war_no_cb(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
    if(&nation == &other_nation)
        luaL_error(L, string_format("%s can't declare war on itself", nation.ref_name.c_str()).c_str());
    nation.declare_war(other_nation);
    return 0;
}

int LuaAPI::get_provinces_owned_by_nation(lua_State* L) {
    const auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto province_id : nation.owned_provinces) {
        lua_pushnumber(L, (size_t)province_id);
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_provinces_with_nucleus_by_nation(lua_State* L) {
    const auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& province : g_world.provinces) {
        bool is_nuclei = false;
        for(const auto& nucleus_id : province.nuclei) {
            auto& nucleus = g_world.nations[nucleus_id];
            if(&nucleus == &nation) {
                is_nuclei = true;
                break;
            }
        }
        if(!is_nuclei) continue;
        lua_pushnumber(L, (size_t)g_world.get_id(province));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::set_nation_primary_language(lua_State*) {
    return 0;
}

int LuaAPI::set_nation_capital(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    nation.capital_id = ProvinceId(lua_tonumber(L, 2));
    return 0;
}

int LuaAPI::add_accepted_language(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    nation.language_acceptance.at(lua_tonumber(L, 2)) = 1.f;
    return 0;
}

int LuaAPI::add_accepted_religion(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    nation.religion_discrim.at(lua_tonumber(L, 2)) = 1.f;
    return 0;
}

int LuaAPI::add_nation_client_hint(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    Nation::ClientHint hint{};
    hint.ideology_id = IdeologyId(lua_tonumber(L, 2));
    hint.name = luaL_checkstring(L, 3);
    hint.color = std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 4))) >> 8;
    hint.color |= 0xff000000;
    nation.client_hints[hint.ideology_id] = hint;
    return 0;
}

int LuaAPI::get_nation_relation(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
    auto& relation = g_world.get_relation(nation, other_nation);
    lua_pushnumber(L, relation.alliance);
    lua_pushnumber(L, relation.relation);
    lua_pushboolean(L, relation.has_war);
    return 3;
}

int LuaAPI::set_nation_relation(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
    auto& relation = g_world.get_relation(nation, other_nation);
    relation.alliance = lua_tonumber(L, 3);
    relation.relation = lua_tonumber(L, 4);
    relation.has_war = lua_toboolean(L, 5);
    return 0;
}

int LuaAPI::nation_declare_unjustified_war(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
    if(!g_world.get_relation(nation, other_nation).has_war)
        nation.declare_war(other_nation);
    return 0;
}

int LuaAPI::add_province(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    Province province{};
    province.ref_name = luaL_checkstring(L, 1);
    province.color = (std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 2))) >> 8) | 0xff000000;
    province.name = luaL_checkstring(L, 3);
    province.terrain_type_id = TerrainTypeId(lua_tonumber(L, 4));

    // Load rgo_size
    province.rgo_size.resize(g_world.commodities.size(), 0);
    luaL_checktype(L, 5, LUA_TTABLE);
    size_t n_rgo_size = lua_rawlen(L, 5);
    for(size_t i = 1; i <= n_rgo_size; i++) {
        lua_rawgeti(L, 5, i);
        if(!lua_istable(L, -1))
            luaL_error(L, "RGO size is a multidimensional table, \'rgo_size={\"wheat\",1}\' is not valid, do \'rgo_size={{\"wheat\",1}}\' instead");
        
        {
            lua_rawgeti(L, -1, 1);
            const Commodity& commodity = find_or_throw<Commodity>(luaL_checkstring(L, -1));
            lua_pop(L, 1);

            lua_rawgeti(L, -1, 2);
            const uint32_t amount = (uint32_t)lua_tonumber(L, -1);
            lua_pop(L, 1);

            province.rgo_size[commodity] = amount;
        }
        lua_pop(L, 1);
    }

    // Check for duplicates
    for(size_t i = 0; i < g_world.provinces.size(); i++) {
        if(province.color == g_world.provinces[i].color) {
            luaL_error(L, string_format("%s province has same color as %s", province.ref_name.c_str(), g_world.provinces[i].ref_name.c_str()).c_str());
        } else if(province.ref_name == g_world.provinces[i].ref_name) {
            luaL_error(L, string_format("Duplicate ref_name %s", province.ref_name.c_str()).c_str());
        }
    }
    
    province.products.resize(g_world.commodities.size(), Product{});
    province.languages.resize(g_world.languages.size(), 0.f);
    province.religions.resize(g_world.religions.size(), 0.f);
    province.buildings.resize(g_world.building_types.size());
    for(auto& building : province.buildings)
        building.estate_foreign.resize(g_world.nations.size());
    
    {
        size_t i = 0;
        for(auto& pop : province.pops) {
            pop.type_id = PopTypeId(i);
            i++;
        }
    }

    // Set bounding box of province to the whole world (will later be resized at the bitmap-processing step)
    province.box_area = Eng3D::Rect(0, 0, std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max());
    g_world.insert(province);
    lua_pushnumber(L, g_world.provinces.size() - 1);
    return 1;
}

int LuaAPI::update_province(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    province.ref_name = luaL_checkstring(L, 2);
    province.color = (std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 3))) >> 8) | 0xff000000;
    province.name = luaL_checkstring(L, 4);
    province.terrain_type_id = TerrainTypeId(lua_tonumber(L, 5));
    // Check for duplicates
    for(size_t i = 0; i < g_world.provinces.size(); i++) {
        if(province.color == g_world.provinces[i].color) {
            luaL_error(L, string_format("%s province has same color as %s", province.ref_name.c_str(), g_world.provinces[i].ref_name.c_str()).c_str());
        } else if(province.ref_name == g_world.provinces[i].ref_name) {
            luaL_error(L, string_format("Duplicate ref_name %s", province.ref_name.c_str()).c_str());
        }
    }
    return 0;
}

int LuaAPI::get_province(lua_State* L) {
    const Province& province = find_or_throw<Province>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(province));
    lua_pushstring(L, province.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((province.color & 0x00ffffff) << 8));
    lua_pushnumber(L, (size_t)province.terrain_type_id);
    lua_newtable(L);
    size_t index = 1;
    for(size_t i = 0; i < province.rgo_size.size(); i++) {
        if(province.rgo_size[i] != 0) {
            lua_pushnumber(L, index++);
            lua_newtable(L);
            append_to_table(L, 1, g_world.commodities[i].ref_name.c_str());
            append_to_table(L, 2, province.rgo_size[i]);
            lua_settable(L, -3);
        }
    }
    return 5;
}

int LuaAPI::get_province_by_id(lua_State* L) {
    const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    lua_pushstring(L, province.ref_name.c_str());
    lua_pushstring(L, province.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((province.color & 0x00ffffff) << 8));
    lua_pushnumber(L, (size_t)province.terrain_type_id);
    lua_newtable(L);
    size_t index = 1;
    for(size_t i = 0; i < province.rgo_size.size(); i++) {
        if(province.rgo_size[i] != 0) {
            lua_pushnumber(L, index++);
            lua_newtable(L);
            append_to_table(L, 1, g_world.commodities[i].ref_name.c_str());
            append_to_table(L, 2, province.rgo_size[i]);
            lua_settable(L, -3);
        }
    }
    return 5;
}

int LuaAPI::province_add_unit(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    auto& unit_type = g_world.unit_types.at(lua_tonumber(L, 2));
    const size_t size = lua_tonumber(L, 3);

    Unit unit{};
    unit.set_owner(g_world.nations.at(province.owner_id));
    unit.type_id = unit_type;
    unit.experience = 1.f;
    unit.size = size;
    unit.base = unit_type.max_health;
    g_world.unit_manager.add_unit(unit, province);
    return 0;
}

int LuaAPI::update_province_building(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    const auto& building_type = g_world.building_types.at(lua_tonumber(L, 2)); // Add up a level of upgrade
    province.buildings[building_type].level = lua_tonumber(L, 3);
    province.buildings[building_type].budget += 1000.f;
    return 0;
}

int LuaAPI::give_province_to(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    g_world.nations.at(lua_tonumber(L, 2)).give_province(province);
    return 0;
}

int LuaAPI::give_hard_province_to(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    auto& nation = g_world.nations.at(lua_tonumber(L, 2));
    const auto& unit_ids = g_world.unit_manager.get_province_units(province);
    for(const auto unit_id : unit_ids) {
        auto& unit = g_world.unit_manager.units[unit_id];
        if(unit.owner_id == province.controller_id)
            unit.set_owner(nation);
    }
    nation.control_province(province);
    nation.give_province(province);

    // Take all the troops of the dead nation if this is the last province of 'em
    if(!g_world.nations[province.controller_id].exists())
        g_world.unit_manager.units.for_each([&](auto& unit) {
            if(unit.owner_id == province.controller_id)
                unit.set_owner(nation);
        });
    return 0;
}

// Obtains the owner of a province (ref_name)
int LuaAPI::get_province_owner(lua_State* L) {
    const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    lua_pushstring(L, g_world.nations[province.controller_id].ref_name.c_str());
    return 1;
}

// Get the country who owms a larger chunk of the province - this is not the same as owner
int LuaAPI::get_province_controller(lua_State* L) {
    const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    lua_pushnumber(L, (size_t)province.controller_id);
    return 1;
}

// Obtains the neighbours of a province (by ID)
int LuaAPI::get_province_neighbours(lua_State* L) {
    const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    lua_newtable(L);
    size_t i = 0;
    for(const auto neighbour_id : province.neighbour_ids) {
        lua_pushnumber(L, (size_t)neighbour_id);
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_province_nuclei(lua_State* L) {
    const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    lua_newtable(L);
    size_t i = 0;
    for(const auto& nucleus_id : province.nuclei) {
        lua_pushnumber(L, (size_t)nucleus_id);
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::add_province_pop(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    auto& pop = province.pops.at(lua_tonumber(L, 2));
    pop.size = lua_tonumber(L, 3);
    if(!pop.size) {
        luaL_error(L, "Can't create pops with 0 size");
        return 0;
    }
    pop.literacy = lua_tonumber(L, 4);
    pop.budget = pop.size;
    return 0;
}

int LuaAPI::rename_province(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    province.name = luaL_checkstring(L, 2);
    return 0;
}

int LuaAPI::add_province_nucleus(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    province.nuclei.emplace_back(static_cast<size_t>(lua_tonumber(L, 2)));
    std::sort(province.nuclei.begin(), province.nuclei.end());
    auto last = std::unique(province.nuclei.begin(), province.nuclei.end());
    province.nuclei.erase(last, province.nuclei.end());
    return 0;
}

int LuaAPI::set_province_language(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    province.languages.at(lua_tonumber(L, 2)) = lua_tonumber(L, 3);
    return 0;
}

int LuaAPI::set_province_religion(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    province.religions.at(lua_tonumber(L, 2)) = lua_tonumber(L, 3);
    return 0;
}

int LuaAPI::add_province_owner(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    auto& nation = g_world.nations.at(lua_tonumber(L, 2));
    nation.give_province(province);
    return 0;
}

int LuaAPI::add_event(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    Event event{};
    event.ref_name = luaL_checkstring(L, 1);
    lua_pushvalue(L, 2);
    event.conditions_function = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 3);
    event.do_event_function = luaL_ref(L, LUA_REGISTRYINDEX);
    event.title = luaL_checkstring(L, 4);
    event.text = luaL_checkstring(L, 5);
    event.checked = lua_toboolean(L, 6);
    g_world.insert(event);
    lua_pushnumber(L, (size_t)g_world.get_id(event));
    return 1;
}

int LuaAPI::update_event(lua_State* L) {
    auto& event = g_world.events[lua_tonumber(L, 1)];
    event.ref_name = luaL_checkstring(L, 2);
    lua_pushvalue(L, 3);
    event.conditions_function = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 4);
    event.do_event_function = luaL_ref(L, LUA_REGISTRYINDEX);
    event.title = luaL_checkstring(L, 5);
    event.text = luaL_checkstring(L, 6);
    event.checked = lua_toboolean(L, 7);
    return 0;
}

int LuaAPI::get_event(lua_State* L) {
    const auto& event = find_or_throw<Event>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(event));
    /// @todo A better way to get closures
    lua_pushinteger(L, event.conditions_function);
    lua_pushinteger(L, event.do_event_function);
    lua_pushstring(L, event.title.c_str());
    lua_pushstring(L, event.text.c_str());
    lua_pushboolean(L, event.checked);
    return 6;
}

int LuaAPI::add_event_receivers(lua_State* L) {
    // Add receivers of the event by id
    auto& event = g_world.events.at(lua_tonumber(L, 1));
    for(size_t i = 0; i < lua_tonumber(L, 2); i++)
        event.receiver_ids.push_back((size_t)lua_tonumber(L, 3 + i));
    return 0;
}

int LuaAPI::add_decision(lua_State* L) {
    Event& event = g_world.events.at(lua_tonumber(L, 1));
    Decision decision = Decision();
    decision.ref_name = luaL_checkstring(L, 2);
    decision.name = luaL_checkstring(L, 3);
    lua_pushvalue(L, 4);
    decision.do_decision_function = luaL_ref(L, LUA_REGISTRYINDEX);
    decision.effects = luaL_checkstring(L, 5);
    event.decisions.push_back(decision); // Add onto vector
    return 0;
}

int LuaAPI::add_pop_type(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");
    
    PopType pop_type{};
    pop_type.ref_name = luaL_checkstring(L, 1);
    pop_type.name = luaL_checkstring(L, 2);
    pop_type.social_value = lua_tonumber(L, 3);

    pop_type.basic_needs_amount.resize(g_world.commodities.size(), 0.f);
    pop_type.luxury_needs_satisfaction.resize(g_world.commodities.size(), 0.f);
    pop_type.luxury_needs_deminishing_factor.resize(g_world.commodities.size(), 0.f);

    // Lua next = pops top and then pushes key & value in table
    lua_pushvalue(L, 4);
    lua_pushnil(L);
    while(lua_next(L, -2)) {
        lua_pushnil(L);
        lua_next(L, -2);
        const auto& commodity = find_or_throw<Commodity>(pop_string(L));
        lua_next(L, -2);
        const float amount = pop_number(L);
        lua_pop(L, 2);
        pop_type.basic_needs_amount[commodity] = amount;
    }
    lua_pop(L, 1);

    lua_pushvalue(L, 5);
    lua_pushnil(L);
    while(lua_next(L, -2)) {
        lua_pushnil(L);
        lua_next(L, -2);
        const auto& commodity = find_or_throw<Commodity>(pop_string(L));
        lua_next(L, -2);
        const float satisfaction = pop_number(L);
        lua_next(L, -2);
        const float deminishing = pop_number(L);
        lua_pop(L, 2);
        pop_type.luxury_needs_satisfaction[commodity] = satisfaction;
        pop_type.luxury_needs_deminishing_factor[commodity] = deminishing;
    }
    lua_pop(L, 1);

    g_world.insert(pop_type);
    lua_pushnumber(L, g_world.pop_types.size() - 1);
    return 1;
}

int LuaAPI::get_pop_type(lua_State* L) {
    const auto& pop_type = find_or_throw<PopType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, (size_t)pop_type.get_id());
    lua_pushstring(L, pop_type.name.c_str());
    lua_pushnumber(L, pop_type.social_value);
    lua_newtable(L);
    size_t index = 1;
    for(size_t i = 0; i < pop_type.basic_needs_amount.size(); i++) {
        if(pop_type.basic_needs_amount[i] != 0) {
            lua_pushnumber(L, index++);
            lua_newtable(L);
            append_to_table(L, 1, g_world.commodities[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.basic_needs_amount[i]);
            lua_settable(L, -3);
        }
    }
    lua_newtable(L);
    index = 1;
    for(size_t i = 0; i < pop_type.luxury_needs_satisfaction.size(); i++) {
        if(pop_type.luxury_needs_satisfaction[i] != 0) {
            lua_pushnumber(L, index++);
            lua_newtable(L);
            append_to_table(L, 1, g_world.commodities[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.luxury_needs_satisfaction[i]);
            append_to_table(L, 3, pop_type.luxury_needs_deminishing_factor[i]);
            lua_settable(L, -3);
        }
    }
    return 5;
}

int LuaAPI::get_pop_type_by_id(lua_State* L) {
    const PopType& pop_type = g_world.pop_types.at(lua_tonumber(L, 1));
    lua_pushstring(L, pop_type.ref_name.c_str());
    lua_pushstring(L, pop_type.name.c_str());
    lua_pushnumber(L, pop_type.social_value);
    lua_newtable(L);
    size_t index = 1;
    for(size_t i = 0; i < pop_type.basic_needs_amount.size(); i++) {
        if(pop_type.basic_needs_amount[i] != 0) {
            lua_pushnumber(L, index++);
            lua_newtable(L);
            append_to_table(L, 1, g_world.commodities[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.basic_needs_amount[i]);
            lua_settable(L, -3);
        }
    }
    index = 1;
    for(size_t i = 0; i < pop_type.luxury_needs_satisfaction.size(); i++) {
        if(pop_type.luxury_needs_satisfaction[i] != 0) {
            lua_pushnumber(L, index++);
            lua_newtable(L);
            append_to_table(L, 1, g_world.commodities[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.luxury_needs_satisfaction[i]);
            append_to_table(L, 3, pop_type.luxury_needs_deminishing_factor[i]);
            lua_settable(L, -3);
        }
    }
    return 5;
}

int LuaAPI::add_language(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    Language language{};
    language.ref_name = luaL_checkstring(L, 1);
    language.name = luaL_checkstring(L, 2);
    language.color = (std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 3))) >> 8) | 0xff000000;
    language.adjective = luaL_checkstring(L, 4);
    language.noun = luaL_checkstring(L, 5);
    language.combo_form = luaL_checkstring(L, 6);
    g_world.insert(language);
    lua_pushnumber(L, g_world.languages.size() - 1);
    return 1;
}

int LuaAPI::get_language(lua_State* L) {
    const auto& language = find_or_throw<Language>(luaL_checkstring(L, 1));

    lua_pushnumber(L, (size_t)g_world.get_id(language));
    lua_pushstring(L, language.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((language.color & 0x00ffffff) << 8));
    lua_pushstring(L, language.adjective.c_str());
    lua_pushstring(L, language.noun.c_str());
    lua_pushstring(L, language.combo_form.c_str());
    return 6;
}

int LuaAPI::get_language_by_id(lua_State* L) {
    const auto& language = g_world.languages.at(lua_tonumber(L, 1));
    lua_pushstring(L, language.ref_name.c_str());
    lua_pushstring(L, language.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((language.color & 0x00ffffff) << 8));
    lua_pushstring(L, language.adjective.c_str());
    lua_pushstring(L, language.noun.c_str());
    lua_pushstring(L, language.combo_form.c_str());
    return 6;
}

int LuaAPI::add_religion(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    Religion religion{};
    religion.ref_name = luaL_checkstring(L, 1);
    religion.name = luaL_checkstring(L, 2);
    religion.color = (std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 3))) >> 8) | 0xff000000;
    g_world.insert(religion);
    lua_pushnumber(L, g_world.religions.size() - 1);
    return 1;
}

int LuaAPI::get_religion(lua_State* L) {
    const auto& religion = find_or_throw<Religion>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(religion));
    lua_pushstring(L, religion.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((religion.color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::get_religion_by_id(lua_State* L) {
    const auto& religion = g_world.religions.at(lua_tonumber(L, 1));
    lua_pushstring(L, religion.ref_name.c_str());
    lua_pushstring(L, religion.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((religion.color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::add_unit_type(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    UnitType unit_type{};
    unit_type.ref_name = luaL_checkstring(L, 1);
    unit_type.name = luaL_checkstring(L, 2);
    unit_type.attack = (lua_tonumber(L, 3));
    unit_type.defense = (lua_tonumber(L, 4));
    unit_type.max_health = (lua_tonumber(L, 5));
    unit_type.is_ground = lua_toboolean(L, 6);
    unit_type.is_naval = lua_toboolean(L, 7);
    unit_type.speed = (lua_tonumber(L, 8));
    g_world.insert(unit_type);
    lua_pushnumber(L, g_world.unit_types.size() - 1);
    return 1;
}

int LuaAPI::get_unit_type(lua_State* L) {
    const auto unit_type = find_or_throw<UnitType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, (size_t)g_world.get_id(unit_type));
    lua_pushstring(L, unit_type.name.c_str());
    lua_pushnumber(L, unit_type.attack);
    lua_pushnumber(L, unit_type.defense);
    lua_pushnumber(L, unit_type.max_health);
    lua_pushboolean(L, unit_type.is_ground);
    lua_pushboolean(L, unit_type.is_naval);
    lua_pushnumber(L, unit_type.speed);
    return 8;
}

int LuaAPI::add_req_good_unit_type(lua_State* L) {
    auto& unit_type = g_world.unit_types.at(lua_tonumber(L, 1));
    auto& commodity = g_world.commodities.at(lua_tonumber(L, 2));
    size_t amount = lua_tonumber(L, 3);
    unit_type.req_goods.emplace_back(commodity, amount);
    return 0;
}

static int traceback(lua_State* L) {
    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");
    lua_pushvalue(L, 1);
    lua_pushinteger(L, 1);
    lua_call(L, 2, 1);
    return 1;
}

void LuaAPI::fire_event(lua_State* L, Nation& nation, Event& event, bool& is_multi, const std::string_view extra) {
    // Save the original event & momentarily replace it on the big world
    // since the event callback **MIGHT** modify the event itself so we store
    // a copy for each event everytime it fires
    auto orig_event = event;

    // Call the "do event" function
    Eng3D::Log::debug("event", Eng3D::translate_format("Event %s using lua#%i", event.ref_name.c_str(), event.do_event_function));
    int nargs = 1;
    lua_rawgeti(L, LUA_REGISTRYINDEX, event.do_event_function);
    lua_pushstring(L, nation.ref_name.c_str());
    if(!extra.empty()) {
        lua_pushstring(L, extra.data());
        nargs++;
    }
    if(g_world.lua.call_func(nargs, 1)) {
        Eng3D::Log::error("lua", translate_format("lua_pcall failed: %s", lua_tostring(L, -1)));
        lua_pop(L, 1);
        goto restore_original;
    }
    is_multi = lua_toboolean(L, -1);
    lua_pop(L, 1);

    {
        // The changes done to the event "locally" are then created into a new local event
        auto local_event = event;
        local_event.cached_id = Event::invalid();
        local_event.extra_data = std::string(extra);
        //local_event.ref_name = Eng3D::StringRef(string_format("%s:%s", local_event.ref_name.c_str(), nation.ref_name.c_str()).c_str());
        if(local_event.decisions.empty()) {
            Eng3D::Log::error("event", translate_format("Event %s has no decisions (ref_name=%s)", local_event.ref_name.c_str(), nation.ref_name.c_str()));
        } else {
            // Check that descisions have functions
            for(auto& descision : local_event.decisions) {
                descision.extra_data = local_event.extra_data;
                if(descision.do_decision_function == 0) {
                    Eng3D::Log::error("event", translate_format("(Lua event %s on descision %s has no function callback", orig_event.ref_name.c_str(), descision.ref_name.c_str()));
                    goto restore_original;
                }
            }
            nation.inbox.push_back(local_event);
            Eng3D::Log::debug("event", translate_format("Event triggered! %s (with %zu decisions)", local_event.ref_name.c_str(), local_event.decisions.size()));
        }
    }
restore_original: // Original event then gets restored
    event = orig_event;
}

// Checks all events and their condition functions
void LuaAPI::check_events(lua_State* L) {
    const std::scoped_lock lock(g_world.inbox_mutex);
    for(auto& event : g_world.events) {
        if(event.checked) continue;
        bool is_multi = true, has_fired = false;
        for(const auto nation_id : event.receiver_ids) {
            auto& nation = g_world.nations[nation_id];
            if(nation.exists()) {
                lua_rawgeti(L, LUA_REGISTRYINDEX, event.conditions_function);
                lua_pushstring(L, nation.ref_name.c_str());
                lua_pcall(L, 1, 1, 0);
                bool r = lua_toboolean(L, -1);
                lua_pop(L, 1);
                if(r) { // Conditions met
                    has_fired = true;
                    LuaAPI::fire_event(L, nation, event, is_multi, "");
                }
            }
        }
        // Event is marked as checked if it's not of multiple occurences
        if(has_fired && !is_multi)
            event.checked = true;
    }

    // Do decisions taken effects in the queue, then clear it awaiting other
    // taken decisions :)
    for(auto& [dec, nation_id] : g_world.taken_decisions) {
        const auto& nation = g_world.nations[nation_id];
        Eng3D::Log::debug("event", string_format("%s took the descision %i", nation.ref_name.c_str(), dec.do_decision_function));
        lua_rawgeti(L, LUA_REGISTRYINDEX, dec.do_decision_function);
        int nargs = 1;
        lua_pushstring(L, nation.ref_name.c_str());
        if(!dec.extra_data.get_string().empty()) {
            lua_pushstring(L, dec.extra_data.c_str());
            nargs++;
        }
        if(g_world.lua.call_func(nargs, 0)) {
            const std::string_view err_msg = lua_tostring(L, -1);
            lua_pop(L, 1);
            CXX_THROW(Eng3D::LuaException, string_format("%i(%s): %s", dec.do_decision_function, nation.ref_name.c_str(), err_msg).c_str());
        }
    }
    g_world.taken_decisions.clear();
}

#include "client/game_state.hpp"
#include "action.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"
int LuaAPI::ui_call_builtin(lua_State* L) {
    const std::string builtin_fn = luaL_checkstring(L, 1);
    auto& gs = static_cast<GameState&>(Eng3D::State::get_instance());

    if(builtin_fn == "gs.ai_control.form_packet") {
        //gs.client->send(Action::AiControl::form_packet(*gs.curr_nation));
        return 0;
    } else if(builtin_fn == "gs.ai_do_cmd_troops.get") {
        if(gs.curr_nation == nullptr)
            return 0;
        lua_pushboolean(L, gs.curr_nation->ai_do_cmd_troops);
        return 1;
    } else if(builtin_fn == "gs.ai_do_cmd_troops.set") {
        if(gs.curr_nation == nullptr)
            return 0;
        gs.curr_nation->ai_do_cmd_troops = lua_toboolean(L, 2);
        return 0;
    } else if(builtin_fn == "gs.map.reload_shaders") {
        gs.map->reload_shaders();
        return 0;
    } else if(builtin_fn == "gs.shader_opt.set") {
        auto options = gs.map->map_render->options.get_options();
        const std::string optname = luaL_checkstring(L, 2);
        for(const auto& option : options) {
            if(option.get_option() == optname) {
                bool is_used = lua_toboolean(L, 3);
                Eng3D::Log::debug("lua_bind", "Setting map_shader option " + optname + " to " + (is_used ? "TRUE" : "FALSE"));
                if(optname == "NOISE") {
                    gs.map->map_render->options.noise.used = is_used;
                } else if(optname == "SDF") {
                    gs.map->map_render->options.sdf.used = is_used;
                } else if(optname == "LIGHTING") {
                    gs.map->map_render->options.lighting.used = is_used;
                } else if(optname == "CITY_LIGHTS") {
                    gs.map->map_render->options.city_lights.used = is_used;
                } else if(optname == "PARALLAX") {
                    gs.map->map_render->options.parallax.used = is_used;
                } else if(optname == "RIVERS") {
                    gs.map->map_render->options.rivers.used = is_used;
                } else if(optname == "WATER") {
                    gs.map->map_render->options.water.used = is_used;
                } else if(optname == "GRID") {
                    gs.map->map_render->options.grid.used = is_used;
                } else if(optname == "UNITS") {
                    gs.map->map_render->options.units.used = is_used;
                } else if(optname == "BUILDINGS") {
                    gs.map->map_render->options.buildings.used = is_used;
                } else if(optname == "TREES") {
                    gs.map->map_render->options.trees.used = is_used;
                }
                gs.map->reload_shaders();
                break;
            }
        }
        return 0;
    } else if(builtin_fn == "gs.shader_opt.get") {
        auto options = gs.map->map_render->options.get_options();
        const std::string optname = luaL_checkstring(L, 2);
        for(const auto& option : options) {
            if(option.get_option() == optname) {
                bool is_used = false;
                if(optname == "NOISE") {
                    is_used = gs.map->map_render->options.noise.used;
                } else if(optname == "SDF") {
                    is_used = gs.map->map_render->options.sdf.used;
                } else if(optname == "LIGHTING") {
                    is_used = gs.map->map_render->options.lighting.used;
                } else if(optname == "CITY_LIGHTS") {
                    is_used = gs.map->map_render->options.city_lights.used;
                } else if(optname == "PARALLAX") {
                    is_used = gs.map->map_render->options.parallax.used;
                } else if(optname == "RIVERS") {
                    is_used = gs.map->map_render->options.rivers.used;
                } else if(optname == "WATER") {
                    is_used = gs.map->map_render->options.water.used;
                } else if(optname == "GRID") {
                    is_used = gs.map->map_render->options.grid.used;
                } else if(optname == "UNITS") {
                    is_used = gs.map->map_render->options.units.used;
                } else if(optname == "BUILDINGS") {
                    is_used = gs.map->map_render->options.buildings.used;
                } else if(optname == "TREES") {
                    is_used = gs.map->map_render->options.trees.used;
                }
                Eng3D::Log::debug("lua_bind", "Getting map_shader option " + optname + " is " + (is_used ? "TRUE" : "FALSE"));
                lua_pushboolean(L, is_used);
                return 1;
            }
        }
        return 0;
    } else if(builtin_fn == "gs.motion_blur.set") {
        gs.motion_blur = lua_toboolean(L, 2);
        return 0;
    } else if(builtin_fn == "gs.motion_blur.get") {
        lua_pushboolean(L, gs.motion_blur);
        return 1;
    } else if(builtin_fn == "gs.music_volume.set") {
        gs.audio_man.music_volume = luaL_checknumber(L, 2);
        return 0;
    } else if(builtin_fn == "gs.sound_volume.set") {
        gs.audio_man.sound_volume = luaL_checknumber(L, 2);
        return 0;
    } else if(builtin_fn == "gs.music_volume.get") {
        lua_pushnumber(L, gs.audio_man.music_volume);
        return 1;
    } else if(builtin_fn == "gs.sound_volume.get") {
        lua_pushnumber(L, gs.audio_man.sound_volume);
        return 1;
    }

    // Invalid callback name
    return 0;
}
