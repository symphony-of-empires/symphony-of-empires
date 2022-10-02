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

#include "province.hpp"
#include "world.hpp"
#include "diplomacy.hpp"
#include "policy.hpp"
#include "server/lua_api.hpp"
#include "server/server_network.hpp"
#include "action.hpp"
#include "server/economy.hpp"

#undef min
#undef max

/// @brief List so we don't reconstruct a vector every time we query a tile
/// fortunely the size is a constant of 4 possible neighbours so we're good
static thread_local std::vector<Tile> tmp_tile_list = std::vector<Tile>();
const std::vector<Tile>& Tile::get_neighbours(const World& world) const {
    const auto i = world.get_id(*this);
    // Up
    if(i > world.width)
        tmp_tile_list.push_back(world.tiles[i - world.width]);
    // Down
    if(i < (world.width * world.height) - world.width)
        tmp_tile_list.push_back(world.tiles[i + world.width]);
    // Left
    if(i > 1)
        tmp_tile_list.push_back(world.tiles[i - 1]);
    // Right
    if(i < (world.width * world.height) - 1)
        tmp_tile_list.push_back(world.tiles[i + 1]);
    return tmp_tile_list;
}

World g_world;

namespace AI {
    void init(World& world);
    void do_tick(World& world);
}

// Creates a new world
void World::init_lua() {
    tmp_tile_list.reserve(4);

    lua = luaL_newstate();
    luaL_openlibs(lua);

    // Register our API functions
    lua_register(lua, "add_terrain_type", LuaAPI::add_terrain_type);
    lua_register(lua, "get_terrain_type", LuaAPI::get_terrain_type);
    lua_register(lua, "get_terrain_type_by_id", LuaAPI::get_terrain_type_by_id);

    lua_register(lua, "add_technology", LuaAPI::add_technology);
    lua_register(lua, "get_technology", LuaAPI::get_technology);
    lua_register(lua, "add_req_tech_to_tech", LuaAPI::add_req_tech_to_tech);

    lua_register(lua, "add_building_type", LuaAPI::add_building_type);
    lua_register(lua, "get_building_type", LuaAPI::get_building_type);

    lua_register(lua, "add_good", LuaAPI::add_good);
    lua_register(lua, "get_good", LuaAPI::get_good);

    lua_register(lua, "add_input_to_industry_type", LuaAPI::add_input_to_industry_type);
    lua_register(lua, "add_output_to_industry_type", LuaAPI::add_output_to_industry_type);
    lua_register(lua, "add_req_good_to_industry_type", LuaAPI::add_req_good_to_industry_type);
    lua_register(lua, "add_req_technology_to_industry_type", LuaAPI::add_req_technology_to_industry_type);

    lua_register(lua, "add_nation", LuaAPI::add_nation);
    lua_register(lua, "get_nation", LuaAPI::get_nation);
    lua_register(lua, "get_nation_by_id", LuaAPI::get_nation_by_id);
    lua_register(lua, "get_all_nations", LuaAPI::get_all_nations);
    lua_register(lua, "switch_nation_soul", LuaAPI::switch_nation_soul);
    lua_register(lua, "nation_declare_war_no_cb", LuaAPI::nation_declare_war_no_cb);

    lua_register(lua, "get_provinces_owned_by_nation", LuaAPI::get_provinces_owned_by_nation);
    lua_register(lua, "get_provinces_with_nucleus_by_nation", LuaAPI::get_provinces_with_nucleus_by_nation);
    lua_register(lua, "set_nation_primary_language", LuaAPI::set_nation_primary_language);
    lua_register(lua, "set_nation_capital", LuaAPI::set_nation_capital);
    lua_register(lua, "add_nation_accepted_language", LuaAPI::add_accepted_language);
    lua_register(lua, "add_nation_accepted_religion", LuaAPI::add_accepted_religion);
    lua_register(lua, "add_nation_client_hint", LuaAPI::add_nation_client_hint);
    lua_register(lua, "get_nation_policies", LuaAPI::get_nation_policies);
    lua_register(lua, "set_nation_policies", LuaAPI::set_nation_policies);
    lua_register(lua, "set_nation_ideology", LuaAPI::set_nation_ideology);
    lua_register(lua, "get_nation_relation", LuaAPI::get_nation_relation);
    lua_register(lua, "set_nation_relation", LuaAPI::set_nation_relation);
    lua_register(lua, "nation_declare_unjustified_war", LuaAPI::nation_declare_unjustified_war);
    lua_register(lua, "nation_make_puppet", LuaAPI::nation_make_puppet);
    
    lua_register(lua, "add_province", LuaAPI::add_province);
    lua_register(lua, "update_province", LuaAPI::update_province);
    lua_register(lua, "get_province", LuaAPI::get_province);
    lua_register(lua, "get_province_by_id", LuaAPI::get_province_by_id);
    lua_register(lua, "province_add_unit", LuaAPI::province_add_unit);
    lua_register(lua, "update_province_building", LuaAPI::update_province_building);
    lua_register(lua, "add_province_pop", LuaAPI::add_province_pop);
    lua_register(lua, "set_province_language", LuaAPI::set_province_language);
    lua_register(lua, "set_province_religion", LuaAPI::set_province_religion);
    lua_register(lua, "give_province_to", LuaAPI::give_province_to);
    lua_register(lua, "give_hard_province_to", LuaAPI::give_hard_province_to);
    lua_register(lua, "get_province_owner", LuaAPI::get_province_owner);
    lua_register(lua, "get_province_controller", LuaAPI::get_province_controller);
    lua_register(lua, "get_province_neighbours", LuaAPI::get_province_neighbours);
    lua_register(lua, "get_province_nuclei", LuaAPI::get_province_nuclei);
    lua_register(lua, "get_province_pops_size", LuaAPI::get_province_pops_size);
    lua_register(lua, "get_province_pop", LuaAPI::get_province_pop);
    lua_register(lua, "set_province_pop", LuaAPI::set_province_pop);
    lua_register(lua, "get_province_pop_ideology_approval", LuaAPI::get_province_pop_ideology_approval);
    lua_register(lua, "set_province_pop_ideology_approval", LuaAPI::set_province_pop_ideology_approval);
    lua_register(lua, "rename_province", LuaAPI::rename_province);
    lua_register(lua, "add_province_nucleus", LuaAPI::add_province_nucleus);
    lua_register(lua, "add_province_owner", LuaAPI::add_province_owner);

    lua_register(lua, "add_event", LuaAPI::add_event);
    lua_register(lua, "get_event", LuaAPI::get_event);
    lua_register(lua, "update_event", LuaAPI::update_event);
    lua_register(lua, "add_event_receivers", LuaAPI::add_event_receivers);

    lua_register(lua, "add_decision", LuaAPI::add_decision);

    lua_register(lua, "add_pop_type", LuaAPI::add_pop_type);
    lua_register(lua, "get_pop_type", LuaAPI::get_pop_type);
    lua_register(lua, "get_pop_type_by_id", LuaAPI::get_pop_type_by_id);

    lua_register(lua, "add_language", LuaAPI::add_language);
    lua_register(lua, "get_language", LuaAPI::get_language);
    lua_register(lua, "get_language_by_id", LuaAPI::get_language_by_id);

    lua_register(lua, "add_religion", LuaAPI::add_religion);
    lua_register(lua, "get_religion", LuaAPI::get_religion);
    lua_register(lua, "get_religion_by_id", LuaAPI::get_religion_by_id);

    lua_register(lua, "add_unit_type", LuaAPI::add_unit_type);
    lua_register(lua, "get_unit_type", LuaAPI::get_unit_type);
    lua_register(lua, "add_req_good_unit_type", LuaAPI::add_req_good_unit_type);

    lua_register(lua, "add_ideology", LuaAPI::add_ideology);
    lua_register(lua, "get_ideology", LuaAPI::get_ideology);
    lua_register(lua, "get_ideology_by_id", LuaAPI::get_ideology_by_id);

    lua_register(lua, "get_day", [](lua_State* L) {
        lua_pushnumber(L, 1 + (g_world.time % g_world.ticks_per_month));
        return 1;
    });
    lua_register(lua, "get_month", [](lua_State* L) {
        lua_pushnumber(L, 1 + (g_world.time / g_world.ticks_per_month % 12));
        return 1;
    });
    lua_register(lua, "get_year", [](lua_State* L) {
        lua_pushnumber(L, g_world.time / g_world.ticks_per_month / 12);
        return 1;
    });
    lua_register(lua, "set_date", [](lua_State* L) {
        const int year = lua_tonumber(L, 1) * 12 * 30;
        const int month = lua_tonumber(L, 2) * 30;
        const int day = lua_tonumber(L, 3);
        g_world.time = year + month + day;
        return 1;
    });

    // No translation is done
    lua_register(lua, "_", [](lua_State* L) {
        std::string msgid = luaL_checkstring(L, 1);
        lua_pushstring(L, msgid.c_str());
        return 1;
    });

    // And for the UI too
    lua_register(lua, "ui_new_button", LuaAPI::ui_new_button);
    lua_register(lua, "ui_new_div", LuaAPI::ui_new_div);
    lua_register(lua, "ui_new_group", LuaAPI::ui_new_group);
    lua_register(lua, "ui_new_image", LuaAPI::ui_new_image);
    lua_register(lua, "ui_new_checkbox", LuaAPI::ui_new_checkbox);
    lua_register(lua, "ui_set_checkbox_value", LuaAPI::ui_set_checkbox_value);
    lua_register(lua, "ui_get_checkbox_value", LuaAPI::ui_get_checkbox_value);
    lua_register(lua, "ui_new_slider", LuaAPI::ui_new_slider);
    lua_register(lua, "ui_get_slider_value", LuaAPI::ui_get_slider_value);
    lua_register(lua, "ui_set_slider_value", LuaAPI::ui_set_slider_value);
    lua_register(lua, "ui_new_label", LuaAPI::ui_new_label);
    lua_register(lua, "ui_new_window", LuaAPI::ui_new_window);
    lua_register(lua, "ui_get_image", LuaAPI::ui_get_image);
    lua_register(lua, "ui_set_image", LuaAPI::ui_set_image);
    lua_register(lua, "ui_set_scroll", LuaAPI::ui_set_scroll);
    lua_register(lua, "ui_set_text", LuaAPI::ui_set_text);
    lua_register(lua, "ui_set_on_click", LuaAPI::ui_set_on_click);
    lua_register(lua, "ui_set_window_on_click_close_btn", LuaAPI::ui_set_window_on_click_close_btn);
    lua_register(lua, "ui_get_widget", LuaAPI::ui_get_widget);
    lua_register(lua, "ui_widget_kill", LuaAPI::ui_widget_kill);
    lua_register(lua, "ui_widget_set_tooltip", LuaAPI::ui_widget_set_tooltip);
    lua_register(lua, "UI_RegisterCallback", LuaAPI::ui_register_callback);
    lua_register(lua, "ui_widget_set_flex", LuaAPI::ui_widget_set_flex);
    lua_register(lua, "UI_CallBuiltin", LuaAPI::ui_call_builtin);

    /*LuaAPI::register_new_table(lua, "Ideology", {}, {
        { "test", [](lua_State* L) {
            Eng3D::Log::debug("lua_test", "hello world");
            return 0;
        }}
    });*/

    // Constants for ease of readability
    lua_pushboolean(lua, true);
    lua_setglobal(lua, "EVENT_CONDITIONS_MET");
    lua_pushboolean(lua, false);
    lua_setglobal(lua, "EVENT_CONDITIONS_UNMET");

    lua_pushboolean(lua, true);
    lua_setglobal(lua, "EVENT_DO_MANY_TIMES");
    lua_pushboolean(lua, false);
    lua_setglobal(lua, "EVENT_DO_ONE_TIME");

    // Technology types
    lua_pushnumber(lua, TechnologyType::STRATEGIC);
    lua_setglobal(lua, "TECH_STRATEGIC");
    lua_pushnumber(lua, TechnologyType::MILITARY);
    lua_setglobal(lua, "TECH_MILITARY");
    lua_pushnumber(lua, TechnologyType::NAVY);
    lua_setglobal(lua, "TECH_NAVY");
    lua_pushnumber(lua, TechnologyType::SOCIAL);
    lua_setglobal(lua, "TECH_SOCIAL");
    lua_pushnumber(lua, TechnologyType::ECONOMIC);
    lua_setglobal(lua, "TECH_ECONOMIC");
    lua_pushnumber(lua, TechnologyType::POLITICS);
    lua_setglobal(lua, "TECH_POLITICS");

    // Set path for `require` statements in lua, this will allow us to require
    // without using data/scripts
    lua_getglobal(lua, "package");
    lua_getfield(lua, -1, "path");
    std::string curr_path = lua_tostring(lua, -1);

    // Add all scripts onto the path (with glob operator '?')
    const auto paths = Eng3D::State::get_instance().package_man.get_paths();
    for(const auto& path : paths) {
        Eng3D::Log::debug("lua", "Added path " + path);
        curr_path.append(";" + path + "/lua/?.lua");
    }
    lua_pop(lua, 1);
    lua_pushstring(lua, curr_path.c_str());
    lua_setfield(lua, -2, "path");
    lua_pop(lua, 1);
}

