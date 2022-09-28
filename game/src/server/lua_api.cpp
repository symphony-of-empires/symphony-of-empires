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
const T* find_or_throw(const std::string& ref_name) {
    const auto& list = World::get_instance().get_list((T*)nullptr);
    const auto result = std::find_if(list.begin(), list.end(), [&ref_name](const auto& o) {
        return (o->ref_name == ref_name);
    });

    if(result == list.end())
        CXX_THROW(LuaAPI::Exception, translate_format("Object<%s> not found", typeid(T).name()));
    return (*result);
}

template<typename T>
const T& find_or_throw_local(const std::string& ref_name) {
    const auto& list = World::get_instance().get_list((T*)nullptr);
    const auto result = std::find_if(list.begin(), list.end(), [&ref_name](const auto& o) {
        return (o.ref_name == ref_name);
    });

    if(result == list.end())
        CXX_THROW(LuaAPI::Exception, translate_format("Object<%s> not found", typeid(T).name()));
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
    const auto& terrain_type = find_or_throw_local<TerrainType>(luaL_checkstring(L, 1));
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
    const auto& technology = find_or_throw_local<Technology>(luaL_checkstring(L, 1));
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
    const auto& building_type = find_or_throw_local<BuildingType>(luaL_checkstring(L, 1));

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

    Good good{};
    good.ref_name = luaL_checkstring(L, 1);
    good.name = luaL_checkstring(L, 2);
    g_world.insert(good);
    lua_pushnumber(L, g_world.goods.size() - 1);
    return 1;
}

int LuaAPI::get_good(lua_State* L) {
    const auto& good = find_or_throw_local<Good>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(good));
    lua_pushstring(L, good.name.c_str());
    return 2;
}

int LuaAPI::add_input_to_industry_type(lua_State* L) {
    auto& industry_type = g_world.building_types.at(lua_tonumber(L, 1));
    auto& good = g_world.goods.at(lua_tonumber(L, 2));
    industry_type.input_ids.push_back(good);
    industry_type.num_req_workers += 100;
    return 0;
}

int LuaAPI::add_output_to_industry_type(lua_State* L) {
    auto& industry_type = g_world.building_types.at(lua_tonumber(L, 1));
    auto& good = g_world.goods.at(lua_tonumber(L, 2));
    industry_type.output_id = good;
    industry_type.num_req_workers += 100;
    return 0;
}

int LuaAPI::add_req_good_to_industry_type(lua_State* L) {
    auto& industry_type = g_world.building_types.at(lua_tonumber(L, 1));
    auto& good = g_world.goods.at(lua_tonumber(L, 2));
    industry_type.req_goods.emplace_back(good, lua_tonumber(L, 3));
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
    nation.ideology_id = g_world.ideologies.at(0);
    nation.religion_discrim.resize(g_world.religions.size(), 0.5f);
    nation.language_discrim.resize(g_world.languages.size(), 0.5f);
    nation.client_hints.resize(g_world.ideologies.size());
    nation.research.resize(g_world.technologies.size());
    for(const auto& technology : g_world.technologies)
        nation.research[technology] = technology.cost;

    // Check for duplicates
    for(const auto& other_nation : g_world.nations) {
        if(Nation::is_invalid(other_nation))
            luaL_error(L, "Nation with invalid Id!");
        if(nation.ref_name == other_nation.ref_name)
            luaL_error(L, ("Duplicate ref_name " + nation.ref_name).c_str());
    }
    g_world.insert(nation);
    lua_pushnumber(L, g_world.nations.size() - 1);
    return 1;
}

int LuaAPI::get_nation(lua_State* L) {
    const auto& nation = find_or_throw_local<Nation>(luaL_checkstring(L, 1));
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
        assert(Nation::is_valid(nation));
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
        luaL_error(L, (nation.ref_name + " can't switch to itself").c_str());
    
    /// @todo Broadcast this to all clients?
    return 0;
}

int LuaAPI::nation_declare_war_no_cb(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
    if(&nation == &other_nation)
        luaL_error(L, (nation.ref_name + " can't declare war with itself").c_str());
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
    nation.language_discrim.at(lua_tonumber(L, 2)) = 1.f;
    return 0;
}

