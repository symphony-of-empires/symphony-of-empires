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
//      world.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>

#include "eng3d/binary_image.hpp"
#include "eng3d/log.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/state.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/string.hpp"

#include "server/emigration.hpp"
#include "province.hpp"
#include "world.hpp"
#include "diplomacy.hpp"
#include "world.hpp"
#include "server/lua_api.hpp"
#include "server/server_network.hpp"
#include "action.hpp"
#include "server/economy.hpp"

#undef min
#undef max

World g_world;

namespace AI {
    void init(World& world);
    void do_tick(World& world);
}

template<typename T>
static const T& find_or_throw(const std::string_view ref_name) {
    const auto& list = World::get_instance().get_list((T*)nullptr);
    const auto result = std::find_if(list.begin(), list.end(), [ref_name](const auto& o) {
        return !strcmp(o.ref_name.data(), ref_name.data());
    });

    if(result == list.end())
        CXX_THROW(Eng3D::LuaException, translate_format("Object<%s> not found", typeid(T).name()).data());
    return *result;
}

// Creates a new world
void World::init_lua() {
    // Register our API functions
    lua_register(lua.state, "add_terrain_type", LuaAPI::add_terrain_type);
    lua_register(lua.state, "get_terrain_type", LuaAPI::get_terrain_type);
    lua_register(lua.state, "get_terrain_type_by_id", LuaAPI::get_terrain_type_by_id);

    lua_register(lua.state, "add_technology", LuaAPI::add_technology);
    lua_register(lua.state, "get_technology", LuaAPI::get_technology);
    lua_register(lua.state, "add_req_tech_to_tech", LuaAPI::add_req_tech_to_tech);

    lua_register(lua.state, "add_building_type", LuaAPI::add_building_type);
    lua_register(lua.state, "get_building_type", LuaAPI::get_building_type);

    lua_register(lua.state, "add_good", LuaAPI::add_good);
    lua_register(lua.state, "get_good", LuaAPI::get_good);

    lua_register(lua.state, "add_input_to_industry_type", LuaAPI::add_input_to_industry_type);
    lua_register(lua.state, "add_output_to_industry_type", LuaAPI::add_output_to_industry_type);
    lua_register(lua.state, "add_req_good_to_industry_type", LuaAPI::add_req_good_to_industry_type);
    lua_register(lua.state, "add_req_technology_to_industry_type", LuaAPI::add_req_technology_to_industry_type);

    lua_register(lua.state, "add_nation", LuaAPI::add_nation);
    lua_register(lua.state, "get_nation", LuaAPI::get_nation);
    lua_register(lua.state, "get_nation_by_id", LuaAPI::get_nation_by_id);
    lua_register(lua.state, "get_all_nations", LuaAPI::get_all_nations);
    lua_register(lua.state, "switch_nation_soul", LuaAPI::switch_nation_soul);
    lua_register(lua.state, "nation_declare_war_no_cb", LuaAPI::nation_declare_war_no_cb);

    lua_register(lua.state, "get_provinces_owned_by_nation", LuaAPI::get_provinces_owned_by_nation);
    lua_register(lua.state, "get_provinces_with_nucleus_by_nation", LuaAPI::get_provinces_with_nucleus_by_nation);
    lua_register(lua.state, "set_nation_primary_language", LuaAPI::set_nation_primary_language);
    lua_register(lua.state, "set_nation_capital", LuaAPI::set_nation_capital);
    lua_register(lua.state, "add_nation_accepted_language", LuaAPI::add_accepted_language);
    lua_register(lua.state, "add_nation_accepted_religion", LuaAPI::add_accepted_religion);
    lua_register(lua.state, "add_nation_client_hint", LuaAPI::add_nation_client_hint);
    lua_register(lua.state, "set_nation_ideology", [](lua_State* L) {
        auto& nation = g_world.nations.at(lua_tonumber(L, 1));
        const auto& ideology = g_world.ideologies.at(lua_tonumber(L, 2));
        nation.ideology_id = ideology.get_id();
        nation.subideology_id = SubideologyId(0);
        const std::string subideology_ref_name = luaL_checkstring(L, 3);
        for(const auto& subideology : ideology.subideologies) {
            if(subideology.ref_name.get_string() == subideology_ref_name) {
                nation.subideology_id = subideology.get_id();
                nation.current_policy.economic = subideology.economic;
                nation.current_policy.political = subideology.political;
                break;
            }
        }
        return 0;
    });
    lua_register(lua.state, "get_nation_relation", LuaAPI::get_nation_relation);
    lua_register(lua.state, "set_nation_relation", LuaAPI::set_nation_relation);
    lua_register(lua.state, "nation_declare_unjustified_war", LuaAPI::nation_declare_unjustified_war);
    lua_register(lua.state, "nation_make_puppet", [](lua_State* L) {
        const auto& nation = g_world.nations.at(lua_tonumber(L, 1));
        auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
        other_nation.make_puppet(nation);
        auto& relation = g_world.get_relation(nation, other_nation);
        relation.alliance = 0.45f; // Just below to not make a customs union
        relation.relation = 0.f;
        return 0;
    });
    lua_register(lua.state, "nation_make_customs_union", [](lua_State* L) {
        auto& nation = g_world.nations.at(lua_tonumber(L, 1));
        auto& other_nation = g_world.nations.at(lua_tonumber(L, 2));
        auto& relation = g_world.get_relation(nation, other_nation);
        relation.alliance = 1.f;
        relation.relation = 0.f;
        return 0;
    });
    lua_register(lua.state, "set_nation_flag", [](lua_State* L) {
        auto& nation = g_world.nations.at(lua_tonumber(L, 1));
        nation.flags[luaL_checkstring(L, 2)] = lua_tonumber(L, 3);
        return 0;
    });
    lua_register(lua.state, "get_nation_flag", [](lua_State* L) {
        auto& nation = g_world.nations.at(lua_tonumber(L, 1));
        lua_pushnumber(L, nation.flags[luaL_checkstring(L, 2)]);
        return 1;
    });

    lua_register(lua.state, "add_province", LuaAPI::add_province);
    lua_register(lua.state, "update_province", LuaAPI::update_province);
    lua_register(lua.state, "get_province", LuaAPI::get_province);
    lua_register(lua.state, "get_province_by_id", LuaAPI::get_province_by_id);
    lua_register(lua.state, "province_add_unit", LuaAPI::province_add_unit);
    lua_register(lua.state, "update_province_building", LuaAPI::update_province_building);
    lua_register(lua.state, "add_province_pop", LuaAPI::add_province_pop);
    lua_register(lua.state, "set_province_language", LuaAPI::set_province_language);
    lua_register(lua.state, "set_province_religion", LuaAPI::set_province_religion);
    lua_register(lua.state, "give_province_to", LuaAPI::give_province_to);
    lua_register(lua.state, "give_hard_province_to", LuaAPI::give_hard_province_to);
    lua_register(lua.state, "get_province_owner", LuaAPI::get_province_owner);
    lua_register(lua.state, "get_province_controller", LuaAPI::get_province_controller);
    lua_register(lua.state, "get_province_neighbours", LuaAPI::get_province_neighbours);
    lua_register(lua.state, "get_province_nuclei", LuaAPI::get_province_nuclei);
    lua_register(lua.state, "get_province_pops_size", [](lua_State* L) {
        const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
        lua_pushnumber(L, province.pops.size());
        return 1;
    });
    lua_register(lua.state, "get_province_pop", [](lua_State* L) {
        const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
        const auto& pop = province.pops.at(lua_tonumber(L, 2));
        lua_pushnumber(L, pop.size);
        lua_pushnumber(L, pop.budget);
        lua_pushnumber(L, pop.literacy);
        lua_pushnumber(L, pop.life_needs_met);
        lua_pushnumber(L, 1.f);
        lua_pushnumber(L, 1.f);
        lua_pushnumber(L, (size_t)pop.type_id);
        lua_pushnumber(L, IdeologyId(0));
        lua_pushnumber(L, pop.militancy);
        return 9;
    });
    lua_register(lua.state, "set_province_pop", [](lua_State* L) {
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
    });

    lua_register(lua.state, "get_province_buildings_size", [](lua_State* L) {
        const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
        lua_pushnumber(L, province.buildings.size());
        return 1;
    });
    lua_register(lua.state, "get_province_building", [](lua_State* L) {
        const auto& province = g_world.provinces.at(lua_tonumber(L, 1));
        const auto& building = province.buildings.at(lua_tonumber(L, 2));
        lua_pushnumber(L, building.level);
        lua_pushnumber(L, building.production_scale);
        lua_pushnumber(L, building.workers);
        return 3;
    });
    lua_register(lua.state, "set_province_building", [](lua_State* L) {
        auto& province = g_world.provinces.at(lua_tonumber(L, 1));
        auto& building = province.buildings.at(lua_tonumber(L, 2));
        building.level = lua_tonumber(L, 3);
        building.production_scale = lua_tonumber(L, 4);
        building.workers = lua_tonumber(L, 5);
        return 0;
    });

    lua_register(lua.state, "rename_province", LuaAPI::rename_province);
    lua_register(lua.state, "add_province_nucleus", LuaAPI::add_province_nucleus);
    lua_register(lua.state, "add_province_owner", LuaAPI::add_province_owner);

    lua_register(lua.state, "add_event", LuaAPI::add_event);
    lua_register(lua.state, "get_event", LuaAPI::get_event);
    lua_register(lua.state, "update_event", LuaAPI::update_event);
    lua_register(lua.state, "add_event_receivers", LuaAPI::add_event_receivers);

    lua_register(lua.state, "add_decision", LuaAPI::add_decision);

    lua_register(lua.state, "add_pop_type", LuaAPI::add_pop_type);
    lua_register(lua.state, "get_pop_type", LuaAPI::get_pop_type);
    lua_register(lua.state, "get_pop_type_by_id", LuaAPI::get_pop_type_by_id);

    lua_register(lua.state, "add_language", LuaAPI::add_language);
    lua_register(lua.state, "get_language", LuaAPI::get_language);
    lua_register(lua.state, "get_language_by_id", LuaAPI::get_language_by_id);

    lua_register(lua.state, "add_religion", LuaAPI::add_religion);
    lua_register(lua.state, "get_religion", LuaAPI::get_religion);
    lua_register(lua.state, "get_religion_by_id", LuaAPI::get_religion_by_id);

    lua_register(lua.state, "add_unit_type", LuaAPI::add_unit_type);
    lua_register(lua.state, "get_unit_type", LuaAPI::get_unit_type);
    lua_register(lua.state, "add_req_good_unit_type", LuaAPI::add_req_good_unit_type);

    lua_register(lua.state, "add_ideology", [](lua_State* L) {
        if(g_world.needs_to_sync)
            luaL_error(L, "MP-Sync in this function is not supported");

        Ideology ideology{};
        ideology.ref_name = luaL_checkstring(L, 1);
        ideology.name = luaL_checkstring(L, 2);
        ideology.color = (std::byteswap<std::uint32_t>(static_cast<int>(lua_tonumber(L, 3))) >> 8) | 0xff000000;
        g_world.insert(ideology);
        lua_pushnumber(L, g_world.ideologies.size() - 1);
        return 1;
    });
    lua_register(lua.state, "add_ideology_subideology", [](lua_State* L) {
        if(g_world.needs_to_sync)
            luaL_error(L, "MP-Sync in this function is not supported");

        auto& ideology = g_world.ideologies.at(lua_tonumber(L, 1));
        Ideology::Subideology subideology{};
        subideology.ref_name = luaL_checkstring(L, 2);
        subideology.name = luaL_checkstring(L, 3);
        subideology.economic.distributism = lua_tonumber(L, 4);
        subideology.economic.mercantilist = lua_tonumber(L, 5);
        subideology.economic.capitalism = lua_tonumber(L, 6);
        subideology.political.individualism = lua_tonumber(L, 7);
        subideology.political.state_power = lua_tonumber(L, 8);
        subideology.political.equalitarianism = lua_tonumber(L, 9);
        subideology.political.secular = lua_tonumber(L, 10);
        subideology.political.pluralism = lua_tonumber(L, 11);

        subideology.cached_id = SubideologyId(ideology.subideologies.size());
        ideology.subideologies.push_back(subideology);
        return 0;
    });
    lua_register(lua.state, "get_ideology", [](lua_State* L) {
        const auto& ideology = find_or_throw<Ideology>(luaL_checkstring(L, 1));
        lua_pushnumber(L, (size_t)g_world.get_id(ideology));
        lua_pushstring(L, ideology.name.data());
        lua_pushnumber(L, std::byteswap<std::uint32_t>((ideology.color & 0x00ffffff) << 8));
        return 3;
    });
    lua_register(lua.state, "get_ideology_by_id", [](lua_State* L) {
        const auto& ideology = g_world.ideologies.at(lua_tonumber(L, 1));
        lua_pushstring(L, ideology.ref_name.data());
        lua_pushstring(L, ideology.name.data());
        lua_pushnumber(L, std::byteswap<std::uint32_t>((ideology.color & 0x00ffffff) << 8));
        return 3;
    });

    lua_register(lua.state, "get_day", [](lua_State* L) {
        lua_pushnumber(L, g_world.get_day());
        return 1;
    });
    lua_register(lua.state, "get_month", [](lua_State* L) {
        lua_pushnumber(L, g_world.get_month());
        return 1;
    });
    lua_register(lua.state, "get_year", [](lua_State* L) {
        lua_pushnumber(L, g_world.get_year());
        return 1;
    });
    lua_register(lua.state, "set_date", [](lua_State* L) {
        const int year = lua_tonumber(L, 1) * 12 * 30;
        const int month = lua_tonumber(L, 2) * 30;
        const int day = lua_tonumber(L, 3);
        g_world.time = year + month + day;
        return 1;
    });

    /*LuaAPI::register_new_table(lua.state, "Ideology", {}, {
        { "test", [](lua_State* L) {
            Eng3D::Log::debug("lua_test", "hello world");
            return 0;
        }}
    });*/

    // Constants for ease of readability
    lua_pushboolean(lua.state, true);
    lua_setglobal(lua.state, "EVENT_CONDITIONS_MET");
    lua_pushboolean(lua.state, false);
    lua_setglobal(lua.state, "EVENT_CONDITIONS_UNMET");

    lua_pushboolean(lua.state, true);
    lua_setglobal(lua.state, "EVENT_DO_MANY_TIMES");
    lua_pushboolean(lua.state, false);
    lua_setglobal(lua.state, "EVENT_DO_ONE_TIME");

    // Technology types
    lua_pushnumber(lua.state, TechnologyType::STRATEGIC);
    lua_setglobal(lua.state, "TECH_STRATEGIC");
    lua_pushnumber(lua.state, TechnologyType::MILITARY);
    lua_setglobal(lua.state, "TECH_MILITARY");
    lua_pushnumber(lua.state, TechnologyType::NAVY);
    lua_setglobal(lua.state, "TECH_NAVY");
    lua_pushnumber(lua.state, TechnologyType::SOCIAL);
    lua_setglobal(lua.state, "TECH_SOCIAL");
    lua_pushnumber(lua.state, TechnologyType::ECONOMIC);
    lua_setglobal(lua.state, "TECH_ECONOMIC");
    lua_pushnumber(lua.state, TechnologyType::POLITICS);
    lua_setglobal(lua.state, "TECH_POLITICS");

    // Policies control
#define POLICY_CAPITALIST 1
    lua_pushnumber(lua.state, POLICY_CAPITALIST);
    lua_setglobal(lua.state, "POLICY_CAPITALIST");
    lua_register(lua.state, "relative_nation_policy_stance", [](lua_State* L) {
        auto& nation = g_world.nations.at(lua_tonumber(L, 1));
        float val = lua_tonumber(L, 3);
        float ret_val = 0.f;
        auto& policy = nation.current_policy;
        switch((int)lua_tonumber(L, 2)) {
        case POLICY_CAPITALIST:
            ret_val = policy.economic.capitalism = glm::clamp(policy.economic.capitalism + val, -1.f, 1.f);
            break;
        default:
            break;
        }
        lua_pushnumber(L, ret_val);
        return 1;
    });

    lua_register(lua.state, "UI_CallBuiltin", LuaAPI::ui_call_builtin);

    // Set path for `require` statements in lua.state, this will allow us to require
    // without using data/scripts
    lua_getglobal(lua.state, "package");
    lua_getfield(lua.state, -1, "path");
    std::string curr_path = lua_tostring(lua.state, -1);

    // Add all scripts onto the path (with glob operator '?')
    const auto paths = Eng3D::State::get_instance().package_man.get_paths();
    for(const auto& path : paths) {
        Eng3D::Log::debug("lua", "Added path " + path);
        curr_path.append(";" + path + "/lua/?.lua");
    }
    lua_pop(lua.state, 1);
    lua_pushstring(lua.state, curr_path.data());
    lua_setfield(lua.state, -2, "path");
    lua_pop(lua.state, 1);
}

