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

#ifdef windows
extern "C" {
#	include <lua.h>
}
#else
#  ifdef LUA54
#    include <lua5.4/lua.hpp>
#  else
#    include <lua5.3/lua.hpp>
#  endif
#endif
#include <cstring>
#include <cstdlib>
#ifdef windows
#	define bswap_32(x) _byteswap_ulong(x)
#	define bswap_64(x) _byteswap_uint64(x)
#else
#	include <byteswap.h>
#endif

#include "server/lua_api.hpp"
#include "world.hpp"
#include "nation.hpp"
#include "server/economy.hpp"
#include "unified_render/print.hpp"
#include "unified_render/path.hpp"
#include "event.hpp"
#include "building.hpp"

// !!! IMPORTANT !!!
// Doing changes to the world state (like Nation and Province) does NOT require an explicit update as this is done
// after each economical tick
// HOWEVER, adding new elements or changing other states REQUIRES a explicity synchronization!!

template<typename T>
const T* find_or_throw(const std::string& ref_name) {
    const T* obj_to_find = nullptr;

    const auto& list = World::get_instance().get_list(obj_to_find);
    const auto result = std::find_if(list.begin(), list.end(), [&ref_name](const auto& o) {
        return (o->ref_name == ref_name);
    });

    if(result == list.end()) {
        throw LuaAPI::Exception(ref_name + " not found");
    }
    return (*result);
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
    if(g_world->needs_to_sync) {
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    }

    auto* terrain_type = new TerrainType();

    terrain_type->ref_name = luaL_checkstring(L, 1);
    terrain_type->name = luaL_checkstring(L, 2);
    terrain_type->color = bswap_32(lua_tonumber(L, 3)) >> 8;
    terrain_type->color |= 0xff000000;
    terrain_type->movement_penalty = lua_tonumber(L, 4);
    terrain_type->is_water_body = lua_toboolean(L, 5);

    g_world->insert(terrain_type);
    lua_pushnumber(L, g_world->terrain_types.size() - 1);
    return 1;
}

int LuaAPI::get_terrain_type(lua_State* L) {
    const auto* terrain_type = find_or_throw<TerrainType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(terrain_type));
    lua_pushstring(L, terrain_type->ref_name.c_str());
    lua_pushstring(L, terrain_type->name.c_str());
    lua_pushnumber(L, bswap_32((terrain_type->color & 0x00ffffff) << 8));
    lua_pushnumber(L, terrain_type->movement_penalty);
    lua_pushboolean(L, terrain_type->is_water_body);
    return 6;
}

int LuaAPI::set_nation_mod_to_invention(lua_State* L) {
    Technology* technology = g_world->technologies.at(lua_tonumber(L, 1));
    technology->modifiers.push_back(g_world->nation_modifiers.at(lua_tonumber(L, 2)));
    return 0;
}

int LuaAPI::add_technology(lua_State* L) {
    if(g_world->needs_to_sync) {
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    }

    Technology* technology = new Technology();

    technology->ref_name = luaL_checkstring(L, 1);
    technology->name = luaL_checkstring(L, 2);
    technology->description = lua_tostring(L, 3);
    technology->cost = lua_tonumber(L, 4);
    technology->type = (TechnologyType)((int)lua_tonumber(L, 5));

    g_world->insert(technology);
    lua_pushnumber(L, g_world->technologies.size() - 1);

    for(auto& nation : g_world->nations) {
        nation->research.push_back(technology->cost);
    }
    return 1;
}

int LuaAPI::get_technology(lua_State* L) {
    const auto* technology = find_or_throw<Technology>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(technology));
    lua_pushstring(L, technology->name.c_str());
    lua_pushstring(L, technology->description.c_str());
    lua_pushnumber(L, technology->cost);
    lua_pushnumber(L, technology->type);
    return 5;
}

int LuaAPI::add_req_tech_to_tech(lua_State* L) {
    Technology* technology = g_world->technologies.at(lua_tonumber(L, 1));
    technology->req_technologies.push_back(g_world->technologies.at(lua_tonumber(L, 2)));
    return 0;
}

int LuaAPI::add_unit_trait(lua_State* L) {
    if(g_world->needs_to_sync) {
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    }

    UnitTrait* unit_trait = new UnitTrait();

    unit_trait->ref_name = luaL_checkstring(L, 1);
    unit_trait->supply_consumption_mod = lua_tonumber(L, 2);
    unit_trait->speed_mod = lua_tonumber(L, 3);
    unit_trait->max_health_mod = lua_tonumber(L, 4);
    unit_trait->defense_mod = lua_tonumber(L, 5);
    unit_trait->attack_mod = lua_tonumber(L, 6);

    g_world->insert(unit_trait);
    lua_pushnumber(L, g_world->unit_traits.size() - 1);
    return 1;
}