int LuaAPI::add_accepted_religion(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    nation.religion_discrim.at(lua_tonumber(L, 2)) = 1.f;
    return 0;
}

int LuaAPI::add_nation_client_hint(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    NationClientHint hint{};
    hint.ideology_id = g_world.ideologies.at(lua_tonumber(L, 2));
    hint.alt_name = luaL_checkstring(L, 3);
    hint.color = std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 4))) >> 8;
    hint.color |= 0xff000000;
    nation.client_hints[hint.ideology_id] = hint;
    return 0;
}

int LuaAPI::get_nation_policies(lua_State* L) {
    const auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    // We are going to push everything in the policies structure
    // this is horrible - reflection may help in this case
    const auto& policy = nation.current_policy;
    lua_pushnumber(L, policy.treatment);
    lua_pushnumber(L, policy.migration);
    lua_pushnumber(L, policy.immigration);
    lua_pushnumber(L, policy.censorship);
    lua_pushnumber(L, policy.build_infrastructure);
    lua_pushnumber(L, policy.build_factories);
    lua_pushboolean(L, policy.private_property);
    lua_pushboolean(L, policy.companies_allowed);
    lua_pushboolean(L, policy.public_education);
    lua_pushboolean(L, policy.secular_education);
    lua_pushboolean(L, policy.public_healthcare);
    lua_pushboolean(L, policy.social_security);
    lua_pushboolean(L, policy.slavery);
    lua_pushboolean(L, policy.legislative_parliament);
    lua_pushboolean(L, policy.executive_parliament);
    lua_pushboolean(L, policy.constitutional);
    lua_pushboolean(L, policy.foreign_trade);
    lua_pushnumber(L, (policy.import_tax));
    lua_pushnumber(L, (policy.export_tax));
    lua_pushnumber(L, 1.f);
    lua_pushnumber(L, 1.f);
    lua_pushnumber(L, (policy.poor_flat_tax));
    lua_pushnumber(L, (policy.med_flat_tax));
    lua_pushnumber(L, (policy.rich_flat_tax));
    lua_pushnumber(L, (policy.industry_tax));
    lua_pushnumber(L, (policy.military_spending));
    lua_pushboolean(L, policy.free_supplies);
    lua_pushnumber(L, (policy.min_wage));
    lua_pushnumber(L, (policy.min_sv_for_parliament));
    return 29;
}

int LuaAPI::set_nation_policies(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    // We are going to push everything in the policies structure
    // this is horrible - reflection may help in this case
    auto& policy = nation.current_policy;
    policy.treatment = static_cast<TreatmentPolicy>(lua_tonumber(L, 1));
    policy.migration = static_cast<AllowancePolicy>(lua_tonumber(L, 2));
    policy.immigration = static_cast<AllowancePolicy>(lua_tonumber(L, 3));
    policy.censorship = static_cast<CensorshipPolicy>(lua_tonumber(L, 4));
    policy.build_infrastructure = static_cast<AutoBuildPolicy>(lua_tonumber(L, 5));
    policy.build_factories = static_cast<AutoBuildPolicy>(lua_tonumber(L, 6));
    policy.private_property = lua_toboolean(L, 7);
    policy.companies_allowed = lua_toboolean(L, 8);
    policy.public_education = lua_toboolean(L, 9);
    policy.secular_education = lua_toboolean(L, 10);
    policy.public_healthcare = lua_toboolean(L, 11);
    policy.social_security = lua_toboolean(L, 12);
    policy.slavery = lua_toboolean(L, 13);
    policy.legislative_parliament = lua_toboolean(L, 14);
    policy.executive_parliament = lua_toboolean(L, 15);
    policy.constitutional = lua_toboolean(L, 16);
    policy.foreign_trade = lua_toboolean(L, 17);
    policy.import_tax = (lua_tonumber(L, 18));
    policy.export_tax = (lua_tonumber(L, 19));
    //(lua_tonumber(L, 20));
    //(lua_tonumber(L, 21));
    policy.poor_flat_tax = (lua_tonumber(L, 22));
    policy.med_flat_tax = (lua_tonumber(L, 23));
    policy.rich_flat_tax = (lua_tonumber(L, 24));
    policy.industry_tax = (lua_tonumber(L, 25));
    policy.military_spending = (lua_tonumber(L, 26));
    policy.free_supplies = lua_toboolean(L, 27);
    policy.min_wage = (lua_tonumber(L, 28));
    policy.min_sv_for_parliament = (lua_tonumber(L, 29));
    return 0;
}