World::~World() {
    if(lua != nullptr)
        lua_close(lua);
}

static void lua_exec_all_of(World& world, const std::vector<std::string> files, const std::string& dir = "lua") {
    std::string files_buf = "require(\"classes/base\")\n\n";
    for(const auto& file : files) {
        auto paths = Eng3D::State::get_instance().package_man.get_multiple(dir + "/" + file + ".lua");
        for(const auto& path : paths) {
#ifdef E3D_TARGET_WINDOWS
            std::string m_path;
            for(auto& c : path->get_abs_path())
                if(c == '\\') m_path += "\\\\";
                else m_path += c;
#else
            std::string m_path = path->get_abs_path();
#endif
            files_buf += "print(\"" + m_path + "\")\nloadfile(\"" + m_path + "\")()\n";
        }
    }
    Eng3D::Log::debug("lua", "Buffer " + files_buf);
    if(luaL_loadstring(world.lua, files_buf.c_str()) != LUA_OK || lua_pcall(world.lua, 0, 0, 0) != LUA_OK)
        CXX_THROW(LuaAPI::Exception, lua_tostring(world.lua, -1));
}

void World::load_initial() {
    try {
        Archive ar{};
        ar.from_file("world.cache");
        std::string creat_date;
        ::deserialize(ar, creat_date);
        if(creat_date != __DATE__) CXX_THROW(std::runtime_error, "Unmatching cache \"" + creat_date + "\"(" + std::to_string(creat_date.size()) + ") != \"" + __DATE__ + "\"");
        ::deserialize(ar, *this);
    } catch(const std::exception& e) {
        Eng3D::Log::error("cache", e.what());

        // Execute all lua files
        lua_exec_all_of(*this, std::vector<std::string> {
            "terrain_types", "good_types", "ideologies", "languages",
            "building_types", "technology", "religions", "pop_types",
            "industry_types", "unit_types", "boat_types",
            "nations", "provinces", "init"
        }, "lua/entities");

        auto div = std::make_unique<Eng3D::BinaryImage>(Eng3D::State::get_instance().package_man.get_unique("map/provinces.png")->get_abs_path());
        width = div->width;
        height = div->height;
        tiles = std::make_unique<Tile[]>(width * height);

        // Uncomment this and see a bit more below
#if 0
        std::set<uint32_t> colors_found;
        std::set<uint32_t> colors_used;
#endif
        Eng3D::Log::debug("world", translate("Associate tiles with provinces"));

        // Build a lookup table for super fast speed on finding provinces
        // 16777216 * 4 = c.a 64 MB, that quite a lot but we delete the table after anyways
        Eng3D::Log::debug("world", translate("Building the province lookup table"));
        std::vector<ProvinceId> province_color_table(0xffffff + 1, ProvinceId(-1));
        for(const auto& province : provinces)
            province_color_table[province.color & 0xffffff] = this->get_id(province);

        const auto* raw_buffer = div->buffer.get();
        for(size_t i = 0; i < width * height; i++) {
            const ProvinceId province_id = province_color_table[raw_buffer[i] & 0xffffff];
#if 0
            if(province_id == (ProvinceId)-1)
                colors_found.insert(raw_buffer[i]);
            colors_used.insert(raw_buffer[i] & 0xffffff);
#endif
            tiles[i].province_id = province_id;
        }
        div.reset();

#if 0
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
                    if(i % 128 == 0 && Province::is_invalid(province_color_table[i])) {
                        const uint32_t color = i << 8;
                        fprintf(color_fp.get(), "%06lx\n", static_cast<unsigned long int>(std::byteswap<std::uint32_t>(color)));
                    }
                }
            }

            // Flush files before throwing
            province_fp.reset();
            color_fp.reset();

            // Exit
            CXX_THROW(std::runtime_error, "There are unregistered provinces, please register them!");
        }

        std::string provinces_ref_names = "";
        for(auto& province : provinces)
            if(!colors_used.contains(province.color & 0xffffff))
                provinces_ref_names += "'" + province.ref_name + "'";

        if(!provinces_ref_names.empty()) {
            std::string error = "Province " + provinces_ref_names + " is registered but missing on province.png, please add it!";
            Eng3D::Log::error("world", error);
            CXX_THROW(std::runtime_error, error.c_str());
        }