int LuaAPI::add_building_type(lua_State* L) {
    if(g_world->needs_to_sync) {
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    }

    BuildingType* building_type = new BuildingType();

    building_type->ref_name = luaL_checkstring(L, 1);
    building_type->name = luaL_checkstring(L, 2);
    building_type->is_plot_on_sea = lua_toboolean(L, 3);
    building_type->is_build_land_units = lua_toboolean(L, 4);
    building_type->is_build_naval_units = lua_toboolean(L, 5);
    building_type->defense_bonus = lua_tonumber(L, 6);
    building_type->is_factory = lua_toboolean(L, 7);

    g_world->insert(building_type);
    lua_pushnumber(L, g_world->building_types.size() - 1);
    return 1;
}

int LuaAPI::get_building_type(lua_State* L) {
    const auto* building_type = find_or_throw<BuildingType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(building_type));
    lua_pushstring(L, building_type->ref_name.c_str());
    lua_pushstring(L, building_type->name.c_str());
    lua_pushboolean(L, building_type->is_plot_on_sea);
    lua_pushboolean(L, building_type->is_build_land_units);
    lua_pushboolean(L, building_type->is_build_naval_units);
    lua_pushnumber(L, building_type->defense_bonus);
    lua_pushboolean(L, building_type->is_factory);
    return 8;
}

int LuaAPI::add_good(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    Good* good = new Good();

    good->ref_name = luaL_checkstring(L, 1);
    good->name = luaL_checkstring(L, 2);
    good->is_edible = lua_toboolean(L, 3);

    g_world->insert(good);
    lua_pushnumber(L, g_world->goods.size() - 1);
    return 1;
}

int LuaAPI::get_good(lua_State* L) {
    const auto* good = find_or_throw<Good>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(good));
    lua_pushstring(L, good->name.c_str());
    return 2;
}

int LuaAPI::add_input_to_industry_type(lua_State* L) {
    BuildingType* industry_type = g_world->building_types.at(lua_tonumber(L, 1));
    Good* good = g_world->goods.at(lua_tonumber(L, 2));
    industry_type->inputs.push_back(good);
    return 0;
}

int LuaAPI::add_output_to_industry_type(lua_State* L) {
    BuildingType* industry_type = g_world->building_types.at(lua_tonumber(L, 1));
    Good* good = g_world->goods.at(lua_tonumber(L, 2));
    industry_type->outputs.push_back(good);
    return 0;
}

int LuaAPI::add_req_good_to_industry_type(lua_State* L) {
    BuildingType* industry_type = g_world->building_types.at(lua_tonumber(L, 1));
    auto* good = g_world->goods.at(lua_tonumber(L, 2));
    industry_type->req_goods.push_back(std::make_pair(good, lua_tonumber(L, 3)));
    return 0;
}

int LuaAPI::add_req_technology_to_industry_type(lua_State* L) {
    BuildingType* industry_type = g_world->building_types.at(lua_tonumber(L, 1));
    auto* technology = g_world->technologies.at(lua_tonumber(L, 2));
    industry_type->req_technologies.push_back(technology);
    return 0;
}

int LuaAPI::add_nation(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    Nation* nation = new Nation();

    nation->ref_name = luaL_checkstring(L, 1);
    nation->name = luaL_checkstring(L, 2);
    nation->ideology = g_world->ideologies.at(0);

    // Check for duplicates
    for(size_t i = 0; i < g_world->nations.size(); i++) {
        if(nation->ref_name == g_world->nations[i]->ref_name) {
            throw LuaAPI::Exception("Duplicate ref_name " + nation->ref_name);
        }
    }

    g_world->insert(nation);
    lua_pushnumber(L, g_world->get_id(nation));
    return 1;
}

int LuaAPI::get_nation(lua_State* L) {
    const auto* nation = find_or_throw<Nation>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(nation));
    lua_pushstring(L, nation->name.c_str());
    return 2;
}

int LuaAPI::get_nation_by_id(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));

    lua_pushstring(L, nation->name.c_str());
    lua_pushstring(L, nation->ref_name.c_str());
    return 2;
}