static void lua_exec_all_of(World& world, const std::vector<std::string> files, const std::string_view dir = "lua") {
    std::string files_buf = "require(\"classes/base\")\n\n";
    for(const auto& file : files) {
        auto paths = Eng3D::State::get_instance().package_man.get_multiple(std::string(dir) + "/" + file + ".lua");
        for(const auto& path : paths) {
#ifdef _WIN32
            std::string m_path;
            for(auto& c : path->abs_path)
                if(c == '\\') m_path += "\\\\";
                else m_path += c;
#else
            std::string m_path = path->abs_path;
#endif
            files_buf += "print(\"" + m_path + "\")\nassert(loadfile(\"" + m_path + "\"))()\n";
        }
    }
    Eng3D::Log::debug("lua", "Buffer " + files_buf);
    if(luaL_loadstring(world.lua.state, files_buf.data()) != LUA_OK || lua_pcall(world.lua.state, 0, 0, 0) != LUA_OK)
        CXX_THROW(Eng3D::LuaException, lua_tostring(world.lua.state, -1));
}

void World::load_initial() try {
    Eng3D::Deser::Archive ar{};
    ar.from_file("world.cch");
    Eng3D::Deser::deserialize(ar, Eng3D::StringManager::get_instance().strings);
    Eng3D::Deser::deserialize(ar, *this);
} catch(const std::exception& e) {
    Eng3D::Log::error("cache", e.what());

    // Execute all lua files
    lua_exec_all_of(*this, std::vector<std::string> {
        "terrain_types", "good_types", "ideologies", "languages",
            "building_types", "technology", "religions", "pop_types",
            "industry_types", "unit_types", "boat_types",
            "nations", "provinces", "init"
    }, "lua/entities");

    auto div = std::make_unique<Eng3D::BinaryImage>(Eng3D::State::get_instance().package_man.get_unique("map/provinces.png")->abs_path);
    width = div->width;
    height = div->height;
    tiles = std::make_unique<ProvinceId[]>(width * height);

    Eng3D::Log::debug("world", translate("Associate tiles with provinces"));

    // Build a lookup table for super fast speed on finding provinces
    // 16777216 * 4 = c.a 64 MB, that quite a lot but we delete the table after anyways
    Eng3D::Log::debug("world", translate("Building the province lookup table"));
    std::vector<ProvinceId> province_color_table(0xffffff + 1, ProvinceId(0));
    for(const auto& province : provinces)
        province_color_table[province.color & 0xffffff] = this->get_id(province);

    const auto* raw_buffer = div->buffer.get();
    tbb::parallel_for(static_cast<size_t>(0), height, [this, &province_color_table, raw_buffer](const auto j) {
        const auto off = j * width;
        for(size_t i = 0; i < width; i++)
            tiles[off + i] = province_color_table[raw_buffer[off + i] & 0xffffff];
    });

//#if 0
    std::set<uint32_t> colors_found;
    std::set<uint32_t> colors_used;
    for(size_t i = 0; i < width * height; i++) {
        const auto province_id = province_color_table[raw_buffer[i] & 0xffffff];
        if(province_id == (ProvinceId)0)
            colors_found.insert(raw_buffer[i]);
        colors_used.insert(raw_buffer[i] & 0xffffff);
    }
    if(!colors_found.empty()) {
        std::unique_ptr<FILE, int(*)(FILE*)> province_fp(fopen("uprovinces.lua", "w+t"), fclose);
        if(province_fp != nullptr) {
            for(const auto& color_raw : colors_found) {
                uint32_t color = color_raw << 8;
                fprintf(province_fp.get(), "province=Province:new{ref_name=\"province_%06x\",name=translate(\"Unknown\"),color=0x%06x,terrain=tt_sea,rgo_size={}}\n", static_cast<unsigned int>(std::byteswap<std::uint32_t>(color)), static_cast<unsigned int>(std::byteswap<std::uint32_t>(color)));
                fprintf(province_fp.get(), "province:register()\n");
            }
        }

        std::unique_ptr<FILE, int(*)(FILE*)> color_fp(fopen("ucolors.txt", "w+t"), fclose);
        if(color_fp != nullptr) {
            for(size_t i = 0; i < province_color_table.size(); i++) {
                if((i % 128 == 0) && (province_color_table[i] == ProvinceId(0))) {
                    const uint32_t color = i << 8;
                    fprintf(color_fp.get(), "%06lx\n", static_cast<unsigned long int>(std::byteswap<std::uint32_t>(color)));
                }
            }
        }

        // Flush files before throwing
        province_fp.reset();
        color_fp.reset();

        // Exit
        Eng3D::Log::error("world", "There are unregistered provinces, please register them!");
    }

    std::string provinces_ref_names = "";
    for(auto& province : provinces)
        if(!colors_used.contains(province.color & 0xffffff))
            provinces_ref_names += "'" + std::string{province.ref_name.get_string()} + "'";

    if(!provinces_ref_names.empty()) {
        std::string error = "Province " + provinces_ref_names + " is registered but missing on province.png, please add it!";
        Eng3D::Log::error("world", error);
        CXX_THROW(std::runtime_error, error.data());
    }
//#endif
    div.reset();

    // Calculate the edges of the province (min and max x and y coordinates)
    Eng3D::Log::debug("world", translate("Calculate the edges of the province (min and max x and y coordinates)"));

    // Init the province bounds
    for(auto& province : provinces) {
        province.box_area.right = province.box_area.bottom = 0.f;
        province.box_area.left = width;
        province.box_area.top = height;
    }

    for(size_t i = 0; i < width; i++) {
        for(size_t j = 0; j < height; j++) {
            auto& province = provinces[this->tiles[i + j * width]];
            province.box_area.left = glm::min(province.box_area.left, static_cast<float>(i));
            province.box_area.right = glm::max(province.box_area.right, static_cast<float>(i));
            province.box_area.bottom = glm::max(province.box_area.bottom, static_cast<float>(j));
            province.box_area.top = glm::min(province.box_area.top, static_cast<float>(j));
        }
    }

    // Correct stuff from provinces
    Eng3D::Log::debug("world", translate("Correcting values for provinces"));
    for(auto& province : provinces) {
        province.box_area.right = glm::min(width, static_cast<size_t>(province.box_area.right));
        province.box_area.bottom = glm::min(height, static_cast<size_t>(province.box_area.bottom));
    }

    // Neighbours
    Eng3D::Log::debug("world", translate("Calculating neighbours for provinces"));
    for(size_t i = 0; i < width * height; i++) {
        auto& province = this->provinces[this->tiles[i]];
        if(i > this->width) { // Up
            auto other_tile = this->tiles[i - this->width];
            province.neighbour_ids.push_back(other_tile);
        }
        if(i < (this->width * this->height) - this->width) { // Down
            auto other_tile = this->tiles[i + this->width];
            province.neighbour_ids.push_back(other_tile);
        }
        if(i > 1) { // Left
            auto other_tile = this->tiles[i - 1];
            province.neighbour_ids.push_back(other_tile);
        }
        if(i < (this->width * this->height) - 1) { // Right
            auto other_tile = this->tiles[i + 1];
            province.neighbour_ids.push_back(other_tile);
        }
    }

    // Remove neighbouring duplicates first
    for(auto& province : this->provinces) {
        auto last = std::unique(province.neighbour_ids.begin(), province.neighbour_ids.end());
        province.neighbour_ids.erase(last, province.neighbour_ids.end());
        std::erase(province.neighbour_ids, province); // Erase self
    }

    // Then sort and remove any remaining duplicates
    for(auto& province : this->provinces) {
        std::sort(province.neighbour_ids.begin(), province.neighbour_ids.end());
        auto last = std::unique(province.neighbour_ids.begin(), province.neighbour_ids.end());
        province.neighbour_ids.erase(last, province.neighbour_ids.end());
    }
    unit_manager.init(*this);

    // Create diplomatic relations between nations
    Eng3D::Log::debug("world", translate("Creating diplomatic relations"));
    // Relations between nations start at 0 (and latter modified by lua scripts)
    // since we use cantor's pairing function we only have to make an n*2 array so yeah let's do that!
    this->relations.resize(this->nations.size() * this->nations.size());

    // Auto-relocate capitals for countries which do not have one
    for(auto& nation : this->nations) {
        if(!nation.exists()) continue;
        //Eng3D::Log::debug("world", translate("Relocating capital of [" + nation.ref_name + "]"));
        nation.auto_relocate_capital();
    }

    // Write the entire world to the cache file
    Eng3D::Deser::Archive ar{};
    Eng3D::Deser::serialize(ar, Eng3D::StringManager::get_instance().strings);
    Eng3D::Deser::serialize(ar, *this);
    ar.to_file("world.cch");
}