#endif

        // Calculate the edges of the province (min and max x and y coordinates)
        Eng3D::Log::debug("world", translate("Calculate the edges of the province (min and max x and y coordinates)"));

        // Init the province bounds
        for(auto& province : provinces) {
            province.box_area.right = 0;
            province.box_area.bottom = 0;
            province.box_area.left = width;
            province.box_area.top = height;
        }

        tbb::parallel_for(static_cast<size_t>(0), height, [this](const auto j) {
            for(size_t i = 0; i < width; i++) {
                const auto& tile = this->get_tile(i, j);
                auto& province = provinces[tile.province_id];
                province.box_area.left = glm::min(province.box_area.left, static_cast<float>(i));
                province.box_area.right = glm::max(province.box_area.right, static_cast<float>(i));
                province.box_area.bottom = glm::max(province.box_area.bottom, static_cast<float>(j));
                province.box_area.top = glm::min(province.box_area.top, static_cast<float>(j));
            }
        });

        // Correct stuff from provinces
        Eng3D::Log::debug("world", translate("Correcting values for provinces"));
        for(auto& province : provinces) {
            province.box_area.right = glm::min(width, static_cast<size_t>(province.box_area.right));
            province.box_area.bottom = glm::min(height, static_cast<size_t>(province.box_area.bottom));
        }

        // Neighbours
        Eng3D::Log::debug("world", translate("Calculating neighbours for provinces"));
        for(size_t i = 0; i < width * height; i++) {
            auto& province = this->provinces[this->tiles[i].province_id];
            if(i > this->width) { // Up
                auto other_tile = this->tiles[i - this->width];
                province.neighbour_ids.push_back(other_tile.province_id);
            }
            if(i < (this->width * this->height) - this->width) { // Down
                auto other_tile = this->tiles[i + this->width];
                province.neighbour_ids.push_back(other_tile.province_id);
            }
            if(i > 1) { // Left
                auto other_tile = this->tiles[i - 1];
                province.neighbour_ids.push_back(other_tile.province_id);
            }
            if(i < (this->width * this->height) - 1) { // Right
                auto other_tile = this->tiles[i + 1];
                province.neighbour_ids.push_back(other_tile.province_id);
            }
        }
        for(auto& province : this->provinces) {
            // Remove duplicates
            auto last = std::unique(province.neighbour_ids.begin(), province.neighbour_ids.end());
            province.neighbour_ids.erase(last, province.neighbour_ids.end());
            std::erase(province.neighbour_ids, province); // Erase self
        }
        unit_manager.init(*this);

        // Create diplomatic relations between nations
        Eng3D::Log::debug("world", translate("Creating diplomatic relations"));
        // Relations between nations start at 0 (and latter modified by lua scripts)
        // since we use cantor's pairing function we only have to make an n*2 array so yeah let's do that!
        size_t relations_len = this->nations.size() * this->nations.size();
        this->relations = std::make_unique<NationRelation[]>(relations_len);
        for(size_t i = 0; i < relations_len; i++)
            this->relations[i] = NationRelation{};
        
        // Auto-relocate capitals for countries which do not have one
        for(auto& nation : this->nations) {
            if(!nation.exists() || Province::is_invalid(nation.capital_id)) continue;
            //Eng3D::Log::debug("world", translate("Relocating capital of [" + nation.ref_name + "]"));
            nation.auto_relocate_capital();
        }

        // Write the entire world to the cache file
        Archive ar{};
        std::string creat_date = __DATE__;
        ::serialize(ar, creat_date);
        ::serialize(ar, *this);
        ar.to_file("world.cache");
    }

    Eng3D::Log::debug("world", translate("World partially intiialized"));
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
    assert(Province::is_valid(unit.province_id()));
    //assert(Nation::is_valid(unit.owner_id));

    // Do not evaluate if we have an ongoing battle
    if(unit.on_battle) {
        unit.stop_movement();
        return;
    }

    const auto& province = world.provinces[unit.province_id()];
    const auto weight_factor = glm::max(0.01f, unit.size / 50000.f);
    // Analyze neighbours of where the unit is standing on
    for(const auto neighbour_id : province.neighbour_ids) {
        const auto& neighbour = world.provinces[neighbour_id];
        if(neighbour.controller_id != unit.owner_id && Nation::is_valid(neighbour.controller_id)) {
            // Decrease relations if we're militarizing our border
            auto& relation = world.get_relation(neighbour.controller_id, unit.owner_id);
            if(!relation.has_landpass())
                relation.relation = glm::clamp<float>(relation.relation - weight_factor, -100.f, 100.f);
        }
    }

    // Replenish units
    if(unit.size < unit.base)
        unit.size = glm::min<float>(unit.base, unit.size + unit.experience * 10.f);

    if(Province::is_valid(unit.get_target_province_id())) {
        assert(unit.get_target_province_id() != unit.province_id());

        auto& unit_target = world.provinces[unit.get_target_province_id()];
        bool can_move = true, can_take = false;
        if(Nation::is_valid(unit_target.controller_id) && unit_target.controller_id != unit.owner_id) {
            const auto& relation = world.get_relation(unit_target.controller_id, unit.owner_id);
            can_move = relation.has_landpass();
            can_take = relation.has_war;
        }

        if(can_move) {
            bool unit_moved = unit.update_movement(world.unit_manager);
            if(unit_moved && can_take) {
                // Moving to a province not owned by us!
                if(unit.owner_id != unit_target.owner_id && Nation::is_valid(unit_target.owner_id)) {
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
    
    auto prov_id = world.unit_manager.get_unit_current_province(unit);
    auto& unit_province = world.provinces[prov_id];
    // Handle battles
    if(!unit.on_battle) {
        const auto& unit_nation = world.nations[unit.owner_id];
        // Get the first province_battles that we are involved in
        auto prov_battles_it = std::find_if(unit_province.battles.begin(), unit_province.battles.end(), [&unit_nation, &world](const auto& e) {
            return world.wars[e.war_id].is_involved(unit_nation);
        });
        if(prov_battles_it != unit_province.battles.end()) { 
            // If there is a relevant battle in the province the unit joins the battle
            unit.on_battle = true;
            unit.stop_movement();
            auto& battle = *prov_battles_it;
            auto& war = world.wars[battle.war_id];

            // Add the unit to one side depending on who are we attacking however unit must not be already involved
            /// @todo Make it be instead depending on who attacked first in this battle
            assert(std::find(battle.attackers_ids.begin(), battle.attackers_ids.end(), unit) == battle.attackers_ids.end());
            assert(std::find(battle.defenders_ids.begin(), battle.defenders_ids.end(), unit) == battle.defenders_ids.end());
            if(war.is_attacker(unit_nation)) {
                battle.attackers_ids.push_back(unit);
                //Eng3D::Log::debug("game", "Adding unit <attacker> to battle of \"" + battle.name + "\"");
            } else {
                assert(war.is_defender(unit_nation));
                battle.defenders_ids.push_back(unit);
                //Eng3D::Log::debug("game", "Adding unit <defender> to battle of \"" + battle.name + "\"");
            }
        } else {
            // No battle on the current province, create a new one so check if we can start a new battle
            const auto& unit_ids = world.unit_manager.get_province_units(prov_id);
            for(const auto other_unit_id : unit_ids) {
                auto& other_unit = world.unit_manager.units[other_unit_id];
                // Make sure the other unit isn't the same nation or already in battle, remember that if it's already
                // in battle then we can't probably join
                if(unit.owner_id == other_unit.owner_id || other_unit.on_battle) continue;
                // Check relations, if we're at war we will attack this unit
                const auto& relation = world.get_relation(unit.owner_id, other_unit.owner_id);
                if(relation.has_war) {
                    // Find war with both nations
                    const auto& other_unit_nation = world.nations[unit.owner_id];
                    auto wars_it = std::find_if(world.wars.begin(), world.wars.end(), [&unit_nation, &other_unit_nation](const auto& e) {
                        return e.is_involved(unit_nation) && e.is_involved(other_unit_nation);
                    });
                    // Since `relation.has_war` is true we have to find at least one war
                    assert(wars_it != world.wars.end());
                    auto& war = *wars_it;

                    // If we found an unit we can attack, start a battle
                    unit.on_battle = true;
                    unit.stop_movement();
                    other_unit.on_battle = true;
                    other_unit.stop_movement();
                    // Create a new battle
                    Battle battle(war);
                    battle.name = "battleName";
                    if(war.is_attacker(world.nations[unit.owner_id])) {
                        battle.attackers_ids.emplace_back(unit);
                        battle.defenders_ids.emplace_back(other_unit);
                    } else {
                        battle.attackers_ids.emplace_back(other_unit);
                        battle.defenders_ids.emplace_back(unit);
                    }
                    unit_province.battles.push_back(battle);
                    Eng3D::Log::debug("game", string_format("New battle of \"%s\"", battle.name.c_str()));
                    break;
                }
            }
        }
    }
}

void World::do_tick() {
    province_manager.clear();

    profiler.start("AI");
    AI::do_tick(*this);
    profiler.stop("AI");

    profiler.start("Economy");
    // Every ticks_per_month ticks do an economical tick
    Economy::do_tick(*this, economyState);
    profiler.stop("Economy");

    profiler.start("E-packages");
    g_server->broadcast(Action::NationUpdate::form_packet(nations));
    g_server->broadcast(Action::ProvinceUpdate::form_packet(provinces));
    profiler.stop("E-packages");

    profiler.start("Research");
    std::vector<float> mil_research_pts(nations.size(), 0.f);
    std::vector<float> naval_research_pts(nations.size(), 0.f);
    // Now researches for every country are going to be accounted :)
    for(auto& nation : nations) {
        for(const auto& technology : technologies) {
            if(!nation.can_research(technology)) continue;
            float* research_progress = &nation.research[get_id(technology)];
            if(!(*research_progress)) continue;

            float* pts_count;
            if(technology.type == TechnologyType::MILITARY) {
                pts_count = &mil_research_pts[get_id(nation)];
            } else if(technology.type == TechnologyType::NAVY) {
                pts_count = &naval_research_pts[get_id(nation)];
            } else {
                continue;
            }
            if(*pts_count <= 0.f) continue;

            const float pts = *pts_count / 4.f;
            *research_progress += pts;
            *pts_count -= pts;
            break;
        }
    }
    profiler.stop("Research");

    profiler.start("Treaties");
    // Do the treaties clauses
    for(const auto& treaty : treaties) {
        // Check that the treaty is agreed by all parties before enforcing it
        bool on_effect = std::find_if(treaty.approval_status.begin(), treaty.approval_status.end(), [](auto& status) { return (status.second != TreatyApproval::ACCEPTED); }) == treaty.approval_status.end();
        if(!on_effect) continue;
        // Check with treaty
        if(!treaty.in_effect()) continue;

        // Treaties clauses now will be enforced
        Eng3D::Log::debug("game", string_format("Enforcing treaty %s", treaty.name));
        for(auto& clause : treaty.clauses) {
            assert(clause != nullptr);
            if(clause->type == TreatyClauseType::MONEY) {
                auto dyn_clause = static_cast<TreatyClause::WarReparations*>(clause);
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

        auto& relation = g_world.get_relation(treaty.sender_id, treaty.receiver_id);
        if(relation.has_war) {
            // Remove the receiver from the wars
            for(size_t i = 0; i < g_world.wars.size(); i++) {
                auto& war = g_world.wars[i];
                // All people participating stop war with each other
                for(const auto& [nation, _] : treaty.approval_status) {
                    auto it = std::find(war.attacker_ids.begin(), war.attacker_ids.end(), nation);
                    if(it != war.attacker_ids.end()) war.attacker_ids.erase(it);
                    else std::erase(war.defender_ids, nation);
                    // Stop the war between the nations
                    for(const auto& [other_nation, _] : treaty.approval_status)
                        if(nation != other_nation)
                            g_world.get_relation(treaty.sender_id, treaty.receiver_id).has_war = false;
                }

                if(war.attacker_ids.empty() || war.defender_ids.empty()) { // Once nobody is in a war remove it from the world
                    Eng3D::Log::debug("war", translate_format("War %s finished", war.name));
                    g_world.wars.erase(g_world.wars.begin() + i);
                    break;
                }
            }
            relation.has_war = false;
        }
    }
    profiler.stop("Treaties");

    wcmap_mutex.lock();
    profiler.start("Units");
    // Evaluate units
    this->unit_manager.for_each_unit([this](Unit& unit) {
        unit_do_tick(*this, unit);
    });
    profiler.stop("Units");

    // Perform all battles of the active wars
    profiler.start("Battles");
    tbb::combinable<std::vector<UnitId>> clear_units;
    tbb::parallel_for(tbb::blocked_range(provinces.begin(), provinces.end()), [this, &clear_units](auto& provinces_range) {
        for(auto& province : provinces_range) {
            for(size_t j = 0; j < province.battles.size(); j++) {
                auto& battle = province.battles[j];
                // Attackers attack Defenders
                auto& units = this->unit_manager.units;
                for(auto attacker_id : battle.attackers_ids) {
                    auto& attacker = units[attacker_id];
                    assert(attacker.is_valid());
                    for(size_t i = 0; i < battle.defenders_ids.size(); ) {
                        auto& unit = units[battle.defenders_ids[i]];
                        assert(unit.is_valid());
                        const auto prev_size = unit.size;
                        attacker.attack(unit);
                        battle.defender_casualties += prev_size - unit.size;
                        if(unit.size <= 1.f) {
                            //Eng3D::Log::debug("game", "Removing attacker \"" + this->unit_types[unit.type_id].ref_name + "\" unit to battle of \"" + battle.name + "\"");
                            battle.defenders_ids.erase(battle.defenders_ids.begin() + i);
                            clear_units.local().push_back(unit);
                            continue;
                        }
                        i++;
                    }
                }

                // Defenders attack attacker_ids
                for(auto defender_id : battle.defenders_ids) {
                    auto& defender = units[defender_id];
                    assert(defender.is_valid());
                    for(size_t i = 0; i < battle.attackers_ids.size(); ) {
                        auto& unit = units[battle.attackers_ids[i]];
                        assert(unit.is_valid());
                        const auto prev_size = unit.size;
                        defender.attack(unit);
                        battle.attacker_casualties += prev_size - unit.size;
                        if(unit.size <= 1.f) {
                            //Eng3D::Log::debug("game", "Removing defender \"" + this->unit_types[unit.type_id].ref_name + "\" unit to battle of \"" + battle.name + "\"");
                            battle.attackers_ids.erase(battle.attackers_ids.begin() + i);
                            clear_units.local().push_back(unit);
                            continue;
                        }
                        i++;
                    }
                }

                // Once one side has fallen this battle has ended
                if(battle.defenders_ids.empty() || battle.attackers_ids.empty()) {
                    // Defenders defeated
                    if(battle.defenders_ids.empty()) {
                        this->nations[units[battle.attackers_ids[0]].owner_id].control_province(province);
                        // Clear flags of all units
                        for(const auto unit_id : battle.attackers_ids) {
                            auto& unit = units[unit_id];
                            this->nations[unit.owner_id].prestige += unit.base / 10000.f; // Prestige reward
                            unit.on_battle = false;
                            assert(unit.size);
                        }
                        //Eng3D::Log::debug("game", "Battle \"" + battle.name + "\": attacker_ids win");
                    }
                    // Defenders won
                    else {
                        this->nations[units[battle.defenders_ids[0]].owner_id].control_province(province);
                        for(const auto unit_id : battle.defenders_ids) {
                            auto& unit = units[unit_id];
                            this->nations[unit.owner_id].prestige += unit.base / 10000.f; // Prestige reward
                            unit.on_battle = false;
                            assert(unit.size);
                        }
                        //Eng3D::Log::debug("game", "Battle \"" + battle.name + "\": defender_ids win");
                    }
                    // Erase the battle from the province
                    province.battles.erase(province.battles.begin() + j);
                    j--;
                    continue;
                }
            }
        }
    });
    profiler.stop("Battles");

    profiler.start("Cleaning");
    clear_units.combine_each([this](const auto& units_clear_list) {
        for(auto unit_id : units_clear_list) {
            auto& unit = this->unit_manager.units[unit_id];
            this->nations[unit.owner_id].prestige -= unit.base / 1000.f; // Prestige penalty for losing unit
            this->unit_manager.remove_unit(unit_id);
        }
    });
    profiler.stop("Cleaning");
    wcmap_mutex.unlock();

    //profiler.start("Events");
    //LuaAPI::check_events(lua);
    //profiler.stop("Events");

    // Remove empty pops and combine similar pops
    tbb::parallel_for(tbb::blocked_range(provinces.begin(), provinces.end()), [](auto& province_ranges) {
        for(auto& province : province_ranges)
            province.clean_pops();
    });

    profiler.start("Send packets");
    g_server->broadcast(Action::UnitUpdate::form_packet(this->unit_manager.units));
    profiler.stop("Send packets");

    if(!(time % ticks_per_month))
        Eng3D::Log::debug("game", std::to_string(time / 12 / ticks_per_month) + "/" + std::to_string((time / ticks_per_month % 12) + 1) + +"/" + std::to_string((time % ticks_per_month) + 1));
    Eng3D::Log::debug("game", "Tick " + std::to_string(time) + " done");
    time++;
    
    // Tell clients that this tick has been done
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::WORLD_TICK);
    packet.data(ar.get_buffer(), ar.size());
    g_server->broadcast(packet);
}