int LuaAPI::get_all_nations(lua_State* L) {
    lua_newtable(L);

    size_t i = 0;
    for(const auto& nation : g_world->nations) {
        lua_pushnumber(L, g_world->get_id(nation));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

// TODO: Make wars be dynamically named with cassus bellis??
/*int LuaAPI::set_war_of_nation(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));
    const auto* other_nation = g_world->nations.at(lua_tonumber(L, 2));

    const NationRelation& relation = nation->relations[g_world->get_id(other_nation)];
    return 0;
}*/

int LuaAPI::get_friends_of_nation(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& friend_nation : g_world->nations) {
        const NationRelation& relation = nation->relations[g_world->get_id(friend_nation)];
        if(relation.relation < 50.f) {
            continue;
        }

        lua_pushnumber(L, g_world->get_id(friend_nation));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_enemies_of_nation(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& other_nation : g_world->nations) {
        const NationRelation& relation = nation->relations[g_world->get_id(other_nation)];
        if(relation.relation > -50.f) {
            continue;
        }

        lua_pushnumber(L, g_world->get_id(other_nation));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_allies_of_nation(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& other_nation : g_world->nations) {
        const NationRelation& relation = nation->relations[g_world->get_id(other_nation)];
        if(!relation.has_alliance) {
            continue;
        }

        lua_pushnumber(L, g_world->get_id(other_nation));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_warenemies_of_nation(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& other_nation : g_world->nations) {
        const NationRelation& relation = nation->relations[g_world->get_id(other_nation)];
        if(!relation.has_war) {
            continue;
        }

        lua_pushnumber(L, g_world->get_id(other_nation));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_embargoed_of_nation(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& other_nation : g_world->nations) {
        const NationRelation& relation = nation->relations[g_world->get_id(other_nation)];
        if(!relation.has_embargo) {
            continue;
        }

        lua_pushnumber(L, g_world->get_id(other_nation));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_provinces_owned_by_nation(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& province : nation->owned_provinces) {
        lua_pushnumber(L, g_world->get_id(province));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_provinces_with_nucleus_by_nation(lua_State* L) {
    const auto* nation = g_world->nations.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& province : g_world->provinces) {
        bool is_nuclei = false;
        for(const auto& nuclei : province->nuclei) {
            if(nuclei == nation) {
                is_nuclei = true;
                break;
            }
        }
        if(!is_nuclei) {
            continue;
        }

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
    Nation* nation = g_world->nations.at(lua_tonumber(L, 1));
    nation->capital = g_world->provinces.at(lua_tonumber(L, 2));
    return 0;
}

int LuaAPI::add_accepted_culture(lua_State* L) {
    Nation* nation = g_world->nations.at(lua_tonumber(L, 1));
    nation->accepted_cultures.insert(g_world->cultures.at(lua_tonumber(L, 2)));
    return 0;
}

int LuaAPI::add_accepted_religion(lua_State* L) {
    Nation* nation = g_world->nations.at(lua_tonumber(L, 1));
    nation->accepted_religions.insert(g_world->religions.at(lua_tonumber(L, 2)));
    return 0;
}

int LuaAPI::add_nation_client_hint(lua_State* L) {
    Nation* nation = g_world->nations.at(lua_tonumber(L, 1));

    NationClientHint hint;
    hint.ideology = g_world->ideologies.at(lua_tonumber(L, 2));
    hint.alt_name = luaL_checkstring(L, 3);
    hint.color = bswap_32(lua_tonumber(L, 4)) >> 8;
    hint.color |= 0xff000000;

    nation->client_hints.push_back(hint);
    return 0;
}

int LuaAPI::get_nation_policies(lua_State* L) {
    Nation* nation = g_world->nations.at(lua_tonumber(L, 1));

    // We are going to push everything in the policies structure
    // this is horrible - reflection may help in this case
    const Policies& policy = nation->current_policy;
    lua_pushnumber(L, policy.treatment);
    lua_pushnumber(L, policy.migration);
    lua_pushnumber(L, policy.immigration);
    lua_pushnumber(L, policy.censorship);
    lua_pushnumber(L, policy.build_infrastructure);
    lua_pushnumber(L, policy.build_factories);
    lua_pushboolean(L, policy.national_id);
    lua_pushboolean(L, policy.men_suffrage);
    lua_pushboolean(L, policy.men_labour);
    lua_pushboolean(L, policy.women_suffrage);
    lua_pushboolean(L, policy.women_labour);
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
    lua_pushnumber(L, policy.import_tax);
    lua_pushnumber(L, policy.export_tax);
    lua_pushnumber(L, policy.domestic_import_tax);
    lua_pushnumber(L, policy.domestic_export_tax);
    lua_pushnumber(L, policy.poor_flat_tax);
    lua_pushnumber(L, policy.med_flat_tax);
    lua_pushnumber(L, policy.rich_flat_tax);
    lua_pushnumber(L, policy.industry_tax);
    lua_pushnumber(L, policy.military_spending);
    lua_pushboolean(L, policy.free_supplies);
    lua_pushnumber(L, policy.min_wage);
    lua_pushnumber(L, policy.min_sv_for_parliament);
    return 34;
}

int LuaAPI::set_nation_policies(lua_State* L) {
    Nation* nation = g_world->nations.at(lua_tonumber(L, 1));

    // We are going to push everything in the policies structure
    // this is horrible - reflection may help in this case
    Policies& policy = nation->current_policy;
    policy.treatment = static_cast<TreatmentPolicy>(lua_tonumber(L, 1));
    policy.migration = static_cast<AllowancePolicy>(lua_tonumber(L, 2));
    policy.immigration = static_cast<AllowancePolicy>(lua_tonumber(L, 3));
    policy.censorship = static_cast<CensorshipPolicy>(lua_tonumber(L, 4));
    policy.build_infrastructure = static_cast<AutoBuildPolicy>(lua_tonumber(L, 5));
    policy.build_factories = static_cast<AutoBuildPolicy>(lua_tonumber(L, 6));
    policy.national_id = lua_toboolean(L, 7);
    policy.men_suffrage = lua_toboolean(L, 8);
    policy.men_labour = lua_toboolean(L, 9);
    policy.women_suffrage = lua_toboolean(L, 10);
    policy.women_labour = lua_toboolean(L, 11);
    policy.private_property = lua_toboolean(L, 12);
    policy.companies_allowed = lua_toboolean(L, 13);
    policy.public_education = lua_toboolean(L, 14);
    policy.secular_education = lua_toboolean(L, 15);
    policy.public_healthcare = lua_toboolean(L, 16);
    policy.social_security = lua_toboolean(L, 17);
    policy.slavery = lua_toboolean(L, 18);
    policy.legislative_parliament = lua_toboolean(L, 19);
    policy.executive_parliament = lua_toboolean(L, 20);
    policy.constitutional = lua_toboolean(L, 21);
    policy.foreign_trade = lua_toboolean(L, 22);
    policy.import_tax = lua_tonumber(L, 23);
    policy.export_tax = lua_tonumber(L, 24);
    policy.domestic_import_tax = lua_tonumber(L, 25);
    policy.domestic_export_tax = lua_tonumber(L, 26);
    policy.poor_flat_tax = lua_tonumber(L, 27);
    policy.med_flat_tax = lua_tonumber(L, 28);
    policy.rich_flat_tax = lua_tonumber(L, 29);
    policy.industry_tax = lua_tonumber(L, 30);
    policy.military_spending = lua_tonumber(L, 31);
    policy.free_supplies = lua_toboolean(L, 32);
    policy.min_wage = lua_tonumber(L, 33);
    policy.min_sv_for_parliament = lua_tonumber(L, 34);
    return 0;
}

int LuaAPI::set_nation_ideology(lua_State* L) {
    Nation* nation = g_world->nations.at(lua_tonumber(L, 1));
    nation->ideology = g_world->ideologies.at(lua_tonumber(L, 2));
    return 0;
}

int LuaAPI::get_nation_relation(lua_State* L) {
    Nation& nation = *g_world->nations.at(lua_tonumber(L, 1));
    Nation& other_nation = *g_world->nations.at(lua_tonumber(L, 2));

    NationRelation& relation = nation.relations[g_world->get_id(&other_nation)];
    lua_pushnumber(L, relation.relation);
    lua_pushnumber(L, relation.interest);
    lua_pushboolean(L, relation.has_embargo);
    lua_pushboolean(L, relation.has_war);
    lua_pushboolean(L, relation.has_alliance);
    lua_pushboolean(L, relation.has_defensive_pact);
    lua_pushboolean(L, relation.has_truce);
    lua_pushboolean(L, relation.has_embassy);
    lua_pushboolean(L, relation.has_military_access);
    lua_pushboolean(L, relation.has_market_access);
    lua_pushboolean(L, relation.free_supplies);
    return 11;
}

int LuaAPI::set_nation_relation(lua_State* L) {
    Nation& nation = *g_world->nations.at(lua_tonumber(L, 1));
    Nation& other_nation = *g_world->nations.at(lua_tonumber(L, 2));

    NationRelation& relation = nation.relations[g_world->get_id(&other_nation)];
    relation.relation = lua_tonumber(L, 3);
    relation.interest = lua_tonumber(L, 4);
    relation.has_embargo = lua_toboolean(L, 5);
    relation.has_war = lua_toboolean(L, 6);
    relation.has_alliance = lua_toboolean(L, 7);
    relation.has_defensive_pact = lua_toboolean(L, 8);
    relation.has_truce = lua_toboolean(L, 9);
    relation.has_embassy = lua_toboolean(L, 10);
    relation.has_military_access = lua_toboolean(L, 11);
    relation.has_market_access = lua_toboolean(L, 12);
    relation.free_supplies = lua_toboolean(L, 13);
    return 0;
}

int LuaAPI::nation_declare_unjustified_war(lua_State* L) {
    Nation& nation = *g_world->nations.at(lua_tonumber(L, 1));
    Nation& other_nation = *g_world->nations.at(lua_tonumber(L, 2));

    if(!nation.relations[g_world->get_id(&other_nation)].has_war) {
        nation.declare_war(other_nation);
    }
    return 0;
}

int LuaAPI::add_nation_mod(lua_State* L) {
    if(g_world->needs_to_sync) {
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    }

    NationModifier* mod = new NationModifier();

    mod->ref_name = luaL_checkstring(L, 1);
    mod->name = luaL_checkstring(L, 2);

    mod->industry_output_mod = lua_tonumber(L, 3);
    mod->industry_input_mod = lua_tonumber(L, 4);
    mod->workers_needed_mod = lua_tonumber(L, 5);
    mod->salary_paid_mod = lua_tonumber(L, 6);
    mod->delivery_cost_mod = lua_tonumber(L, 7);
    mod->literacy_learn_mod = lua_tonumber(L, 8);
    mod->reproduction_mod = lua_tonumber(L, 9);
    mod->death_mod = lua_tonumber(L, 10);
    mod->militancy_mod = lua_tonumber(L, 11);
    mod->con_mod = lua_tonumber(L, 12);
    mod->life_needs_met_mod = lua_tonumber(L, 13);
    mod->everyday_needs_met_mod = lua_tonumber(L, 14);
    mod->luxury_needs_met_mod = lua_tonumber(L, 15);

    g_world->insert(mod);
    lua_pushnumber(L, g_world->get_id(mod));
    return 1;
}

int LuaAPI::get_nation_mod(lua_State* L) {
    const auto* mod = find_or_throw<NationModifier>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(mod));
    lua_pushstring(L, mod->name.c_str());

    lua_pushnumber(L, mod->industry_output_mod);
    lua_pushnumber(L, mod->industry_input_mod);
    lua_pushnumber(L, mod->workers_needed_mod);
    lua_pushnumber(L, mod->salary_paid_mod);
    lua_pushnumber(L, mod->delivery_cost_mod);
    lua_pushnumber(L, mod->literacy_learn_mod);
    lua_pushnumber(L, mod->reproduction_mod);
    lua_pushnumber(L, mod->death_mod);
    lua_pushnumber(L, mod->militancy_mod);
    lua_pushnumber(L, mod->con_mod);
    lua_pushnumber(L, mod->life_needs_met_mod);
    lua_pushnumber(L, mod->everyday_needs_met_mod);
    lua_pushnumber(L, mod->luxury_needs_met_mod);
    return 1;
}

int LuaAPI::add_province(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    Province* province = new Province();
    province->terrain_type = g_world->terrain_types[0];

    province->ref_name = luaL_checkstring(L, 1);
    province->color = (bswap_32(lua_tonumber(L, 2)) >> 8) | 0xff000000;
    
    province->name = luaL_checkstring(L, 3);
    province->budget = 500.f;

    // Set bounding box of province to the whole world (will later be resized at the bitmap-processing step)
    province->max_x = std::numeric_limits<uint32_t>::min();
    province->max_y = std::numeric_limits<uint32_t>::min();
    province->min_x = std::numeric_limits<uint32_t>::max();
    province->min_y = std::numeric_limits<uint32_t>::max();

    // Check for duplicates
    for(size_t i = 0; i < g_world->provinces.size(); i++) {
        if(province->color == g_world->provinces[i]->color) {
            throw LuaAPI::Exception(province->ref_name + " province has same color as " + g_world->provinces[i]->ref_name);
        } else if(province->ref_name == g_world->provinces[i]->ref_name) {
            throw LuaAPI::Exception("Duplicate ref_name " + province->ref_name);
        }
    }

    g_world->insert(province);
    lua_pushnumber(L, g_world->get_id(province));
    return 1;
}

int LuaAPI::get_province(lua_State* L) {
    const auto* province = find_or_throw<Province>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(province));
    lua_pushstring(L, province->name.c_str());
    lua_pushnumber(L, bswap_32((province->color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::get_province_by_id(lua_State* L) {
    const Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_pushstring(L, province->ref_name.c_str());
    lua_pushstring(L, province->name.c_str());
    lua_pushnumber(L, bswap_32((province->color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::add_province_industry(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    Province*& province = g_world->provinces.at(lua_tonumber(L, 1));

    Building* building = new Building();
    building->province = province;
    building->type = g_world->building_types.at(lua_tonumber(L, 2));
    building->owner = g_world->nations.at(lua_tonumber(L, 3));
    building->budget = 100.f;
    if(building->type->is_factory == true) {
        building->create_factory();
    }
    g_world->insert(building);
    return 0;
}

int LuaAPI::set_province_terrain(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    province->terrain_type = g_world->terrain_types.at(lua_tonumber(L, 2));
    return 0;
}

int LuaAPI::give_province_to(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    g_world->nations.at(lua_tonumber(L, 2))->give_province(*province);
    return 0;
}

int LuaAPI::give_hard_province_to(lua_State* L) {
    auto* province = g_world->provinces.at(lua_tonumber(L, 1));
    auto* nation = g_world->nations.at(lua_tonumber(L, 2));

    for(auto& unit : g_world->units) {
        if(unit->province == province && unit->owner == province->controller) {
            unit->owner = nation;
        }
    }

    for(auto& building : g_world->buildings) {
        if(building->get_province() == province && building->get_owner() == province->controller) {
            building->owner = nation;
        }
    }

    nation->give_province(*province);
    return 0;
}

// Obtains the owner of a province (ref_name)
int LuaAPI::get_province_owner(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_pushstring(L, province->controller->ref_name.c_str());
    return 1;
}

// Get the country who owms a larger chunk of the province - this is not the same as owner
int LuaAPI::get_province_controller(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    Nation* nation = province->controller;
    if(nation == nullptr) {
        lua_pushnumber(L, -1);
    } else {
        lua_pushnumber(L, g_world->get_id(nation));
    }
    return 1;
}

// Obtains the neighbours of a province (by ID)
int LuaAPI::get_province_neighbours(lua_State* L) {
    const Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& neighbour : province->neighbours) {
        lua_pushnumber(L, g_world->get_id(neighbour));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_province_nuclei(lua_State* L) {
    const Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_newtable(L);

    size_t i = 0;
    for(const auto& nucleus : province->nuclei) {
        lua_pushnumber(L, g_world->get_id(nucleus));
        lua_rawseti(L, -2, i + 1);
        ++i;
    }
    return 1;
}

int LuaAPI::get_province_pops_size(lua_State* L) {
    const Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    lua_pushnumber(L, province->pops.size());
    return 1;
}

int LuaAPI::get_province_pop(lua_State* L) {
    const Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    const Pop& pop = province->pops.at(lua_tonumber(L, 2));
    lua_pushnumber(L, pop.size);
    lua_pushnumber(L, pop.budget);
    lua_pushnumber(L, pop.literacy);
    lua_pushnumber(L, pop.life_needs_met);
    lua_pushnumber(L, pop.everyday_needs_met);
    lua_pushnumber(L, pop.luxury_needs_met);
    lua_pushnumber(L, g_world->get_id(pop.type));
    lua_pushnumber(L, g_world->get_id(pop.culture));
    lua_pushnumber(L, g_world->get_id(pop.religion));
    lua_pushnumber(L, g_world->get_id(pop.get_ideology()));
    lua_pushnumber(L, pop.militancy);
    lua_pushnumber(L, pop.con);
    return 10;
}

int LuaAPI::set_province_pop(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    Pop& pop = province->pops.at(lua_tonumber(L, 2));
    pop.size = lua_tonumber(L, 3);
    pop.budget = lua_tonumber(L, 4);
    pop.literacy = lua_tonumber(L, 5);
    pop.life_needs_met = lua_tonumber(L, 6);
    pop.everyday_needs_met = lua_tonumber(L, 7);
    pop.luxury_needs_met = lua_tonumber(L, 8);
    pop.type = g_world->pop_types.at(lua_tonumber(L, 9));
    pop.culture = g_world->cultures.at(lua_tonumber(L, 10));
    pop.religion = g_world->religions.at(lua_tonumber(L, 11));
    pop.militancy = lua_tonumber(L, 12);
    pop.con = lua_tonumber(L, 13);
    return 0;
}

int LuaAPI::get_province_pop_ideology_approval(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    Pop& pop = province->pops.at(lua_tonumber(L, 2));
    lua_pushnumber(L, pop.ideology_approval.at(lua_tonumber(L, 3)));
    return 1;
}

int LuaAPI::set_province_pop_ideology_approval(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    Pop& pop = province->pops.at(lua_tonumber(L, 2));
    pop.ideology_approval.at(lua_tonumber(L, 3)) = lua_tonumber(L, 4);
    return 0;
}

int LuaAPI::add_province_pop(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));

    Pop pop;
    pop.type = g_world->pop_types.at(lua_tonumber(L, 2));
    pop.culture = g_world->cultures.at(lua_tonumber(L, 3));
    pop.religion = g_world->religions.at(lua_tonumber(L, 4));
    pop.size = lua_tonumber(L, 5);
    pop.literacy = lua_tonumber(L, 6);
    pop.budget = 10.f;

    // TODO: Make ideology NOT be random
    pop.ideology_approval.resize(g_world->ideologies.size(), 0.f);

    if(!pop.size) {
        throw LuaAPI::Exception("Can't create pops with 0 size");
    }
    province->pops.push_back(pop);
    return 0;
}

int LuaAPI::rename_province(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    province->name = luaL_checkstring(L, 2);
    return 0;
}

int LuaAPI::add_province_nucleus(lua_State* L) {
    g_world->provinces.at(lua_tonumber(L, 1))->nuclei.insert(g_world->nations.at(lua_tonumber(L, 2)));
    return 0;
}

int LuaAPI::add_province_owner(lua_State* L) {
    Province* province = g_world->provinces.at(lua_tonumber(L, 1));
    Nation* nation = g_world->nations.at(lua_tonumber(L, 2));
    nation->give_province(*province);
    return 0;
}

int LuaAPI::add_event(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    Event* event = new Event();

    event->ref_name = luaL_checkstring(L, 1);
    event->conditions_function = luaL_checkstring(L, 2);
    event->do_event_function = luaL_checkstring(L, 3);
    event->title = luaL_checkstring(L, 4);
    event->text = luaL_checkstring(L, 5);
    event->checked = lua_toboolean(L, 6);

    // Add onto vector
    g_world->insert(event);
    lua_pushnumber(L, g_world->events.size() - 1);
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

    lua_pushnumber(L, g_world->get_id(event));
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
    for(size_t i = 0; i < lua_tonumber(L, 2); i++) {
        event->receivers.push_back(g_world->nations.at(lua_tonumber(L, 3 + i)));
    }
    return 0;
}

int LuaAPI::add_descision(lua_State* L) {
    Event* event = g_world->events.at(lua_tonumber(L, 1));

    Descision descision = Descision();

    descision.ref_name = luaL_checkstring(L, 2);
    descision.name = luaL_checkstring(L, 3);
    descision.do_descision_function = luaL_checkstring(L, 4);
    descision.effects = luaL_checkstring(L, 5);

    // Add onto vector
    event->descisions.push_back(descision);
    return 0;
}

int LuaAPI::add_pop_type(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    PopType* pop = new PopType();

    pop->ref_name = luaL_checkstring(L, 1);
    pop->name = luaL_checkstring(L, 2);
    pop->social_value = lua_tonumber(L, 3);
    bool is_burgeoise = lua_toboolean(L, 4);
    bool is_slave = lua_toboolean(L, 5);
    bool is_farmer = lua_toboolean(L, 6);
    bool is_laborer = lua_toboolean(L, 7);
    if(is_burgeoise) {
        pop->group = PopGroup::BURGEOISE;
    } else if(is_slave) {
        pop->group = PopGroup::Slave;
    } else if(is_farmer) {
        pop->group = PopGroup::FARMER;
    } else if(is_laborer) {
        pop->group = PopGroup::LABORER;
    } else {
        pop->group = PopGroup::Other;
    }

    // Add onto vector
    g_world->insert(pop);
    lua_pushnumber(L, g_world->pop_types.size() - 1);
    return 1;
}

int LuaAPI::get_pop_type(lua_State* L) {
    const auto* pop_type = find_or_throw<PopType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(pop_type));
    lua_pushstring(L, pop_type->name.c_str());
    lua_pushnumber(L, pop_type->social_value);
    lua_pushboolean(L, pop_type->group == PopGroup::BURGEOISE);
    lua_pushboolean(L, pop_type->group == PopGroup::Slave);
    lua_pushboolean(L, pop_type->group == PopGroup::FARMER);
    lua_pushboolean(L, pop_type->group == PopGroup::LABORER);
    return 7;
}

int LuaAPI::get_pop_type_by_id(lua_State* L) {
    const PopType* pop_type = g_world->pop_types.at(lua_tonumber(L, 1));

    lua_pushstring(L, pop_type->ref_name.c_str());
    lua_pushstring(L, pop_type->name.c_str());
    lua_pushnumber(L, pop_type->social_value);
    lua_pushboolean(L, pop_type->group == PopGroup::BURGEOISE);
    lua_pushboolean(L, pop_type->group == PopGroup::Slave);
    lua_pushboolean(L, pop_type->group == PopGroup::FARMER);
    lua_pushboolean(L, pop_type->group == PopGroup::LABORER);
    return 7;
}

int LuaAPI::add_culture(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");

    Culture* culture = new Culture();

    culture->ref_name = luaL_checkstring(L, 1);
    culture->name = luaL_checkstring(L, 2);
    culture->color = (bswap_32(lua_tonumber(L, 3)) >> 8) | 0xff000000;
    culture->adjective = luaL_checkstring(L, 4);
    culture->noun = luaL_checkstring(L, 5);
    culture->combo_form = luaL_checkstring(L, 6);

    g_world->insert(culture);
    lua_pushnumber(L, g_world->cultures.size() - 1);
    return 1;
}

int LuaAPI::get_culture(lua_State* L) {
    const auto* culture = find_or_throw<Culture>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(culture));
    lua_pushstring(L, culture->name.c_str());
    lua_pushnumber(L, bswap_32((culture->color & 0x00ffffff) << 8));
    lua_pushstring(L, culture->adjective.c_str());
    lua_pushstring(L, culture->noun.c_str());
    lua_pushstring(L, culture->combo_form.c_str());
    return 6;
}

int LuaAPI::get_culture_by_id(lua_State* L) {
    const auto* culture = g_world->cultures.at(lua_tonumber(L, 1));

    lua_pushstring(L, culture->ref_name.c_str());
    lua_pushstring(L, culture->name.c_str());
    lua_pushnumber(L, bswap_32((culture->color & 0x00ffffff) << 8));
    lua_pushstring(L, culture->adjective.c_str());
    lua_pushstring(L, culture->noun.c_str());
    lua_pushstring(L, culture->combo_form.c_str());
    return 6;
}

int LuaAPI::add_religion(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    Religion* religion = new Religion();
    religion->ref_name = luaL_checkstring(L, 1);
    religion->name = luaL_checkstring(L, 2);
    religion->color = (bswap_32(lua_tonumber(L, 3)) >> 8) | 0xff000000;

    g_world->insert(religion);
    lua_pushnumber(L, g_world->religions.size() - 1);
    return 1;
}

int LuaAPI::get_religion(lua_State* L) {
    const auto* religion = find_or_throw<Religion>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(religion));
    lua_pushstring(L, religion->name.c_str());
    lua_pushnumber(L, bswap_32((religion->color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::get_religion_by_id(lua_State* L) {
    const auto* religion = g_world->religions.at(lua_tonumber(L, 1));

    lua_pushstring(L, religion->ref_name.c_str());
    lua_pushstring(L, religion->name.c_str());
    lua_pushnumber(L, bswap_32((religion->color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::add_unit_type(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    UnitType* unit_type = new UnitType();

    unit_type->ref_name = luaL_checkstring(L, 1);
    unit_type->name = luaL_checkstring(L, 2);
    unit_type->attack = lua_tonumber(L, 3);
    unit_type->defense = lua_tonumber(L, 4);
    unit_type->max_health = lua_tonumber(L, 5);

    unit_type->max_defensive_ticks = lua_tonumber(L, 6);
    unit_type->position_defense = lua_tonumber(L, 7);

    unit_type->is_ground = lua_toboolean(L, 8);
    unit_type->is_naval = lua_toboolean(L, 9);
	
	unit_type->speed = lua_tonumber(L, 10);

    g_world->insert(unit_type);
    lua_pushnumber(L, g_world->unit_types.size() - 1);
    return 1;
}

int LuaAPI::get_unit_type(lua_State* L) {
    const auto* unit_type = find_or_throw<UnitType>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(unit_type));
    lua_pushstring(L, unit_type->name.c_str());
    lua_pushnumber(L, unit_type->attack);
    lua_pushnumber(L, unit_type->defense);
    lua_pushnumber(L, unit_type->max_health);
    lua_pushnumber(L, unit_type->max_defensive_ticks);
    lua_pushnumber(L, unit_type->position_defense);
    lua_pushboolean(L, unit_type->is_ground);
    lua_pushboolean(L, unit_type->is_naval);
	lua_pushnumber(L, unit_type->speed);
    return 10;
}

int LuaAPI::add_req_good_unit_type(lua_State* L) {
    UnitType* unit_type = g_world->unit_types.at(lua_tonumber(L, 1));

    Good* good = g_world->goods.at(lua_tonumber(L, 2));
    size_t amount = lua_tonumber(L, 3);
    unit_type->req_goods.push_back(std::make_pair(good, amount));
    return 0;
}

int LuaAPI::add_ideology(lua_State* L) {
    if(g_world->needs_to_sync)
        throw LuaAPI::Exception("MP-Sync in this function is not supported");
    
    Ideology* ideology = new Ideology();

    ideology->ref_name = luaL_checkstring(L, 1);
    ideology->name = luaL_checkstring(L, 2);
    ideology->color = (bswap_32(lua_tonumber(L, 3)) >> 8) | 0xff000000;

    g_world->insert(ideology);
    lua_pushnumber(L, g_world->ideologies.size() - 1);
    return 1;
}

int LuaAPI::get_ideology(lua_State* L) {
    const auto* ideology = find_or_throw<Ideology>(luaL_checkstring(L, 1));

    lua_pushnumber(L, g_world->get_id(ideology));
    lua_pushstring(L, ideology->name.c_str());
    lua_pushnumber(L, bswap_32((ideology->color & 0x00ffffff) << 8));
    return 3;
}

int LuaAPI::get_ideology_by_id(lua_State* L) {
    const auto* ideology = g_world->ideologies.at(lua_tonumber(L, 1));
    
    lua_pushstring(L, ideology->ref_name.c_str());
    lua_pushstring(L, ideology->name.c_str());
    lua_pushnumber(L, bswap_32((ideology->color & 0x00ffffff) << 8));
    return 3;
}

// Checks all events and their condition functions
void LuaAPI::check_events(lua_State* L) {
    // Because of the logic of this loop, only 1 event can happen in the world per tick
    // This is on purpouse ;)
    for(size_t i = 0; i < g_world->events.size(); i++) {
        Event* event = g_world->events[i];
        if(event->checked) {
            continue;
        }
        
        bool is_multi = true;
        bool has_fired = false;
        for(auto& nation : event->receivers) {
            if(!nation->exists()) {
                continue;
            }

            lua_getglobal(L, event->conditions_function.c_str());
            lua_pushstring(L, nation->ref_name.c_str());
            lua_pcall(L, 1, 1, 0);
            bool r = lua_toboolean(L, -1);
            lua_pop(L, 1);

            // Conditions met
            if(r) {
                has_fired = true;

                Event orig_event = Event(*event);

                // Call the "do event" function
                lua_getglobal(L, event->do_event_function.c_str());
                lua_pushstring(L, nation->ref_name.c_str());
                lua_pcall(L, 1, 1, 0);
                is_multi = lua_tointeger(L, -1);
                lua_pop(L, 1);
				
				// The changes done to the event "locally" are then created into a new local event
                auto* local_event = new Event(*event);
				local_event->cached_id = (Event::Id)-1;
                local_event->ref_name += "_";
                for(unsigned int j = 0; j < 20; j++) {
                    local_event->ref_name += 'a' + (rand() % 26);
                }
				// Do not relaunch a local event
                local_event->checked = true;
				
				if(local_event->descisions.empty()) {
					print_error("Event %s has no descisions (ref_name = %s)", local_event->ref_name.c_str(), nation->ref_name.c_str());
					*event = orig_event;
					continue;
				}
				
                g_world->insert(local_event);
				nation->inbox.push_back(local_event);

                print_info("Event triggered! %s (with %zu descisions)", local_event->ref_name.c_str(), (size_t)local_event->descisions.size());
				
				// Original event then gets restored
                *event = orig_event;
            }
        }
        
        // Event is marked as checked if it's not of multiple occurences
        if(has_fired && !is_multi) {
            g_world->events[i]->checked = true;
        }
    }

    // Do descisions taken effects in the queue, then clear it awaiting
    // other taken descisions :)
    for(auto& dec : g_world->taken_descisions) {
        lua_getglobal(L, dec.first->do_descision_function.c_str());
        lua_pushstring(L, dec.second->ref_name.c_str());

        try {
            lua_pcall(L, 1, 1, 0);
        } catch(const std::exception& e) {
            throw LuaAPI::Exception(dec.first->do_descision_function + ": " + e.what());
        }

        // TODO: Delete local event upon taking a descision
    }
    g_world->taken_descisions.clear();
}