void World::load_mod() {
    const std::vector<std::string> mod_files = {
        "mod", "postinit"
    };
    lua_exec_all_of(*this, mod_files, "lua/init");

    // Server needs now to sync changes to clients (changing state is not enough)
    this->needs_to_sync = true;
    Eng3D::Log::debug("game", translate("World fully intiialized"));
    AI::init(*this); // Initialize the AI
}

static inline void unit_do_tick(World& world, Unit& unit) {
    // Do not evaluate if we have an ongoing battle
    if(unit.on_battle) {
        unit.stop_movement();
        return;
    }

    auto& province = world.provinces[unit.province_id()];
    // Replenish units
    if(unit.size < unit.base)
        unit.size = glm::min<float>(unit.base, unit.size + unit.experience * 10.f);

    if(unit.has_target_province()) {
        assert(unit.get_target_province_id() != unit.province_id());
        auto& unit_target = world.provinces[unit.get_target_province_id()];
        bool can_move = true, can_take = false;
        if(unit_target.controller_id != unit.owner_id) {
            const auto& relation = world.get_relation(unit_target.controller_id, unit.owner_id);
            can_move = relation.has_landpass();
            can_take = relation.has_war;
        }

        if(can_move) {
            bool unit_moved = unit.update_movement(world.unit_manager);
            if(unit_moved && can_take) {
                // Moving to a province not owned by us!
                if(unit.owner_id != unit_target.owner_id) {
                    // Relation between original owner and the conqueree
                    const auto& relation = world.get_relation(unit_target.owner_id, unit.owner_id);
                    if(relation.is_allied()) // Allies will liberate countries implicitly and give back to the original owner
                        world.nations[unit_target.owner_id].control_province(unit_target);
                    else // Non allied means provinces aren't returned implicitly
                        world.nations[unit.owner_id].control_province(unit_target);
                } else { // Liberating our own provinces
                    world.nations[unit.owner_id].control_province(unit_target);
                }
            }
        }
    }
}

