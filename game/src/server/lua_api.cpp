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

#include "eng3d/path.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/decimal.hpp"
#include "eng3d/log.hpp"
#include "eng3d/assert.hpp"

#include "server/lua_api.hpp"
#include "world.hpp"
#include "nation.hpp"
#include "server/economy.hpp"
#include "event.hpp"
#include "building.hpp"

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
        CXX_THROW(LuaAPI::Exception, std::string() + "Object<" + typeid(T).name() + "> " + ref_name + " not found");
    return (*result);
}

template<typename T>
const T& find_or_throw_local(const std::string& ref_name) {
    const auto& list = World::get_instance().get_list((T*)nullptr);
    const auto result = std::find_if(list.begin(), list.end(), [&ref_name](const auto& o) {
        return (o.ref_name == ref_name);
    });

    if(result == list.end())
        CXX_THROW(LuaAPI::Exception, std::string() + "Object<" + typeid(T).name() + "> " + ref_name + " not found");
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
        CXX_THROW(LuaAPI::Exception, "Expected a text but got empty string");
    lua_pop(L, 1);
    return text;
}

int LuaAPI::register_new_table(lua_State* L, const std::string& name, const luaL_Reg meta[], const luaL_Reg methods[]) {
    luaL_newlib(L, methods);
    luaL_newmetatable(L, name.c_str());
    luaL_setfuncs(L, methods, 0);
    lua_setmetatable(L, -2);
    lua_setglobal(L, name.c_str());
    return 0;
}

int LuaAPI::add_terrain_type(lua_State* L) {
    if(g_world->needs_to_sync)
        CXX_THROW(LuaAPI::Exception, "MP-Sync in this function is not supported");

    TerrainType terrain_type{};
    terrain_type.ref_name = luaL_checkstring(L, 1);
    terrain_type.name = luaL_checkstring(L, 2);
    terrain_type.color = bswap32(lua_tonumber(L, 3)) >> 8;
    terrain_type.color |= 0xff000000;
    terrain_type.movement_penalty = (lua_tonumber(L, 4));
    terrain_type.is_water_body = lua_toboolean(L, 5);
    g_world->insert(terrain_type);
    lua_pushnumber(L, g_world->terrain_types.size() - 1);
    return 1;
}

int LuaAPI::get_terrain_type_by_id(lua_State* L) {
    const auto& terrain_type = g_world->terrain_types.at(lua_tonumber(L, 1));

    lua_pushstring(L, terrain_type.ref_name.c_str());
    lua_pushstring(L, terrain_type.name.c_str());
    lua_pushnumber(L, bswap32((terrain_type.color & 0x00ffffff) << 8));
    lua_pushnumber(L, terrain_type.movement_penalty);
    lua_pushboolean(L, terrain_type.is_water_body);
    return 5;
}

int LuaAPI::get_terrain_type(lua_State* L) {
    const auto& terrain_type = find_or_throw_local<TerrainType>(luaL_checkstring(L, 1));
    lua_pushnumber(L, g_world->get_id(terrain_type));
    lua_pushstring(L, terrain_type.name.c_str());
    lua_pushnumber(L, bswap32((terrain_type.color & 0x00ffffff) << 8));
    lua_pushnumber(L, terrain_type.movement_penalty);
    lua_pushboolean(L, terrain_type.is_water_body);
    return 5;
}

int LuaAPI::set_nation_mod_to_invention(lua_State* L) {
    Technology* technology = &g_world->technologies.at(lua_tonumber(L, 1));
    technology->modifiers.push_back(&g_world->nation_modifiers.at(lua_tonumber(L, 2)));
    return 0;
}

int LuaAPI::add_technology(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");

    Technology technology{};
    technology.ref_name = luaL_checkstring(L, 1);
    technology.name = luaL_checkstring(L, 2);
    technology.description = lua_tostring(L, 3);
    technology.cost = (lua_tonumber(L, 4));
    technology.type = (TechnologyType)((int)lua_tonumber(L, 5));
    g_world->insert(technology);
    lua_pushnumber(L, g_world->technologies.size() - 1);
    return 1;
}

int LuaAPI::get_technology(lua_State* L) {
    const auto& technology = find_or_throw_local<Technology>(luaL_checkstring(L, 1));
    lua_pushnumber(L, g_world->get_id(technology));
    lua_pushstring(L, technology.name.c_str());
    lua_pushstring(L, technology.description.c_str());
    lua_pushnumber(L, technology.cost);
    lua_pushnumber(L, technology.type);
    return 5;
}

int LuaAPI::add_req_tech_to_tech(lua_State* L) {
    Technology* technology = &g_world->technologies.at(lua_tonumber(L, 1));
    Technology::Id req_tech_id = lua_tonumber(L, 2);
    assert(req_tech_id < g_world->technologies.size());
    technology->req_technologies.push_back(req_tech_id);
    return 0;
}