int LuaAPI::set_nation_ideology(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    nation.ideology_id = g_world.ideologies.at(lua_tonumber(L, 2));
    return 0;
}

int LuaAPI::get_nation_relation(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
    auto& relation = g_world.get_relation(nation, other_nation);
    lua_pushnumber(L, relation.relation);
    lua_pushboolean(L, relation.has_war);
    return 2;
}

int LuaAPI::set_nation_relation(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
    auto& relation = g_world.get_relation(nation, other_nation);
    relation.relation = lua_tonumber(L, 3);
    return 0;
}

int LuaAPI::nation_make_puppet(lua_State* L) {
    auto& nation = g_world.nations.at(lua_tonumber(L, 1));
    auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
    other_nation.puppet_master_id = nation;
    auto& relation = g_world.get_relation(nation, other_nation);
    relation.alliance = 1.f;
    relation.relation = 0.f;
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
    province.rgo_size.resize(g_world.goods.size(), 0);
    luaL_checktype(L, 5, LUA_TTABLE);
    size_t n_rgo_size = lua_rawlen(L, 5);
    for(size_t i = 1; i <= n_rgo_size; i++) {
        lua_rawgeti(L, 5, i);
        if(!lua_istable(L, -1))
            luaL_error(L, "RGO size is a multidimensional table, \'rgo_size={\"wheat\",1}\' is not valid, do \'rgo_size={{\"wheat\",1}}\' instead");
        
        {
            lua_rawgeti(L, -1, 1);
            const Good& good = find_or_throw_local<Good>(luaL_checkstring(L, -1));
            lua_pop(L, 1);

            lua_rawgeti(L, -1, 2);
            const uint32_t amount = (uint32_t)lua_tonumber(L, -1);
            lua_pop(L, 1);

            province.rgo_size[good] = amount;
        }
        lua_pop(L, 1);
    }

    // Check for duplicates
    for(size_t i = 0; i < g_world.provinces.size(); i++) {
        if(province.color == g_world.provinces[i].color) {
            luaL_error(L, (province.ref_name + " province has same color as " + g_world.provinces[i].ref_name).c_str());
        } else if(province.ref_name == g_world.provinces[i].ref_name) {
            luaL_error(L, ("Duplicate ref_name " + province.ref_name).c_str());
        }
    }
    
    province.products.resize(g_world.goods.size(), Product{});
    province.languages.resize(g_world.languages.size(), 0.f);
    province.religions.resize(g_world.religions.size(), 0.f);
    province.buildings.resize(g_world.building_types.size());
    for(const auto& building_type : g_world.building_types)
        province.buildings[building_type].stockpile.resize(building_type.input_ids.size(), 0);
    // Set bounding box of province to the whole world (will later be resized at the bitmap-processing step)
    province.box_area = Eng3D::Rect(0, 0, std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max());
    province.pops.reserve(100);
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
            luaL_error(L, (province.ref_name + " province has same color as " + g_world.provinces[i].ref_name).c_str());
        } else if(province.ref_name == g_world.provinces[i].ref_name) {
            luaL_error(L, ("Duplicate ref_name " + province.ref_name).c_str());
        }
    }
    return 0;
}

int LuaAPI::get_province(lua_State* L) {
    const Province& province = find_or_throw_local<Province>(luaL_checkstring(L, 1));
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
            append_to_table(L, 1, g_world.goods[i].ref_name.c_str());
            append_to_table(L, 2, province.rgo_size[i]);
            lua_settable(L, -3);
        }
    }
    return 5;
}