static inline void unit_do_battle_tick(World& world, Unit& unit) {
    if(unit.on_battle)
        return;

    auto& province = world.provinces[unit.province_id()];
    if(province.battle.active) {
        // TODO: let neutral people pass thru? or something more realistic
        // Can we join the attackers side?
        bool will_attack = false;
        for(const auto attacker_id : province.battle.attacker_nations_ids) {
            if(unit.owner_id == attacker_id) {
                will_attack = true;
                break;
            }
            const auto& relation = world.get_relation(unit.owner_id, attacker_id);
            if(relation.is_allied()) {
                will_attack = true;
                break;
            }
        }

        if(will_attack) {
            auto& nation_ids = province.battle.attacker_nations_ids;
            if(std::find(nation_ids.begin(), nation_ids.end(), unit.owner_id) == nation_ids.end())
                nation_ids.push_back(unit.owner_id);
        } else {
            auto& nation_ids = province.battle.defender_nations_ids;
            if(std::find(nation_ids.begin(), nation_ids.end(), unit.owner_id) == nation_ids.end())
                nation_ids.push_back(unit.owner_id);
        }
        province.battle.unit_ids.push_back(unit.get_id());
        unit.on_battle = true;
        unit.stop_movement();
    } else {
        // No battle on the current province, create a new one so check if we can start a new battle
        const auto& unit_ids = world.unit_manager.get_province_units(province);
        std::vector<UnitId> enemy_unit_ids;
        for(const auto other_unit_id : unit_ids) {
            auto& other_unit = world.unit_manager.units[other_unit_id];
            // Make sure the other unit isn't the same nation or already in battle, remember that if it's already
            // in battle then we can't probably join
            if(unit.owner_id == other_unit.owner_id)
                continue;
            // Check relations, if we're at war we will attack this unit
            const auto& relation = world.get_relation(unit.owner_id, other_unit.owner_id);
            if(!relation.has_war)
                continue;
            enemy_unit_ids.push_back(other_unit_id);
        }

        // If we found an unit we can attack, start a battle
        if(!enemy_unit_ids.empty()) {
            province.battle = Province::Battle();
            province.battle.active = true;

            // We attack, they defend from us
            unit.on_battle = true;
            unit.stop_movement();
            province.battle.unit_ids.push_back(unit);
            province.battle.attacker_nations_ids.push_back(unit.owner_id);

            std::vector<NationId> v;
            for(const auto enemy_unit_id : enemy_unit_ids) {
                auto& enemy_unit = world.unit_manager.units[enemy_unit_id];
                enemy_unit.on_battle = true;
                enemy_unit.stop_movement();
                province.battle.unit_ids.push_back(enemy_unit);
                v.push_back(enemy_unit.owner_id);
            }
            std::sort(v.begin(), v.end());
            v.erase(std::unique(v.begin(), v.end()), v.end());
            province.battle.defender_nations_ids = v;

            Eng3D::Log::debug("game", string_format("New battle on province %s", province.name.data()));
        }
    }

    // Assert no duplicate units in battles
    std::vector<UnitId> unit_ids;
    for(auto& province : world.provinces)
        if(province.battle.active)
            for(const auto unit_id : province.battle.unit_ids)
                unit_ids.push_back(unit_id);
    std::sort(unit_ids.begin(), unit_ids.end());
    assert(std::adjacent_find(unit_ids.begin(), unit_ids.end()) == unit_ids.end());
}