int LuaAPI::add_building_type(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");

    BuildingType building_type{};
    building_type.ref_name = luaL_checkstring(L, 1);
    building_type.name = luaL_checkstring(L, 2);
    building_type.can_plot_on_sea(lua_toboolean(L, 3));
    building_type.can_build_land_units(lua_toboolean(L, 4));
    building_type.can_build_naval_units(lua_toboolean(L, 5));
    g_world->insert(building_type);
    lua_pushnumber(L, g_world->building_types.size() - 1);
    return 1;
}

int LuaAPI::get_building_type(lua_State* L) {
    const auto& building_type = find_or_throw_local<BuildingType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(building_type));
    lua_pushstring(L, building_type.ref_name.c_str());
    lua_pushstring(L, building_type.name.c_str());
    lua_pushboolean(L, building_type.can_plot_on_sea());
    lua_pushboolean(L, building_type.can_build_land_units());
    lua_pushboolean(L, building_type.can_build_naval_units());
    lua_pushnumber(L, 0.f);
    return 7;
}

int LuaAPI::add_good(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");

    Good good{};
    good.ref_name = luaL_checkstring(L, 1);
    good.name = luaL_checkstring(L, 2);
    g_world->insert(good);
    lua_pushnumber(L, g_world->goods.size() - 1);
    return 1;
}

int LuaAPI::get_good(lua_State* L) {
    const auto& good = find_or_throw_local<Good>(luaL_checkstring(L, 1));
    lua_pushnumber(L, g_world->get_id(good));
    lua_pushstring(L, good.name.c_str());
    return 2;
}

int LuaAPI::add_input_to_industry_type(lua_State* L) {
    BuildingType& industry_type = g_world->building_types.at(lua_tonumber(L, 1));
    Good& good = g_world->goods.at(lua_tonumber(L, 2));
    industry_type.inputs.push_back(&good);
    industry_type.num_req_workers += industry_type.inputs.size() * 100;
    return 0;
}

int LuaAPI::add_output_to_industry_type(lua_State* L) {
    auto& industry_type = g_world->building_types.at(lua_tonumber(L, 1));
    auto& good = g_world->goods.at(lua_tonumber(L, 2));
    if(industry_type.output != nullptr)
        throw LuaAPI::Exception("Already have an output for " + industry_type.ref_name);
    industry_type.output = &good;
    industry_type.num_req_workers += 100;
    return 0;
}

int LuaAPI::add_req_good_to_industry_type(lua_State* L) {
    auto& industry_type = g_world->building_types.at(lua_tonumber(L, 1));
    auto& good = g_world->goods.at(lua_tonumber(L, 2));
    industry_type.req_goods.push_back(std::make_pair(&good, lua_tonumber(L, 3)));
    return 0;
}

int LuaAPI::add_req_technology_to_industry_type(lua_State* L) {
    auto& industry_type = g_world->building_types.at(lua_tonumber(L, 1));
    auto& technology = g_world->technologies.at(lua_tonumber(L, 2));
    industry_type.req_technologies.push_back(&technology);
    return 0;
}

int LuaAPI::add_nation(lua_State* L) {
    if(g_world->needs_to_sync)
        CXX_THROW(LuaAPI::Exception, "MP-Sync in this function is not supported");

    Nation nation{};
    nation.ref_name = luaL_checkstring(L, 1);
    nation.name = luaL_checkstring(L, 2);
    nation.ideology = &g_world->ideologies.at(0);
    nation.religion_discrim.resize(g_world->religions.size(), 0.5f);
    nation.religion_discrim.shrink_to_fit();
    nation.culture_discrim.resize(g_world->cultures.size(), 0.5f);
    nation.culture_discrim.shrink_to_fit();
    nation.client_hints.resize(g_world->ideologies.size());
    nation.client_hints.shrink_to_fit();
    for(const auto& technology : g_world->technologies)
        nation.research.push_back(technology.cost);
    nation.research.shrink_to_fit();

    // Check for duplicates
    for(const auto& other_nation : g_world->nations) {
        if(Nation::is_invalid(other_nation.get_id()))
            CXX_THROW(LuaAPI::Exception, "Nation with invalid Id!");
        if(nation.ref_name == other_nation.ref_name)
            CXX_THROW(LuaAPI::Exception, "Duplicate ref_name " + nation.ref_name);
    }

    g_world->insert(nation);
    lua_pushnumber(L, g_world->nations.size() - 1);
    return 1;
}

int LuaAPI::get_nation(lua_State* L) {
    const auto& nation = find_or_throw_local<Nation>(luaL_checkstring(L, 1));
    lua_pushnumber(L, g_world->get_id(nation));
    lua_pushstring(L, nation.name.c_str());
    return 2;
}

int LuaAPI::get_nation_by_id(lua_State* L) {
    const auto& nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_pushstring(L, nation.name.c_str());
    lua_pushstring(L, nation.ref_name.c_str());
    return 2;
}