int LuaAPI::get_province_by_id(lua_State* L) {
    const Province& province = g_world.provinces.at(lua_tonumber(L, 1));
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
            append_to_table(L, 1, g_world.goods[i].ref_name.c_str());
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
    if(Nation::is_valid(province.controller_id) && !g_world.nations[province.controller_id].exists())
        for(auto& unit : g_world.unit_manager.units)
            if(unit.owner_id == province.controller_id)
                unit.set_owner(nation);
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

int LuaAPI::get_province_pops_size(lua_State* L) {
    const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    lua_pushnumber(L, province.pops.size());
    return 1;
}

int LuaAPI::get_province_pop(lua_State* L) {
    const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    const auto& pop = province.pops.at(lua_tonumber(L, 2));
    lua_pushnumber(L, pop.size);
    lua_pushnumber(L, pop.budget);
    lua_pushnumber(L, pop.literacy);
    lua_pushnumber(L, pop.life_needs_met);
    lua_pushnumber(L, 1.f);
    lua_pushnumber(L, 1.f);
    lua_pushnumber(L, (size_t)pop.type_id);
    lua_pushnumber(L, pop.get_ideology());
    lua_pushnumber(L, pop.militancy);
    return 9;
}

int LuaAPI::set_province_pop(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    auto& pop = province.pops.at(lua_tonumber(L, 2));
    pop.size = lua_tonumber(L, 3);
    pop.budget = lua_tonumber(L, 4);
    pop.literacy = lua_tonumber(L, 5);
    pop.life_needs_met = lua_tonumber(L, 6);
    //lua_tonumber(L, 7);
    //lua_tonumber(L, 8);
    pop.type_id = PopTypeId(lua_tonumber(L, 9));
    pop.militancy = lua_tonumber(L, 10);
    return 0;
}

int LuaAPI::get_province_pop_ideology_approval(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    auto& pop = province.pops.at(lua_tonumber(L, 2));
    lua_pushnumber(L, pop.ideology_approval.at(lua_tonumber(L, 3)));
    return 1;
}

int LuaAPI::set_province_pop_ideology_approval(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    auto& pop = province.pops.at(lua_tonumber(L, 2));
    pop.ideology_approval.at(lua_tonumber(L, 3)) = lua_tonumber(L, 4);
    return 0;
}

int LuaAPI::add_province_pop(lua_State* L) {
    auto& province = g_world.provinces.at(lua_tonumber(L, 1));
    Pop pop{};
    pop.type_id = PopTypeId(lua_tonumber(L, 2));
    pop.size = lua_tonumber(L, 3);
    pop.literacy = lua_tonumber(L, 4);

    pop.budget = pop.size * 100.f * g_world.pop_types[pop.type_id].social_value;

    /// @todo Make ideology NOT be random
    pop.ideology_approval.resize(g_world.ideologies.size(), 0.f);

    if(!pop.size)
        luaL_error(L, "Can't create pops with 0 size");
    province.pops.push_back(pop);
    assert(province.pops.size() < 100);
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
    const auto& event = find_or_throw_local<Event>(luaL_checkstring(L, 1));
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
        event.receivers.push_back(&g_world.nations.at(lua_tonumber(L, 3 + i)));
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
    bool is_burgeoise = lua_toboolean(L, 4);
    bool is_slave = lua_toboolean(L, 5);
    bool is_farmer = lua_toboolean(L, 6);
    bool is_laborer = lua_toboolean(L, 7);
    if(is_burgeoise) pop_type.group = PopGroup::BURGEOISE;
    else if(is_slave) pop_type.group = PopGroup::SLAVE;
    else if(is_farmer) pop_type.group = PopGroup::FARMER;
    else if(is_laborer) pop_type.group = PopGroup::LABORER;
    else pop_type.group = PopGroup::OTHER;

    pop_type.basic_needs_amount.resize(g_world.goods.size(), 0.f);
    pop_type.luxury_needs_satisfaction.resize(g_world.goods.size(), 0.f);
    pop_type.luxury_needs_deminishing_factor.resize(g_world.goods.size(), 0.f);

    // Lua next = pops top and then pushes key & value in table
    lua_pushvalue(L, 8);
    lua_pushnil(L);
    while(lua_next(L, -2)) {
        lua_pushnil(L);
        lua_next(L, -2);
        const Good& good = find_or_throw_local<Good>(pop_string(L));
        lua_next(L, -2);
        const float amount = pop_number(L);
        lua_pop(L, 2);
        pop_type.basic_needs_amount[good] = amount;
    }
    lua_pop(L, 1);

    lua_pushvalue(L, 9);
    lua_pushnil(L);
    while(lua_next(L, -2)) {
        lua_pushnil(L);
        lua_next(L, -2);
        const Good& good = find_or_throw_local<Good>(pop_string(L));
        lua_next(L, -2);
        const float satisfaction = pop_number(L);
        lua_next(L, -2);
        const float deminishing = pop_number(L);
        lua_pop(L, 2);
        pop_type.luxury_needs_satisfaction[g_world.get_id(good)] = satisfaction;
        pop_type.luxury_needs_deminishing_factor[g_world.get_id(good)] = deminishing;
    }
    lua_pop(L, 1);

    g_world.insert(pop_type);
    lua_pushnumber(L, g_world.pop_types.size() - 1);
    return 1;
}

int LuaAPI::get_pop_type(lua_State* L) {
    const auto& pop_type = find_or_throw_local<PopType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, (size_t)g_world.get_id(pop_type));
    lua_pushstring(L, pop_type.name.c_str());
    lua_pushnumber(L, pop_type.social_value);
    lua_pushboolean(L, pop_type.group == PopGroup::BURGEOISE);
    lua_pushboolean(L, pop_type.group == PopGroup::SLAVE);
    lua_pushboolean(L, pop_type.group == PopGroup::FARMER);
    lua_pushboolean(L, pop_type.group == PopGroup::LABORER);
    lua_newtable(L);
    size_t index = 1;
    for(size_t i = 0; i < pop_type.basic_needs_amount.size(); i++) {
        if(pop_type.basic_needs_amount[i] != 0) {
            lua_pushnumber(L, index++);

            lua_newtable(L);
            append_to_table(L, 1, g_world.goods[i].ref_name.c_str());
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
            append_to_table(L, 1, g_world.goods[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.luxury_needs_satisfaction[i]);
            append_to_table(L, 3, pop_type.luxury_needs_deminishing_factor[i]);

            lua_settable(L, -3);
        }
    }
    return 9;
}

int LuaAPI::get_pop_type_by_id(lua_State* L) {
    const PopType& pop_type = g_world.pop_types.at(lua_tonumber(L, 1));
    lua_pushstring(L, pop_type.ref_name.c_str());
    lua_pushstring(L, pop_type.name.c_str());
    lua_pushnumber(L, pop_type.social_value);
    lua_pushboolean(L, pop_type.group == PopGroup::BURGEOISE);
    lua_pushboolean(L, pop_type.group == PopGroup::SLAVE);
    lua_pushboolean(L, pop_type.group == PopGroup::FARMER);
    lua_pushboolean(L, pop_type.group == PopGroup::LABORER);
    lua_newtable(L);
    size_t index = 1;
    for(size_t i = 0; i < pop_type.basic_needs_amount.size(); i++) {
        if(pop_type.basic_needs_amount[i] != 0) {
            lua_pushnumber(L, index++);

            lua_newtable(L);
            append_to_table(L, 1, g_world.goods[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.basic_needs_amount[i]);

            lua_settable(L, -3);
        }
    }
    index = 1;
    for(size_t i = 0; i < pop_type.luxury_needs_satisfaction.size(); i++) {
        if(pop_type.luxury_needs_satisfaction[i] != 0) {
            lua_pushnumber(L, index++);

            lua_newtable(L);
            append_to_table(L, 1, g_world.goods[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.luxury_needs_satisfaction[i]);
            append_to_table(L, 3, pop_type.luxury_needs_deminishing_factor[i]);

            lua_settable(L, -3);
        }
    }
    return 9;
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
    const auto& language = find_or_throw_local<Language>(luaL_checkstring(L, 1));

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
    const auto& religion = find_or_throw_local<Religion>(luaL_checkstring(L, 1));
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
    const auto unit_type = find_or_throw_local<UnitType>(luaL_checkstring(L, 1));

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
    auto& good = g_world.goods.at(lua_tonumber(L, 2));
    size_t amount = lua_tonumber(L, 3);
    unit_type.req_goods.emplace_back(good, amount);
    return 0;
}

int LuaAPI::add_ideology(lua_State* L) {
    if(g_world.needs_to_sync)
        luaL_error(L, "MP-Sync in this function is not supported");

    Ideology ideology{};
    ideology.ref_name = luaL_checkstring(L, 1);
    ideology.name = luaL_checkstring(L, 2);
    ideology.color = (std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 3))) >> 8) | 0xff000000;
    g_world.insert(ideology);
    lua_pushnumber(L, g_world.ideologies.size() - 1);
    return 1;
}