void World::fire_special_event(const std::string_view event_ref_name, const std::string_view nation_ref_name, const std::string_view other_nation_ref_name) {
    auto event_it = std::find_if(this->events.begin(), this->events.end(), [&](const auto& e) {
        return e.ref_name.get_string() == event_ref_name;
    });
    if(event_it == this->events.end())
        CXX_THROW(std::runtime_error, translate_format("Can't find special event %s", event_ref_name.data()));

    auto nation_it = std::find_if(this->nations.begin(), this->nations.end(), [&](const auto& e) {
        return e.ref_name.get_string() == nation_ref_name;
    });
    if(nation_it == this->nations.end())
        CXX_THROW(std::runtime_error, translate_format("Can't find the first nation %s for firing special event %s", nation_ref_name.data(), event_ref_name.data()));

    auto other_nation_it = std::find_if(this->nations.begin(), this->nations.end(), [&](const auto& e) {
        return e.ref_name.get_string() == other_nation_ref_name;
    });
    if(other_nation_it == this->nations.end())
        CXX_THROW(std::runtime_error, translate_format("Can't find the second nation %s for firing special event %s", nation_ref_name.data(), event_ref_name.data()));

    bool discard = false;
    LuaAPI::fire_event(this->lua.state, *nation_it, *event_it, discard, other_nation_ref_name);
}