int LuaAPI::get_all_nations(lua_State* L) {
    lua_newtable(L);

    size_t i = 0;
    for(const auto& nation : g_world->nations) {
        assert(Nation::is_valid(nation.get_id()));
        lua_pushnumber(L, nation.get_id());
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::nation_declare_war_no_cb(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    Nation& other_nation = g_world->nations.at(lua_tonumber(L, 2));
    if(&nation == &other_nation)
        CXX_THROW(LuaAPI::Exception, nation.ref_name.get_string() + " can't declare war with itself");
    nation.declare_war(other_nation);
    return 0;
}

int LuaAPI::get_provinces_owned_by_nation(lua_State* L) {
    const auto& nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& province_id : nation.owned_provinces) {
        lua_pushnumber(L, province_id);
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_provinces_with_nucleus_by_nation(lua_State* L) {
    const auto& nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& province : g_world->provinces) {
        bool is_nuclei = false;
        for(const auto& nucleus_id : province.nuclei) {
            auto& nucleus = g_world->nations[nucleus_id];
            if(&nucleus == &nation) {
                is_nuclei = true;
                break;
            }
        }
        if(!is_nuclei) continue;
        lua_pushnumber(L, g_world->get_id(province));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::set_nation_primary_culture(lua_State* L) {
    return 0;
}

int LuaAPI::set_nation_capital(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    nation.capital_id = lua_tonumber(L, 2);
    return 0;
}

int LuaAPI::add_accepted_culture(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    nation.culture_discrim.at(lua_tonumber(L, 2)) = 1.f;
    return 0;
}

int LuaAPI::add_accepted_religion(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    nation.religion_discrim.at(lua_tonumber(L, 2)) = 1.f;
    return 0;
}

int LuaAPI::add_nation_client_hint(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    NationClientHint hint;
    hint.ideology = &g_world->ideologies.at(lua_tonumber(L, 2));
    hint.alt_name = luaL_checkstring(L, 3);
    hint.color = bswap32(lua_tonumber(L, 4)) >> 8;
    hint.color |= 0xff000000;
    nation.client_hints[g_world->get_id(*hint.ideology)] = hint;
    return 0;
}

int LuaAPI::get_nation_policies(lua_State* L) {
    const Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    // We are going to push everything in the policies structure
    // this is horrible - reflection may help in this case
    const Policies& policy = nation.current_policy;
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
    lua_pushnumber(L, (policy.domestic_import_tax));
    lua_pushnumber(L, (policy.domestic_export_tax));
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
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    // We are going to push everything in the policies structure
    // this is horrible - reflection may help in this case
    Policies& policy = nation.current_policy;
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
    policy.domestic_import_tax = (lua_tonumber(L, 20));
    policy.domestic_export_tax = (lua_tonumber(L, 21));
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
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    nation.ideology = &g_world->ideologies.at(lua_tonumber(L, 2));
    return 0;
}

int LuaAPI::get_nation_relation(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    Nation& other_nation = g_world->nations.at(lua_tonumber(L, 2));
    auto& relation = g_world->get_relation(g_world->get_id(nation), g_world->get_id(other_nation));
    lua_pushnumber(L, (relation.relation));
    lua_pushnumber(L, (relation.interest));
    lua_pushboolean(L, relation.has_embargo);
    lua_pushboolean(L, relation.has_war);
    lua_pushboolean(L, relation.has_alliance);
    lua_pushboolean(L, relation.has_defensive_pact);
    lua_pushboolean(L, relation.has_military_access);
    lua_pushboolean(L, relation.has_market_access);
    return 9;
}

int LuaAPI::set_nation_relation(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    Nation& other_nation = g_world->nations.at(lua_tonumber(L, 2));
    auto& relation = g_world->get_relation(g_world->get_id(nation), g_world->get_id(other_nation));
    relation.relation = (lua_tonumber(L, 3));
    relation.interest = (lua_tonumber(L, 4));
    relation.has_embargo = lua_toboolean(L, 5);
    relation.has_war = lua_toboolean(L, 6);
    relation.has_alliance = lua_toboolean(L, 7);
    relation.has_defensive_pact = lua_toboolean(L, 8);
    relation.has_military_access = lua_toboolean(L, 9);
    relation.has_market_access = lua_toboolean(L, 10);
    return 0;
}

int LuaAPI::nation_make_puppet(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    Nation& other_nation = g_world->nations.at(lua_tonumber(L, 2));
    other_nation.puppet_master = &nation;
    return 0;
}

int LuaAPI::nation_declare_unjustified_war(lua_State* L) {
    Nation& nation = g_world->nations.at(lua_tonumber(L, 1));
    Nation& other_nation = g_world->nations.at(lua_tonumber(L, 2));
    if(!g_world->get_relation(g_world->get_id(nation), g_world->get_id(other_nation)).has_war)
        nation.declare_war(other_nation);
    return 0;
}

int LuaAPI::add_nation_mod(lua_State* L) {
    if(g_world->needs_to_sync)
        CXX_THROW(LuaAPI::Exception, "MP-Sync in this function is not supported");

    NationModifier mod{};

    mod.ref_name = luaL_checkstring(L, 1);
    mod.name = luaL_checkstring(L, 2);

    mod.industry_output_mod = (lua_tonumber(L, 3));
    mod.industry_input_mod = (lua_tonumber(L, 4));
    mod.workers_needed_mod = (lua_tonumber(L, 5));
    mod.salary_paid_mod = (lua_tonumber(L, 6));
    mod.delivery_cost_mod = (lua_tonumber(L, 7));
    mod.literacy_learn_mod = (lua_tonumber(L, 8));
    mod.reproduction_mod = (lua_tonumber(L, 9));
    mod.death_mod = (lua_tonumber(L, 10));
    mod.militancy_mod = (lua_tonumber(L, 11));
    mod.life_needs_met_mod = (lua_tonumber(L, 13));
    mod.everyday_needs_met_mod = (lua_tonumber(L, 14));
    mod.luxury_needs_met_mod = (lua_tonumber(L, 15));

    g_world->insert(mod);
    lua_pushnumber(L, g_world->get_id(mod));
    return 1;
}

int LuaAPI::get_nation_mod(lua_State* L) {
    const auto& mod = find_or_throw_local<NationModifier>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(mod));
    lua_pushstring(L, mod.name.c_str());
    lua_pushnumber(L, mod.industry_output_mod);
    lua_pushnumber(L, mod.industry_input_mod);
    lua_pushnumber(L, mod.workers_needed_mod);
    lua_pushnumber(L, mod.salary_paid_mod);
    lua_pushnumber(L, mod.delivery_cost_mod);
    lua_pushnumber(L, mod.literacy_learn_mod);
    lua_pushnumber(L, mod.reproduction_mod);
    lua_pushnumber(L, mod.death_mod);
    lua_pushnumber(L, mod.militancy_mod);
    lua_pushnumber(L, mod.life_needs_met_mod);
    lua_pushnumber(L, mod.everyday_needs_met_mod);
    lua_pushnumber(L, mod.luxury_needs_met_mod);
    return 14;
}

int LuaAPI::add_province(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");

    Province province{};
    province.ref_name = luaL_checkstring(L, 1);
    province.color = (bswap32(lua_tonumber(L, 2)) >> 8) | 0xff000000;
    province.name = luaL_checkstring(L, 3);
    province.terrain_type = &g_world->terrain_types.at(lua_tonumber(L, 4));
    // load rgo_size
    province.rgo_size.resize(g_world->goods.size(), 0);
    province.rgo_size.shrink_to_fit();
    lua_pushvalue(L, 5);
    lua_pushnil(L);
    while(lua_next(L, -2)) {
        lua_pushnil(L);
        lua_next(L, -2);
        const Good& good = find_or_throw_local<Good>(pop_string(L));
        lua_next(L, -2);
        const uint32_t amount = (uint32_t)pop_number(L);
        lua_pop(L, 2);
        province.rgo_size[g_world->get_id(good)] = amount;
    }
    lua_pop(L, 1);

    // Check for duplicates
    for(size_t i = 0; i < g_world->provinces.size(); i++) {
        if(province.color == g_world->provinces[i].color) {
            CXX_THROW(LuaAPI::Exception, province.ref_name + " province has same color as " + g_world->provinces[i].ref_name);
        } else if(province.ref_name == g_world->provinces[i].ref_name) {
            CXX_THROW(LuaAPI::Exception, "Duplicate ref_name " + province.ref_name);
        }
    }
    
    province.products.resize(g_world->goods.size());
    province.products.shrink_to_fit();
    province.buildings.resize(g_world->building_types.size());
    province.buildings.shrink_to_fit();
    for(const auto& building_type : g_world->building_types) {
        province.buildings[g_world->get_id(building_type)].stockpile.resize(building_type.inputs.size(), 0);
        province.buildings[g_world->get_id(building_type)].stockpile.shrink_to_fit();
    }
    // Set bounding box of province to the whole world (will later be resized at the bitmap-processing step)
    province.box_area = Eng3D::Rect(0, 0, std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max());
    province.pops.reserve(100);
    g_world->insert(province);
    lua_pushnumber(L, g_world->provinces.size() - 1);
    return 1;
}

int LuaAPI::update_province(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    province.ref_name = luaL_checkstring(L, 2);
    province.color = (bswap32(lua_tonumber(L, 3)) >> 8) | 0xff000000;
    province.name = luaL_checkstring(L, 4);
    province.terrain_type = &g_world->terrain_types.at(lua_tonumber(L, 5));
    // Check for duplicates
    for(size_t i = 0; i < g_world->provinces.size(); i++) {
        if(province.color == g_world->provinces[i].color) {
            CXX_THROW(LuaAPI::Exception, province.ref_name + " province has same color as " + g_world->provinces[i].ref_name);
        } else if(province.ref_name == g_world->provinces[i].ref_name) {
            CXX_THROW(LuaAPI::Exception, "Duplicate ref_name " + province.ref_name);
        }
    }
    return 0;
}

int LuaAPI::get_province(lua_State* L) {
    const Province& province = find_or_throw_local<Province>(luaL_checkstring(L, 1));
    lua_pushnumber(L, g_world->get_id(province));
    lua_pushstring(L, province.name.c_str());
    lua_pushnumber(L, bswap32((province.color & 0x00ffffff) << 8));
    lua_pushnumber(L, g_world->get_id(*province.terrain_type));
    lua_newtable(L);
    size_t index = 1;
    for(size_t i = 0; i < province.rgo_size.size(); i++) {
        if(province.rgo_size[i] != 0) {
            lua_pushnumber(L, index++);
            lua_newtable(L);
            append_to_table(L, 1, g_world->goods[i].ref_name.c_str());
            append_to_table(L, 2, province.rgo_size[i]);
            lua_settable(L, -3);
        }
    }
    return 5;
}

int LuaAPI::get_province_by_id(lua_State* L) {
    const Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_pushstring(L, province.ref_name.c_str());
    lua_pushstring(L, province.name.c_str());
    lua_pushnumber(L, bswap32((province.color & 0x00ffffff) << 8));
    lua_pushnumber(L, g_world->get_id(*province.terrain_type));
    lua_newtable(L);
    size_t index = 1;
    for(size_t i = 0; i < province.rgo_size.size(); i++) {
        if(province.rgo_size[i] != 0) {
            lua_pushnumber(L, index++);
            lua_newtable(L);
            append_to_table(L, 1, g_world->goods[i].ref_name.c_str());
            append_to_table(L, 2, province.rgo_size[i]);
            lua_settable(L, -3);
        }
    }
    return 5;
}

int LuaAPI::province_add_unit(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    UnitType& unit_type = g_world->unit_types.at(lua_tonumber(L, 2));
    const size_t size = lua_tonumber(L, 3);

    Unit* unit = new Unit();
    unit->set_province(province);
    unit->type = &unit_type;
    unit->owner = province.owner;
    unit->budget = 5000.f;
    unit->experience = 1.f;
    unit->morale = 1.f;
    unit->supply = 1.f;
    unit->size = size;
    unit->base = unit->type->max_health;
    g_world->insert(*unit);
    return 0;
}

int LuaAPI::update_province_building(lua_State* L) {
    if(g_world->needs_to_sync)
        CXX_THROW(LuaAPI::Exception, "MP-Sync in this function is not supported");
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    // Add up a level of upgrade
    BuildingType* building_type = &g_world->building_types.at(lua_tonumber(L, 2));
    province.buildings[g_world->get_id(*building_type)].level = lua_tonumber(L, 3);
    province.buildings[g_world->get_id(*building_type)].budget += 1000.f;
    return 0;
}

int LuaAPI::give_province_to(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    g_world->nations.at(lua_tonumber(L, 2)).give_province(province);
    return 0;
}

int LuaAPI::give_hard_province_to(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    Nation& nation = g_world->nations.at(lua_tonumber(L, 2));
    for(auto& unit : g_world->units) {
        if(unit->province_id == g_world->get_id(province) && unit->owner == province.controller)
            unit->owner = &nation;
    }
    nation.give_province(province);
    return 0;
}

// Obtains the owner of a province (ref_name)
int LuaAPI::get_province_owner(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_pushstring(L, province.controller->ref_name.c_str());
    return 1;
}

// Get the country who owms a larger chunk of the province - this is not the same as owner
int LuaAPI::get_province_controller(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    if(province.controller == nullptr)
        lua_pushnumber(L, -1);
    else
        lua_pushnumber(L, g_world->get_id(*province.controller));
    return 1;
}

// Obtains the neighbours of a province (by ID)
int LuaAPI::get_province_neighbours(lua_State* L) {
    const Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_newtable(L);
    size_t i = 0;
    for(const auto& neighbour_id : province.neighbours) {
        lua_pushnumber(L, neighbour_id);
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_province_nuclei(lua_State* L) {
    const Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_newtable(L);
    size_t i = 0;
    for(const auto& nucleus_id : province.nuclei) {
        lua_pushnumber(L, nucleus_id);
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_province_pops_size(lua_State* L) {
    const Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_pushnumber(L, province.pops.size());
    return 1;
}

int LuaAPI::get_province_pop(lua_State* L) {
    const Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    const Pop& pop = province.pops.at(lua_tonumber(L, 2));
    lua_pushnumber(L, (pop.size));
    lua_pushnumber(L, (pop.budget));
    lua_pushnumber(L, (pop.literacy));
    lua_pushnumber(L, (pop.life_needs_met));
    lua_pushnumber(L, (pop.everyday_needs_met));
    lua_pushnumber(L, (pop.luxury_needs_met));
    lua_pushnumber(L, g_world->get_id(*pop.type));
    lua_pushnumber(L, g_world->get_id(*pop.culture));
    lua_pushnumber(L, g_world->get_id(*pop.religion));
    lua_pushnumber(L, g_world->get_id(pop.get_ideology()));
    lua_pushnumber(L, (pop.militancy));
    return 9;
}

int LuaAPI::set_province_pop(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    Pop& pop = province.pops.at(lua_tonumber(L, 2));
    pop.size = (lua_tonumber(L, 3));
    pop.budget = (lua_tonumber(L, 4));
    pop.literacy = (lua_tonumber(L, 5));
    pop.life_needs_met = (lua_tonumber(L, 6));
    pop.everyday_needs_met = (lua_tonumber(L, 7));
    pop.luxury_needs_met = (lua_tonumber(L, 8));
    pop.type = &g_world->pop_types.at(lua_tonumber(L, 9));
    pop.culture = &g_world->cultures.at(lua_tonumber(L, 10));
    pop.religion = &g_world->religions.at(lua_tonumber(L, 11));
    pop.militancy = (lua_tonumber(L, 12));
    return 0;
}

int LuaAPI::get_province_pop_ideology_approval(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    Pop& pop = province.pops.at(lua_tonumber(L, 2));
    lua_pushnumber(L, pop.ideology_approval.at(lua_tonumber(L, 3)));
    return 1;
}

int LuaAPI::set_province_pop_ideology_approval(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    Pop& pop = province.pops.at(lua_tonumber(L, 2));
    pop.ideology_approval.at(lua_tonumber(L, 3)) = lua_tonumber(L, 4);
    return 0;
}

int LuaAPI::add_province_pop(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    auto pop = Pop();
    pop.type = &g_world->pop_types.at(lua_tonumber(L, 2));
    pop.culture = &g_world->cultures.at(lua_tonumber(L, 3));
    pop.religion = &g_world->religions.at(lua_tonumber(L, 4));
    pop.size = (lua_tonumber(L, 5));
    pop.literacy = (lua_tonumber(L, 6));
    pop.budget = pop.size * 420.69f;

    /// @todo Make ideology NOT be random
    pop.ideology_approval.resize(g_world->ideologies.size(), 0.f);

    if(!pop.size)
        CXX_THROW(LuaAPI::Exception, "Can't create pops with 0 size");
    province.pops.push_back(pop);
    assert(province.pops.size() < 100);
    return 0;
}

int LuaAPI::rename_province(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    province.name = luaL_checkstring(L, 2);
    return 0;
}

int LuaAPI::add_province_nucleus(lua_State* L) {
    g_world->provinces.at(lua_tonumber(L, 1)).nuclei.insert(lua_tonumber(L, 2));
    return 0;
}

int LuaAPI::add_province_owner(lua_State* L) {
    Province& province = g_world->provinces.at(lua_tonumber(L, 1));
    Nation& nation = g_world->nations.at(lua_tonumber(L, 2));
    nation.give_province(province);
    return 0;
}

int LuaAPI::add_event(lua_State* L) {
    if(g_world->needs_to_sync)
        CXX_THROW(LuaAPI::Exception, "MP-Sync in this function is not supported");

    Event* event = new Event();

    event->ref_name = luaL_checkstring(L, 1);
    event->conditions_function = luaL_checkstring(L, 2);
    event->do_event_function = luaL_checkstring(L, 3);
    event->title = luaL_checkstring(L, 4);
    event->text = luaL_checkstring(L, 5);
    event->checked = lua_toboolean(L, 6);

    // Add onto vector
    g_world->insert(*event);
    lua_pushnumber(L, g_world->get_id(*event));
    return 1;
}

int LuaAPI::update_event(lua_State* L) {
    auto* event = g_world->events[lua_tonumber(L, 1)];
    event->ref_name = luaL_checkstring(L, 2);
    event->conditions_function = luaL_checkstring(L, 3);
    event->do_event_function = luaL_checkstring(L, 4);
    event->title = luaL_checkstring(L, 5);
    event->text = luaL_checkstring(L, 6);
    event->checked = lua_toboolean(L, 7);
    return 0;
}

int LuaAPI::get_event(lua_State* L) {
    const auto* event = find_or_throw<Event>(luaL_checkstring(L, 1));
    lua_pushnumber(L, g_world->get_id(*event));
    lua_pushstring(L, event->conditions_function.c_str());
    lua_pushstring(L, event->do_event_function.c_str());
    lua_pushstring(L, event->title.c_str());
    lua_pushstring(L, event->text.c_str());
    lua_pushboolean(L, event->checked);
    return 6;
}

int LuaAPI::add_event_receivers(lua_State* L) {
    // Add receivers of the event by id
    Event* event = g_world->events.at(lua_tonumber(L, 1));
    for(size_t i = 0; i < lua_tonumber(L, 2); i++)
        event->receivers.push_back(&g_world->nations.at(lua_tonumber(L, 3 + i)));
    return 0;
}

int LuaAPI::add_decision(lua_State* L) {
    Event* event = g_world->events.at(lua_tonumber(L, 1));

    Decision decision = Decision();
    decision.ref_name = luaL_checkstring(L, 2);
    decision.name = luaL_checkstring(L, 3);
    decision.do_decision_function = luaL_checkstring(L, 4);
    decision.effects = luaL_checkstring(L, 5);

    // Add onto vector
    event->decisions.push_back(decision);
    return 0;
}

int LuaAPI::add_pop_type(lua_State* L) {
    if(g_world->needs_to_sync)
        CXX_THROW(LuaAPI::Exception, "MP-Sync in this function is not supported");
    
    auto pop_type = PopType();
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

    pop_type.basic_needs_amount.resize(g_world->goods.size(), 0);
    pop_type.basic_needs_amount.shrink_to_fit();
    pop_type.luxury_needs_satisfaction.resize(g_world->goods.size(), 0);
    pop_type.luxury_needs_satisfaction.shrink_to_fit();
    pop_type.luxury_needs_deminishing_factor.resize(g_world->goods.size(), 0);
    pop_type.luxury_needs_deminishing_factor.shrink_to_fit();

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
        pop_type.basic_needs_amount[g_world->get_id(good)] = amount;
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
        pop_type.luxury_needs_satisfaction[g_world->get_id(good)] = satisfaction;
        pop_type.luxury_needs_deminishing_factor[g_world->get_id(good)] = deminishing;
    }
    lua_pop(L, 1);

    // Add onto vector
    g_world->insert(pop_type);
    lua_pushnumber(L, g_world->pop_types.size() - 1);
    return 1;
}

int LuaAPI::get_pop_type(lua_State* L) {
    const auto& pop_type = find_or_throw_local<PopType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(pop_type));
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
            append_to_table(L, 1, g_world->goods[i].ref_name.c_str());
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
            append_to_table(L, 1, g_world->goods[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.luxury_needs_satisfaction[i]);
            append_to_table(L, 3, pop_type.luxury_needs_deminishing_factor[i]);

            lua_settable(L, -3);
        }
    }
    return 9;
}

int LuaAPI::get_pop_type_by_id(lua_State* L) {
    const PopType& pop_type = g_world->pop_types.at(lua_tonumber(L, 1));
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
            append_to_table(L, 1, g_world->goods[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.basic_needs_amount[i]);

            lua_settable(L, -3);
        }
    }
    index = 1;
    for(size_t i = 0; i < pop_type.luxury_needs_satisfaction.size(); i++) {
        if(pop_type.luxury_needs_satisfaction[i] != 0) {
            lua_pushnumber(L, index++);

            lua_newtable(L);
            append_to_table(L, 1, g_world->goods[i].ref_name.c_str());
            append_to_table(L, 2, pop_type.luxury_needs_satisfaction[i]);
            append_to_table(L, 3, pop_type.luxury_needs_deminishing_factor[i]);

            lua_settable(L, -3);
        }
    }
    return 9;
}

int LuaAPI::add_culture(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");

    auto culture = Culture();
    culture.ref_name = luaL_checkstring(L, 1);
    culture.name = luaL_checkstring(L, 2);
    culture.color = (bswap32(lua_tonumber(L, 3)) >> 8) | 0xff000000;
    culture.adjective = luaL_checkstring(L, 4);
    culture.noun = luaL_checkstring(L, 5);
    culture.combo_form = luaL_checkstring(L, 6);

    g_world->insert(culture);
    lua_pushnumber(L, g_world->cultures.size() - 1);
    return 1;
}

int LuaAPI::get_culture(lua_State* L) {
    const auto& culture = find_or_throw_local<Culture>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(culture));
    lua_pushstring(L, culture.name.c_str());
    lua_pushnumber(L, bswap32((culture.color & 0x00ffffff) << 8));
    lua_pushstring(L, culture.adjective.c_str());
    lua_pushstring(L, culture.noun.c_str());
    lua_pushstring(L, culture.combo_form.c_str());
    return 6;
}

int LuaAPI::get_culture_by_id(lua_State* L) {
    const auto& culture = g_world->cultures.at(lua_tonumber(L, 1));
    lua_pushstring(L, culture.ref_name.c_str());
    lua_pushstring(L, culture.name.c_str());
    lua_pushnumber(L, bswap32((culture.color & 0x00ffffff) << 8));
    lua_pushstring(L, culture.adjective.c_str());
    lua_pushstring(L, culture.noun.c_str());
    lua_pushstring(L, culture.combo_form.c_str());
    return 6;
}

int LuaAPI::add_religion(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");

    auto religion = Religion();
    religion.ref_name = luaL_checkstring(L, 1);
    religion.name = luaL_checkstring(L, 2);
    religion.color = (bswap32(lua_tonumber(L, 3)) >> 8) | 0xff000000;
    g_world->insert(religion);
    lua_pushnumber(L, g_world->religions.size() - 1);
    return 1;
}

int LuaAPI::get_religion(lua_State* L) {
    const auto& religion = find_or_throw_local<Religion>(luaL_checkstring(L, 1));
    lua_pushnumber(L, g_world->get_id(religion));
    lua_pushstring(L, religion.name.c_str());
    lua_pushnumber(L, bswap32((religion.color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::get_religion_by_id(lua_State* L) {
    const auto& religion = g_world->religions.at(lua_tonumber(L, 1));
    lua_pushstring(L, religion.ref_name.c_str());
    lua_pushstring(L, religion.name.c_str());
    lua_pushnumber(L, bswap32((religion.color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::add_unit_type(lua_State* L) {
    if(g_world->needs_to_sync)
        CXX_THROW(LuaAPI::Exception, "MP-Sync in this function is not supported");

    auto unit_type = UnitType();
    unit_type.ref_name = luaL_checkstring(L, 1);
    unit_type.name = luaL_checkstring(L, 2);
    unit_type.attack = (lua_tonumber(L, 3));
    unit_type.defense = (lua_tonumber(L, 4));
    unit_type.max_health = (lua_tonumber(L, 5));
    unit_type.is_ground = lua_toboolean(L, 6);
    unit_type.is_naval = lua_toboolean(L, 7);
    unit_type.speed = (lua_tonumber(L, 8));
    g_world->insert(unit_type);
    lua_pushnumber(L, g_world->unit_types.size() - 1);
    return 1;
}

int LuaAPI::get_unit_type(lua_State* L) {
    const auto unit_type = find_or_throw_local<UnitType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(unit_type));
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
    UnitType& unit_type = g_world->unit_types.at(lua_tonumber(L, 1));
    Good& good = g_world->goods.at(lua_tonumber(L, 2));
    size_t amount = lua_tonumber(L, 3);
    unit_type.req_goods.push_back(std::make_pair(&good, amount));
    return 0;
}

int LuaAPI::add_ideology(lua_State* L) {
    if(g_world->needs_to_sync)
        CXX_THROW(LuaAPI::Exception, "MP-Sync in this function is not supported");

    auto ideology = Ideology();
    ideology.ref_name = luaL_checkstring(L, 1);
    ideology.name = luaL_checkstring(L, 2);
    ideology.color = (bswap32(lua_tonumber(L, 3)) >> 8) | 0xff000000;
    g_world->insert(ideology);
    lua_pushnumber(L, g_world->ideologies.size() - 1);
    return 1;
}

int LuaAPI::get_ideology(lua_State* L) {
    const auto& ideology = find_or_throw_local<Ideology>(luaL_checkstring(L, 1));
    lua_pushnumber(L, g_world->get_id(ideology));
    lua_pushstring(L, ideology.name.c_str());
    lua_pushnumber(L, bswap32((ideology.color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::get_ideology_by_id(lua_State* L) {
    const auto& ideology = g_world->ideologies.at(lua_tonumber(L, 1));
    lua_pushstring(L, ideology.ref_name.c_str());
    lua_pushstring(L, ideology.name.c_str());
    lua_pushnumber(L, bswap32((ideology.color & 0x00ffffff) << 8));
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
    for(auto& event : g_world->events) {
        assert(event != nullptr);
        if(event->checked) continue;
        bool is_multi = true;
        bool has_fired = false;
        for(auto& nation : event->receivers) {
            assert(nation != nullptr);
            if(!nation->exists()) continue;
            lua_getglobal(L, event->conditions_function.c_str());
            lua_pushstring(L, nation->ref_name.c_str());
            lua_pcall(L, 1, 1, 0);
            bool r = lua_toboolean(L, -1);
            lua_pop(L, 1);

            // Conditions met
            if(r) {
                has_fired = true;

                // Save the original event & momentarily replace it on the big world
                auto orig_event = Event(*event);

                // Call the "do event" function
                Eng3D::Log::debug("event", "Event " + event->ref_name + " using " + event->do_event_function + " function");
                lua_getglobal(L, event->do_event_function.c_str());
                lua_pushstring(L, nation->ref_name.c_str());
                if(call_func(L, 1, 1)) {
                    Eng3D::Log::error("lua", std::string() + "lua_pcall failed: " + lua_tostring(L, -1));
                    lua_pop(L, 1);
                    goto restore_original;
                }
                is_multi = lua_tointeger(L, -1);
                lua_pop(L, 1);

                {
                    // The changes done to the event "locally" are then created into a new local event
                    auto local_event = Event(*event);
                    local_event.cached_id = Event::invalid();
                    local_event.ref_name = Eng3D::StringRef(local_event.ref_name + "_local_" + nation->ref_name);
                    // Do not relaunch a local event
                    local_event.checked = true;
                    if(local_event.decisions.empty()) {
                        Eng3D::Log::error("event", "Event " + local_event.ref_name + " has no decisions (ref_name = " + nation->ref_name + ")");
                    } else {
                        // Check that descisions have functions
                        for(const auto& descision : local_event.decisions) {
                            if(descision.do_decision_function.get_string().empty()) {
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
                *event = orig_event;
            }
        }

        // Event is marked as checked if it's not of multiple occurences
        if(has_fired && !is_multi)
            event->checked = true;
    }

    // Do decisions taken effects in the queue, then clear it awaiting
    // other taken decisions :)
    for(auto& dec : g_world->taken_decisions) {
        Eng3D::Log::debug("event", dec.second->ref_name + " took the decision " + dec.first.do_decision_function);

        lua_getglobal(L, dec.first.do_decision_function.c_str());
        lua_pushstring(L, dec.second->ref_name.c_str());
        if(call_func(L, 1, 0)) {
            const std::string err_msg = lua_tostring(L, -1);
            Eng3D::Log::error("lua", "lua_pcall failed: " + err_msg);
            lua_pop(L, 1);
            throw LuaAPI::Exception(dec.first.do_decision_function + "(" + dec.second->ref_name + "): " + err_msg);
        }
    }
    g_world->taken_decisions.clear();
}