int LuaAPI::get_ideology(lua_State* L) {
    const auto& ideology = find_or_throw_local<Ideology>(luaL_checkstring(L, 1));
    lua_pushnumber(L, (size_t)g_world.get_id(ideology));
    lua_pushstring(L, ideology.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((ideology.color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::get_ideology_by_id(lua_State* L) {
    const auto& ideology = g_world.ideologies.at(lua_tonumber(L, 1));
    lua_pushstring(L, ideology.ref_name.c_str());
    lua_pushstring(L, ideology.name.c_str());
    lua_pushnumber(L, std::byteswap<std::uint32_t>((ideology.color & 0x00ffffff) << 8));
    return 3;
}

static int traceback(lua_State* L) {
    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");
    lua_pushvalue(L, 1);
    lua_pushinteger(L, 1);
    lua_call(L, 2, 1);
    return 1;
}

int call_func(lua_State* L, int nargs, int nret) {
    /* calculate stack position for message handler */
    int hpos = lua_gettop(L) - nargs;
    int ret = 0;
    /* push custom error message handler */
    lua_pushcfunction(L, traceback);
    /* move it before function and arguments */
    lua_insert(L, hpos);
    /* call lua_pcall function with custom handler */
    ret = lua_pcall(L, nargs, nret, hpos);
    /* remove custom error message handler from stack */
    lua_remove(L, hpos);
    /* pass return value of lua_pcall */
    return ret;
}

// Checks all events and their condition functions
void LuaAPI::check_events(lua_State* L) {
    for(auto& event : g_world.events) {
        if(event.checked) continue;
        bool is_multi = true;
        bool has_fired = false;
        for(auto& nation : event.receivers) {
            assert(nation != nullptr);
            if(!nation->exists()) continue;
            lua_rawgeti(L, LUA_REGISTRYINDEX, event.conditions_function);
            lua_pushstring(L, nation->ref_name.c_str());
            lua_pcall(L, 1, 1, 0);
            bool r = lua_toboolean(L, -1);
            lua_pop(L, 1);

            // Conditions met
            if(r) {
                has_fired = true;

                // Save the original event & momentarily replace it on the big world
                auto orig_event = Event(event);

                // Call the "do event" function
                Eng3D::Log::debug("event", "Event " + event.ref_name + " using " + std::to_string(event.do_event_function) + " function");
                lua_rawgeti(L, LUA_REGISTRYINDEX, event.do_event_function);
                lua_pushstring(L, nation->ref_name.c_str());
                if(call_func(L, 1, 1)) {
                    Eng3D::Log::error("lua", translate_format("lua_pcall failed: %s", lua_tostring(L, -1)));
                    lua_pop(L, 1);
                    goto restore_original;
                }
                is_multi = lua_tointeger(L, -1);
                lua_pop(L, 1);

                {
                    // The changes done to the event "locally" are then created into a new local event
                    auto local_event = Event(event);
                    local_event.cached_id = Event::invalid();
                    local_event.ref_name = Eng3D::StringRef(local_event.ref_name + "_local_" + nation->ref_name);
                    // Do not relaunch a local event
                    local_event.checked = true;
                    if(local_event.decisions.empty()) {
                        Eng3D::Log::error("event", "Event " + local_event.ref_name + " has no decisions (ref_name = " + nation->ref_name + ")");
                    } else {
                        // Check that descisions have functions
                        for(const auto& descision : local_event.decisions) {
                            if(descision.do_decision_function == 0) {
                                Eng3D::Log::error("lua", "Lua event " + orig_event.ref_name + " on descision " + descision.ref_name + " failed");
                                goto restore_original;
                            }
                        }
                        nation->inbox.push_back(local_event);
                        Eng3D::Log::debug("event", "Event triggered! " + local_event.ref_name + " (with " + std::to_string(local_event.decisions.size()) + " decisions)");
                    }
                }

            restore_original:
                // Original event then gets restored
                event = orig_event;
            }
        }

        // Event is marked as checked if it's not of multiple occurences
        if(has_fired && !is_multi)
            event.checked = true;
    }

    // Do decisions taken effects in the queue, then clear it awaiting
    // other taken decisions :)
    for(auto& dec : g_world.taken_decisions) {
        Eng3D::Log::debug("event", dec.second->ref_name + " took the decision " + std::to_string(dec.first.do_decision_function));

        lua_rawgeti(L, LUA_REGISTRYINDEX, dec.first.do_decision_function);
        lua_pushstring(L, dec.second->ref_name.c_str());
        if(call_func(L, 1, 0)) {
            const std::string err_msg = lua_tostring(L, -1);
            Eng3D::Log::error("lua", "lua_pcall failed: " + err_msg);
            lua_pop(L, 1);
            CXX_THROW(LuaAPI::Exception, std::to_string(dec.first.do_decision_function) + "(" + dec.second->ref_name + "): " + err_msg);
        }
    }
    g_world.taken_decisions.clear();
}

#include <unordered_map>
#include <map>
#include "eng3d/state.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/barchart.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/chart.hpp"
#include "eng3d/ui/checkbox.hpp"
#include "eng3d/ui/close_button.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/group.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/input.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/piechart.hpp"
#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/ui/slider.hpp"
#include "eng3d/ui/text.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/window.hpp"

std::map<int, UI::Widget*> lua_widgets;
std::map<int, std::shared_ptr<Eng3D::Texture>> lua_textures;
std::map<std::string, int> lua_ui_callbacks; 

// TODO: Make this thread-safe
static int id = 1;
static inline int get_unique_id() {
    id++;
    return id;
}

int LuaAPI::ui_new_button(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Button(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_new_image(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Image(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_new_group(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Group(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_new_div(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Div(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_new_window(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Window(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_new_checkbox(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Checkbox(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_set_checkbox_value(lua_State* L) {
    UI::Checkbox* widget = static_cast<UI::Checkbox*>(lua_widgets[luaL_checkinteger(L, 1)]);
    widget->set_value(lua_toboolean(L, 2));
    return 0;
}

int LuaAPI::ui_get_checkbox_value(lua_State* L) {
    UI::Checkbox* widget = static_cast<UI::Checkbox*>(lua_widgets[luaL_checkinteger(L, 1)]);
    lua_pushboolean(L, widget->get_value());
    return 1;
}

int LuaAPI::ui_new_slider(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Slider(x, y, w, h, 0.f, 1.f, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_get_slider_value(lua_State* L) {
    UI::Slider* widget = static_cast<UI::Slider*>(lua_widgets[luaL_checkinteger(L, 1)]);
    lua_pushnumber(L, widget->get_value());
    return 1;
}

int LuaAPI::ui_set_slider_value(lua_State* L) {
    UI::Slider* widget = static_cast<UI::Slider*>(lua_widgets[luaL_checkinteger(L, 1)]);
    widget->set_value(luaL_checknumber(L, 2));
    return 0;
}

int LuaAPI::ui_new_label(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int parent_ref = luaL_checkinteger(L, 3); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Label(x, y, " ", parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_set_text(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    widget->text(luaL_checkstring(L, 2));
    return 0;
}

int LuaAPI::ui_get_image(lua_State* L) {
    const std::string path = luaL_checkstring(L, 1);
    auto& s = Eng3D::State::get_instance();

    const auto widget_id = get_unique_id();
    lua_textures[widget_id] = s.tex_man.load(s.package_man.get_unique(path));
    lua_pushinteger(L, widget_id);
    return 1;
}

int LuaAPI::ui_set_image(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    auto tex = lua_textures[luaL_checkinteger(L, 2)];
    widget->current_texture = tex;
    return 0;
}

int LuaAPI::ui_set_scroll(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    widget->is_scroll = lua_toboolean(L, 2);
    return 0;
}

int LuaAPI::ui_set_on_click(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    lua_pushvalue(L, 2); // Obtain closure id
    widget->lua_on_click = luaL_ref(L, LUA_REGISTRYINDEX);
    widget->set_on_click([L](UI::Widget& w) { // Special callback for handling this closure
        lua_getglobal(L, "UI_DriverCallOnClick");
        lua_rawgeti(L, LUA_REGISTRYINDEX, w.lua_on_click);
        // Find the widget on the map
        auto it = std::find_if(lua_widgets.begin(), lua_widgets.end(), [&w](const auto& e) {
            return e.second == &w;
        });
        lua_pushinteger(L, it->first);
        if(call_func(L, 2, 0)) {
            const std::string err_msg = lua_tostring(L, -1);
            Eng3D::Log::error("lua", "lua_pcall failed: " + err_msg);
            lua_pop(L, 1);
            CXX_THROW(LuaAPI::Exception, "Failure on UI callback: " + err_msg);
        }
    });
    return 0;
}

int LuaAPI::ui_set_window_on_click_close_btn(lua_State* L) {
    UI::Window* widget = static_cast<UI::Window*>(lua_widgets[luaL_checkinteger(L, 1)]);
    lua_pushvalue(L, 2); // Obtain closure id
    widget->lua_on_close_btn = luaL_ref(L, LUA_REGISTRYINDEX);
    widget->set_close_btn_function([L](UI::Widget& w) { // Special callback for handling this closure
        auto& o = (UI::Window&)(*w.parent->parent);
        lua_getglobal(L, "UI_DriverCallOnClick");
        lua_rawgeti(L, LUA_REGISTRYINDEX, o.lua_on_close_btn);
        // Find the widget on the map
        auto it = std::find_if(lua_widgets.begin(), lua_widgets.end(), [&o](const auto& e) {
            return e.second == &o;
        });
        lua_pushinteger(L, it->first);
        if(call_func(L, 2, 0)) {
            const std::string err_msg = lua_tostring(L, -1);
            Eng3D::Log::error("lua", "lua_pcall failed: " + err_msg);
            lua_pop(L, 1);
            CXX_THROW(LuaAPI::Exception, "Failure on UI callback: " + err_msg);
        }
        o.kill(); // Implicitly kill object
    });
    return 0;
}

int LuaAPI::ui_get_widget(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    lua_pushinteger(L, widget->width);
    lua_pushinteger(L, widget->height);
    lua_pushinteger(L, widget->x);
    lua_pushinteger(L, widget->y);
    return 4;
}

int LuaAPI::ui_widget_kill(lua_State* L) {
    const int widget_id = luaL_checkinteger(L, 1);
    UI::Widget* widget = lua_widgets[widget_id];
    widget->kill(); // Kill the widget (not now, but later...)
    lua_widgets.erase(widget_id); // Delete from lua_widgets to avoid dead-pointer referencing
    return 0;
}

int LuaAPI::ui_widget_set_tooltip(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    widget->set_tooltip(luaL_checkstring(L, 2));
    return 0;
}

int LuaAPI::ui_register_callback(lua_State* L) {
    const std::string name = luaL_checkstring(L, 1);
    lua_pushvalue(L, 2); // Obtain closure id
    lua_ui_callbacks[name] = luaL_ref(L, LUA_REGISTRYINDEX);
    // Now the UI callback is registered
    return 0;
}

int LuaAPI::ui_widget_set_flex(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    int flex_mode = luaL_checkinteger(L, 2);
    if(flex_mode == 1) {
        widget->flex = UI::Flex::COLUMN;
    } else if(flex_mode == 2) {
        widget->flex = UI::Flex::ROW;
    } else {
        widget->flex = UI::Flex::NONE;
    }
    return 0;
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

/// @brief Some UI functions are hardcoded, for example the main menu is hardcoded
/// to appear when the game starts, in order to mantain scriptability we just invoke
/// functions coded in lua
/// @param L 
/// @param name 
/// @return int 
void LuaAPI::invoke_registered_callback(lua_State* L, const std::string& name) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ui_callbacks[name]);
    if(call_func(L, 0, 0)) {
        const std::string err_msg = lua_tostring(L, -1);
        Eng3D::Log::error("lua", "lua_pcall failed: " + err_msg);
        lua_pop(L, 1);
        CXX_THROW(LuaAPI::Exception, "Failure on UI callback: " + err_msg);
    }
}