void World::do_tick() {
    province_manager.clear();

    profiler.start("AI");
    AI::do_tick(*this);
    profiler.stop("AI");

    profiler.start("Economy");
    // Every ticks_per_month ticks do an economical tick
    Economy::do_tick(*this, economy_state);
    profiler.stop("Economy");

    profiler.start("E-packages");
    if(g_server != nullptr) {
        g_server->broadcast(Action::NationUpdate::form_packet(nations));
        g_server->broadcast(Action::ProvinceUpdate::form_packet(provinces));
    }
    profiler.stop("E-packages");

    profiler.start("Research");
    for(auto& nation : nations)
        nation.research[nation.focus_tech_id] += nation.get_research_points();
    profiler.stop("Research");

    profiler.start("Treaties");
    // Do the treaties clauses
    for(const auto& treaty : treaties) {
        if(!treaty.in_effect()) continue;

        // Treaties clauses now will be enforced
        Eng3D::Log::debug("game", string_format("Enforcing treaty %s", treaty.name));
        for(auto& clause : treaty.clauses) {
            assert(clause != nullptr);
            if(clause->type == TreatyClauseType::PAYMENT) {
                auto dyn_clause = static_cast<TreatyClause::Payment*>(clause);
                if(!dyn_clause->in_effect()) continue;
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::ANNEX_PROVINCES) {
                auto dyn_clause = static_cast<TreatyClause::AnnexProvince*>(clause);
                if(!dyn_clause->in_effect()) continue;
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::LIBERATE_NATION) {
                auto dyn_clause = static_cast<TreatyClause::LiberateNation*>(clause);
                if(!dyn_clause->in_effect()) continue;
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::HUMILIATE) {
                auto dyn_clause = static_cast<TreatyClause::Humiliate*>(clause);
                if(!dyn_clause->in_effect()) continue;
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::IMPOSE_POLICIES) {
                auto dyn_clause = static_cast<TreatyClause::ImposePolicies*>(clause);
                if(!dyn_clause->in_effect()) continue;
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::PUPPET) {
                auto dyn_clause = static_cast<TreatyClause::Puppet*>(clause);
                if(!dyn_clause->in_effect()) continue;
                dyn_clause->enforce();
            }
        }

        auto& relation = this->get_relation(treaty.sender_id, treaty.receiver_id);
        if(relation.has_war) {
            // Once treaty is signed, puppets of the sender will stop
            // war with the puppets of the receiver, and also stop war
            // with each other depending on the clauses
            relation.has_war = false;
        }
    }
    profiler.stop("Treaties");

    profiler.start("Units");
    // Evaluate units
    this->unit_manager.units.for_each([this](Unit& unit) {
        unit_do_tick(*this, unit);
    });
    this->unit_manager.units.for_each([this](Unit& unit) {
        unit_do_battle_tick(*this, unit);
    });
    profiler.stop("Units");

    // Perform all battles of the active wars
    profiler.start("Battles");
    std::vector<UnitId> clear_units;
    for(auto& province : provinces) {
        if(province.battle.active) {
            auto& units = this->unit_manager.units;
            const auto attacker_unit_ids = province.battle.get_attacker_unit_ids();
            const auto defender_unit_ids = province.battle.get_defender_unit_ids();

            // Assert that there are NO units on both teams
            std::vector<UnitId> v;
            std::set_intersection(attacker_unit_ids.begin(), attacker_unit_ids.end(), defender_unit_ids.begin(), defender_unit_ids.end(), std::back_inserter(v));
            assert(v.empty());

            // Once one side has fallen; this battle has ended
            if(defender_unit_ids.empty() || attacker_unit_ids.empty()) {
                if(!attacker_unit_ids.empty() && defender_unit_ids.empty()) {
                    // Attackers win
                    this->nations[units[attacker_unit_ids[0]].owner_id].control_province(province);
                } else if(attacker_unit_ids.empty() && !defender_unit_ids.empty()) {
                    // Defenders win
                    this->nations[units[defender_unit_ids[0]].owner_id].control_province(province);
                }

                for(const auto unit_id : attacker_unit_ids) {
                    auto& unit = units[unit_id];
                    this->nations[unit.owner_id].prestige += unit.base / 10000.f; // Prestige reward
                    unit.on_battle = false;
                }

                for(const auto unit_id : defender_unit_ids) {
                    auto& unit = units[unit_id];
                    this->nations[unit.owner_id].prestige += unit.base / 10000.f; // Prestige reward
                    unit.on_battle = false;
                }
                province.battle.active = false;
                continue;
            }

            for(auto attacker_id : attacker_unit_ids) {
                auto& attacker = units[attacker_id];
                for(auto defender_id : defender_unit_ids) {
                    auto& defender = units[defender_id];
                    province.battle.defender_casualties += attacker.attack(defender);
                    if(defender.size > 1.f)
                        province.battle.attacker_casualties += defender.attack(attacker);
                }
            }

            // Clear dead units
            for(size_t i = 0; i < province.battle.unit_ids.size(); ) {
                const auto unit_id = province.battle.unit_ids[i];
                auto& unit = units[unit_id];
                assert(unit_id == unit.get_id());
                if(unit.size < 1.f) {
                    Eng3D::Log::debug("game", string_format("Removing unit id=%zu from province %s", (size_t)unit_id, province.name.data()));
                    assert(std::find(clear_units.begin(), clear_units.end(), unit_id) == clear_units.end());
                    clear_units.push_back(unit_id);
                    province.battle.unit_ids.erase(province.battle.unit_ids.begin() + i);
                    continue;
                }
                i++;
            }
        }
    }
    profiler.stop("Battles");

    profiler.start("Cleaning");
    for(const auto unit_id : clear_units) {
        const auto& unit = this->unit_manager.units[unit_id];
        this->nations[unit.owner_id].prestige -= unit.base / 1000.f; // Prestige penalty for losing unit
        this->unit_manager.remove_unit(unit_id);
    }
    profiler.stop("Cleaning");

    profiler.start("Revolts");
    for(auto& province : provinces) {
        if(Nation::is_invalid(province.owner_id)) continue;
        auto& owner_nation = nations[province.owner_id];
        const auto militancy = province.average_militancy();
        if (0 && militancy > 0.5f) {
            bool has_found_revolt_nation = false;
            for(const auto nuclei_id : province.nuclei) {
                auto& revolt_nation = nations[nuclei_id];
                if(revolt_nation == owner_nation) continue;
                if(revolt_nation.owned_provinces.empty()) {
                    revolt_nation.declare_war(owner_nation);
                    revolt_nation.control_province(province);
                    const auto& unit_ids = unit_manager.get_province_units(province);
                    for(const auto unit_id : unit_ids) {
                        auto& unit = unit_manager.units[unit_id];
                        if(unit.owner_id == province.controller_id)
                            unit.set_owner(revolt_nation);
                    }
                    has_found_revolt_nation = true;
                    break;
                }
            }

            if (!has_found_revolt_nation) {
                for(auto& revolt_nation : nations) {
                    if(revolt_nation == owner_nation) continue;
                    if(revolt_nation.owned_provinces.empty()) {
                        revolt_nation.declare_war(owner_nation);
                        revolt_nation.control_province(province);
                        const auto& unit_ids = unit_manager.get_province_units(province);
                        for(const auto unit_id : unit_ids) {
                            auto& unit = unit_manager.units[unit_id];
                            if(unit.owner_id == province.controller_id)
                                unit.set_owner(revolt_nation);
                        }
                        has_found_revolt_nation = true;
                        break;
                    }
                }
            }
        }
    }
    profiler.stop("Revolts");

    profiler.start("Emigration");
    do_emigration(*this);
    profiler.stop("Emigration");

    profiler.start("Events");
    LuaAPI::check_events(this->lua.state);
    profiler.stop("Events");

    profiler.start("Send packets");
    if(g_server != nullptr)
        g_server->broadcast(Action::UnitUpdate::form_packet(this->unit_manager.units));
    profiler.stop("Send packets");

    if(!(time % ticks_per_month))
        Eng3D::Log::debug("game", Eng3D::translate_format("%i/%i/%i", time / 12 / ticks_per_month, (time / ticks_per_month % 12) + 1, (time % ticks_per_month) + 1));
    Eng3D::Log::debug("game", Eng3D::translate_format("Tick %i done", time));
    time++;

    if(g_server != nullptr) {
        // Tell clients that this tick has been done
        Eng3D::Networking::Packet packet{};
        Eng3D::Deser::Archive ar{};
        Eng3D::Deser::serialize<ActionType>(ar, ActionType::WORLD_TICK);
        Eng3D::Deser::serialize(ar, time);
        packet.data(ar.get_buffer(), ar.size());
        g_server->broadcast(packet);
    }
}
