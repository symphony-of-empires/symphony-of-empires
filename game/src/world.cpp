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
//      world.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <execution>
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>

#include "eng3d/binary_image.hpp"
#include "eng3d/path.hpp"
#include "eng3d/log.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/state.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/locale.hpp"
#include "eng3d/assert.hpp"

#include "io_impl.hpp"
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

#if (__cplusplus < 201703L)
namespace std {
    template<typename T>
    constexpr inline T clamp(T n, T min, T max) {
        return std::min<T>(max, std::max<T>(n, min));
    }
}
#endif

const std::vector<const Tile*> Tile::get_neighbours(const World& world) const {
    std::vector<const Tile*> tiles;
    const size_t idx = world.get_id(*this);

    // Up
    if(idx > world.width) {
        tiles.push_back(&world.tiles[idx - world.width]);
    }
    // Down
    if(idx < (world.width * world.height) - world.width) {
        tiles.push_back(&world.tiles[idx + world.width]);
    }
    // Left
    if(idx > 1) {
        tiles.push_back(&world.tiles[idx - 1]);
    }
    // Right
    if(idx < (world.width * world.height) - 1) {
        tiles.push_back(&world.tiles[idx + 1]);
    }
    return tiles;
}

World* g_world;

World& World::get_instance(void) {
    return *g_world;
}

void ai_init(World& world);
void ai_do_tick(Nation& nation);

// Creates a new world
World::World() {
    g_world = this;

    lua = luaL_newstate();
    luaL_openlibs(lua);

    // Register our API functions
    lua_register(lua, "add_terrain_type", LuaAPI::add_terrain_type);
    lua_register(lua, "get_terrain_type", LuaAPI::get_terrain_type);
    lua_register(lua, "get_terrain_type_by_id", LuaAPI::get_terrain_type_by_id);

    lua_register(lua, "add_technology", LuaAPI::add_technology);
    lua_register(lua, "get_technology", LuaAPI::get_technology);
    lua_register(lua, "add_req_tech_to_tech", LuaAPI::add_req_tech_to_tech);

    lua_register(lua, "set_nation_mod_to_invention", LuaAPI::set_nation_mod_to_invention);

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
    lua_register(lua, "nation_declare_war_no_cb", LuaAPI::nation_declare_war_no_cb);

    lua_register(lua, "get_provinces_owned_by_nation", LuaAPI::get_provinces_owned_by_nation);
    lua_register(lua, "get_provinces_with_nucleus_by_nation", LuaAPI::get_provinces_with_nucleus_by_nation);
    lua_register(lua, "set_nation_primary_culture", LuaAPI::set_nation_primary_culture);
    lua_register(lua, "set_nation_capital", LuaAPI::set_nation_capital);
    lua_register(lua, "add_nation_accepted_culture", LuaAPI::add_accepted_culture);
    lua_register(lua, "add_nation_accepted_religion", LuaAPI::add_accepted_religion);
    lua_register(lua, "add_nation_client_hint", LuaAPI::add_nation_client_hint);
    lua_register(lua, "get_nation_policies", LuaAPI::get_nation_policies);
    lua_register(lua, "set_nation_policies", LuaAPI::set_nation_policies);
    lua_register(lua, "set_nation_ideology", LuaAPI::set_nation_ideology);
    lua_register(lua, "get_nation_relation", LuaAPI::get_nation_relation);
    lua_register(lua, "set_nation_relation", LuaAPI::set_nation_relation);
    lua_register(lua, "nation_declare_unjustified_war", LuaAPI::nation_declare_unjustified_war);
    lua_register(lua, "nation_make_puppet", LuaAPI::nation_make_puppet);

    lua_register(lua, "add_nation_mod", LuaAPI::add_nation_mod);
    lua_register(lua, "get_nation_mod", LuaAPI::get_nation_mod);

    lua_register(lua, "add_province", LuaAPI::add_province);
    lua_register(lua, "update_province", LuaAPI::update_province);
    lua_register(lua, "get_province", LuaAPI::get_province);
    lua_register(lua, "get_province_by_id", LuaAPI::get_province_by_id);
    lua_register(lua, "province_add_unit", LuaAPI::province_add_unit);
    lua_register(lua, "update_province_building", LuaAPI::update_province_building);
    lua_register(lua, "add_province_pop", LuaAPI::add_province_pop);
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

    lua_register(lua, "add_culture", LuaAPI::add_culture);
    lua_register(lua, "get_culture", LuaAPI::get_culture);
    lua_register(lua, "get_culture_by_id", LuaAPI::get_culture_by_id);

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
        lua_pushnumber(L, 1 + (g_world->time % g_world->ticks_per_month));
        return 1;
    });
    lua_register(lua, "get_month", [](lua_State* L) {
        lua_pushnumber(L, 1 + (g_world->time / g_world->ticks_per_month % 12));
        return 1;
    });
    lua_register(lua, "get_year", [](lua_State* L) {
        lua_pushnumber(L, g_world->time / g_world->ticks_per_month / 12);
        return 1;
    });
    lua_register(lua, "set_date", [](lua_State* L) {
        const int year = lua_tonumber(L, 1) * 12 * 30;
        const int month = lua_tonumber(L, 2) * 30;
        const int day = lua_tonumber(L, 3);
        g_world->time = year + month + day;
        return 1;
    });

    // No translation is done
    lua_register(lua, "_", [](lua_State* L) {
        std::string msgid = luaL_checkstring(L, 1);
        lua_pushstring(L, msgid.c_str());
        return 1;
    });

    const struct luaL_Reg ideology_meta[] = {
        { "__gc", [](lua_State* L) {
            Eng3D::Log::debug("lua", "__gc?");
            return 0;
        }},
        { "__index", [](lua_State* L) {
            Ideology** ideology = (Ideology**)luaL_checkudata(L, 1, "Ideology");
            std::string member = luaL_checkstring(L, 2);
            if(member == "ref_name") {
                lua_pushstring(L, (*ideology)->ref_name.c_str());
            } else if(member == "name") {
                lua_pushstring(L, (*ideology)->name.c_str());
            }
            Eng3D::Log::debug("lua", "__index?");
            return 1;
        }},
        { "__newindex", [](lua_State* L) {
            Ideology** ideology = (Ideology**)luaL_checkudata(L, 1, "Ideology");
            std::string member = luaL_checkstring(L, 2);
            if(member == "ref_name") {
                (*ideology)->ref_name = Eng3D::StringRef(luaL_checkstring(L, 3));
            } else if(member == "name") {
                (*ideology)->name = Eng3D::StringRef(luaL_checkstring(L, 3));
            }
            Eng3D::Log::debug("lua", "__newindex?");
            return 0;
        }},
        { NULL, NULL }
    };
    const luaL_Reg ideology_methods[] = {
        { "new", [](lua_State* L) {
            Ideology** ideology = (Ideology**)lua_newuserdata(L, sizeof(Ideology*));
            *ideology = new Ideology();
            luaL_setmetatable(L, "Ideology");

            (*ideology)->ref_name = Eng3D::StringRef(luaL_checkstring(L, 1));
            (*ideology)->name = Eng3D::StringRef(luaL_optstring(L, 2, (*ideology)->ref_name.c_str()));

            Eng3D::Log::debug("lua", "__new?");
            return 1;
        }},
        { "register", [](lua_State* L) {
            Ideology** ideology = (Ideology**)luaL_checkudata(L, 1, "Ideology");
            g_world->insert(**ideology);
            Eng3D::Log::debug("lua", "New ideology " + (*ideology)->ref_name);
            Eng3D::Log::debug("lua", "__register?");
            return 0;
        }},
        { "get", [](lua_State* L) {
            const std::string ref_name = Eng3D::StringRef(lua_tostring(L, 1)).get_string();
            auto result = std::find_if(g_world->ideologies.begin(), g_world->ideologies.end(),
            [&ref_name](const auto& o) { return (o.ref_name == ref_name); });
            if(result == g_world->ideologies.end())
                throw LuaAPI::Exception("Ideology " + ref_name + " not found");

            Ideology** ideology = (Ideology**)lua_newuserdata(L, sizeof(Ideology*));
            *ideology = &*result;
            luaL_setmetatable(L, "Ideology");

            Eng3D::Log::debug("lua", "__get?");
            return 1;
        }},
        { NULL, NULL }
    };
    //LuaAPI::register_new_table(lua, "Ideology", ideology_meta, ideology_methods);

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
    std::vector<std::string> mod_paths = Path::get_paths();
    for(const auto& path : mod_paths) {
        curr_path.append(";" + path + "lua/?.lua");
    }

    lua_pop(lua, 1);
    lua_pushstring(lua, curr_path.c_str());
    lua_setfield(lua, -2, "path");
    lua_pop(lua, 1);
}

World::~World() {
    lua_close(lua);
    for(auto& event : events) {
        delete event;
    } for(auto& province : provinces) {
        delete province;
    } for(auto& nation : nations) {
        delete nation;
    } for(auto& unit : units) {
        delete unit;
    } for(auto& war : wars) {
        delete war;
    }
}

static void lua_exec_all_of(World& world, const std::vector<std::string> files, const std::string& dir = "lua") {
    std::string files_buf = "require(\"classes/base\")\n\n";
    for(const auto& file : files) {
        std::vector<std::string> paths = Path::get_all(dir + "/" + file + ".lua");
        for(const auto& path : paths) {
            /*luaL_dofile(lua, path.c_str());

            if(luaL_dofile(lua, path.c_str()) != LUA_OK) {
                throw LuaAPI::Exception(lua_tostring(lua, -1));
            }*/
#ifdef E3D_TARGET_WINDOWS
            std::string m_path;
            for(auto& c : path) {
                if(c == '\\') {
                    m_path += "\\\\";
                } else {
                    m_path += c;
                }
            }
#else
            std::string m_path = path;
#endif
            files_buf += "print(\"" + m_path + "\")\n";
            files_buf += "f = loadfile(\"" + m_path + "\")\n";
            files_buf += "f()\n";
        }
    }
    Eng3D::Log::debug("game", "files_buf: " + files_buf);

    if(luaL_loadstring(world.lua, files_buf.c_str()) != LUA_OK || lua_pcall(world.lua, 0, 0, 0) != LUA_OK) {
        throw LuaAPI::Exception(lua_tostring(world.lua, -1));
    }
}

void World::load_initial(void) {
    // Execute all lua files
    lua_exec_all_of(*this, (std::vector<std::string>) {
        "terrain_types", "good_types", "ideologies", "cultures",
            "building_types", "technology", "religions", "pop_types",
            "industry_types", "unit_types", "boat_types",
            "nations", "provinces", "init"
    }, "lua/entities");
    // std::sort(this->provinces.begin(), this->provinces.end(), [](const auto& lhs, const auto& rhs) {
    //     return lhs->color > rhs->color;
    // });

    std::uint64_t checksum = 0;
    for(auto& province : provinces) {
        checksum += static_cast<std::uint64_t>(province->color);
    }
    checksum = (checksum * width + height) * provinces.size();

    bool recalc_province = true;
    try {
        Archive ar = Archive();
        ar.from_file("map.cache");

        // If the cache has the same checksum as us then we just have to read from the file
        std::uint64_t cache_checksum;
        ::deserialize(ar, &cache_checksum);
        if(checksum == cache_checksum) {
            recalc_province = false;

            ::deserialize(ar, &width);
            ::deserialize(ar, &height);
            tiles = std::make_unique<Tile[]>(width * height);
            for(auto& province : provinces) {
                ::deserialize(ar, &province->neighbours);
                ::deserialize(ar, &province->max_x);
                ::deserialize(ar, &province->max_y);
                ::deserialize(ar, &province->min_x);
                ::deserialize(ar, &province->min_y);
            }

            for(size_t i = 0; i < width * height; i++) {
                ::deserialize(ar, &tiles[i]);
            }
        }
    } catch(const std::exception& e) {
        Eng3D::Log::error("cache", e.what());
    }

    if(recalc_province) {
        std::unique_ptr<BinaryImage> div = std::make_unique<BinaryImage>(Path::get("map/provinces.png"));
        width = div->width;
        height = div->height;
        tiles = std::make_unique<Tile[]>(width * height);

        // Uncomment this and see a bit more below
        std::set<uint32_t> colors_found;
        Eng3D::Log::debug("game", Eng3D::Locale::translate("Associate tiles with provinces"));

        // Build a lookup table for super fast speed on finding provinces
        // 16777216 * 4 = c.a 64 MB, that quite a lot but we delete the table after anyways
        Eng3D::Log::debug("game", Eng3D::Locale::translate("Building the province lookup table"));
        std::vector<Province::Id> province_color_table(0xffffff + 1, Province::invalid());
        for(auto& province : provinces) {
            province_color_table[province->color & 0xffffff] = this->get_id(*province);
        }

        const uint32_t* raw_buffer = div->buffer.get();
        for(size_t i = 0; i < width * height; i++) {
            const Province::Id province_id = province_color_table[raw_buffer[i] & 0xffffff];
            if(province_id == (Province::Id)-1) {
                colors_found.insert(raw_buffer[i]);
            }
            tiles[i].province_id = province_id;
        }
        div.reset();

        if(!colors_found.empty()) {
            std::unique_ptr<FILE, int(*)(FILE*)> province_fp(fopen("uprovinces.lua", "w+t"), fclose);
            if(province_fp != nullptr) {
                for(const auto& color_raw : colors_found) {
                    uint32_t color = color_raw << 8;
                    fprintf(province_fp.get(), "province = Province:new{ ref_name = \"province_%06x\", color = 0x%06x }\n", static_cast<unsigned int>(bswap32(color)), static_cast<unsigned int>(bswap32(color)));
                    fprintf(province_fp.get(), "province.name = _(\"Province_%06x\")\n", static_cast<unsigned int>(bswap32(color)));
                    fprintf(province_fp.get(), "province:register()\n");
                }
            }

            std::unique_ptr<FILE, int(*)(FILE*)> color_fp(fopen("ucolors.txt", "w+t"), fclose);
            if(color_fp != nullptr) {
                for(size_t i = 0; i < province_color_table.size(); i++) {
                    uint32_t color = i << 8;

                        if(i % 128) {
                            continue;
                        }

                    if(Province::is_invalid(province_color_table[i])) {
                        fprintf(color_fp.get(), "%06lx\n", static_cast<unsigned long int>(bswap32(color)));
                    }
                }
            }

            // Exit
            CXX_THROW(std::runtime_error, "There are unregistered provinces, please register them!");
        }

        // Calculate the edges of the province (min and max x and y coordinates)
        Eng3D::Log::debug("game", Eng3D::Locale::translate("Calculate the edges of the province (min and max x and y coordinates)"));
        for(size_t j = 0; j < height; j++) {
            for(size_t i = 0; i < width; i++) {
                const Tile& tile = this->get_tile(i, j);
                auto& province = *provinces[tile.province_id];
                province.max_x = std::max(province.max_x, i);
                province.max_y = std::max(province.max_y, j);
                province.min_x = std::min(province.min_x, i);
                province.min_y = std::min(province.min_y, j);
            }
        }

        // Correct stuff from provinces
        Eng3D::Log::debug("game", Eng3D::Locale::translate("Correcting values for provinces"));
        for(auto& province : provinces) {
            province->max_x = std::min(width, province->max_x);
            province->max_y = std::min(height, province->max_y);
        }

        // Neighbours
        Eng3D::Log::debug("game", Eng3D::Locale::translate("Creating neighbours for provinces"));
        for(size_t i = 0; i < width * height; i++) {
            const Tile* tile = &this->tiles[i];
            if(tile->province_id < static_cast<Province::Id>(-3)) {
                Province* province = this->provinces[this->tiles[i].province_id];
                const std::vector<const Tile*> tiles = tile->get_neighbours(*this);
                for(const auto& other_tile : tiles) {
                    if(other_tile->province_id != tile->province_id && other_tile->province_id < (Province::Id)-3) {
                        province->neighbours.insert(this->provinces.at(other_tile->province_id));
                    }
                }
            }
        }

        // Write the cache file
        Archive ar = Archive();
        ::serialize(ar, &checksum);
        ::serialize(ar, &width);
        ::serialize(ar, &height);
        for(const auto& province : provinces) {
            ::serialize(ar, &province->neighbours);
            ::serialize(ar, &province->max_x);
            ::serialize(ar, &province->max_y);
            ::serialize(ar, &province->min_x);
            ::serialize(ar, &province->min_y);
        }

        for(size_t i = 0; i < width * height; i++) {
            ::serialize(ar, &tiles[i]);
        }
        ar.to_file("map.cache");
    }

    // Create diplomatic relations between nations
    Eng3D::Log::debug("game", Eng3D::Locale::translate("Creating diplomatic relations"));
    // Relations between nations start at 0 (and latter modified by lua scripts)
    // since we use cantor's pairing function we only have to make an n*2 array so yeah let's do that!
    this->relations = std::make_unique<NationRelation[]>(this->nations.size() * this->nations.size() * this->nations.size());

    Eng3D::Log::debug("game", Eng3D::Locale::translate("World partially intiialized"));
    // Auto-relocate capitals for countries which do not have one
    for(auto& nation : this->nations) {
        // Must exist
        if(!nation->exists()) {
            continue;
        }

        // Must not have already a capital set
        if(nation->capital != nullptr) {
            continue;
        }

        Eng3D::Log::debug("game", Eng3D::Locale::translate("Relocating capital of [" + nation->ref_name + "]"));
        nation->auto_relocate_capital();
    }
}

void World::load_mod(void) {
    const std::vector<std::string> mod_files = {
        "mod", "postinit"
    };
    lua_exec_all_of(*this, mod_files, "lua/init");

    // Server needs now to sync changes to clients (changing state is not enough)
    this->needs_to_sync = true;

    // Default init for policies
    for(auto& nation : this->nations) {
        nation->budget = 10000.f;

        Policies& policy = nation->current_policy;
        policy.import_tax = 0.1f;
        policy.export_tax = 0.1f;
        policy.domestic_export_tax = 0.1f;
        policy.domestic_import_tax = 0.1f;
        policy.med_flat_tax = 0.1f;
        policy.poor_flat_tax = 0.1f;
        policy.rich_flat_tax = 0.1f;
        policy.private_property = true;
        policy.immigration = ALLOW_ALL;
        policy.migration = ALLOW_ALL;
        policy.industry_tax = 0.1f;
        policy.foreign_trade = true;
        policy.min_wage = 1.f;
        policy.min_sv_for_parliament = 2.f;
    }
    Eng3D::Log::debug("game", Eng3D::Locale::translate("World fully intiialized"));

    // Initialize the AI
    ai_init(*this);
}

static inline void unit_do_tick(Unit& unit, const std::vector<std::vector<Battle*>>& battles_per_province)
{
    assert(unit.province != nullptr);

    // Do not evaluate if we have an ongoing battle
    if(unit.on_battle) {
        unit.target = nullptr;
        return;
    }

    if(unit.target != nullptr) {
        bool can_move = true, can_take = false;
        if(unit.target->controller != unit.owner) {
            const auto& relation = g_world->get_relation(g_world->get_id(*unit.target->controller), g_world->get_id(*unit.owner));
            can_move = (relation.has_alliance || relation.has_defensive_pact || relation.has_war);
            can_take = (relation.has_war);
        }

        if(can_move) {
            if(unit.move_progress) {
                unit.move_progress -= std::min<Eng3D::Decimal>(unit.move_progress, unit.get_speed());
            } else {
                unit.set_province(*unit.target);
                // Only take control of provinces of the people we're at war with
                if(can_take) {
                    unit.owner->control_province(*unit.target);
                }
            }
        } else {
            unit.target = nullptr;
        }
    }
    
    // If there is another unit of a country we are at war with we will start a battle
    for(auto& war : g_world->wars) {
        if(war->is_involved(*unit.owner)) {
            auto it = std::find_if(war->battles.begin(), war->battles.end(), [&unit](const auto& e) {
                return e.province == unit.province;
            });

            // Create a new battle if none is occurring on this province
            unit.target = nullptr;
            if(it == war->battles.end()) {
                // See above code, by our logic the other unit should already be in a battle if it's
                // against us, and if it is not, and it's probably wise to attack them
                for(auto& other_unit : unit.province->get_units()) {
                    if(other_unit->owner == unit.owner || other_unit->on_battle) {
                        continue;
                    }

                    // Check relations, if we're at war we will attack this unit
                    const auto& relation = g_world->get_relation(g_world->get_id(*unit.owner), g_world->get_id(*other_unit->owner));
                    if(!relation.has_war) {
                        continue;
                    }

                    // If we found an unit we can attack, start a battle
                    unit.on_battle = true;
                    other_unit->on_battle = true;
                    
                    Battle battle = Battle(*war, *unit.province);
                    battle.name = "Battle of " + unit.province->name;
                    if(war->is_attacker(*unit.owner)) {
                        battle.attackers.push_back(&unit);
                        battle.defenders.push_back(other_unit);
                    } else {
                        battle.attackers.push_back(other_unit);
                        battle.defenders.push_back(&unit);
                    }
                    war->battles.push_back(battle);
                    Eng3D::Log::debug("game", "New battle of \"" + battle.name + "\"");
                    break;
                }
            } else {
                Battle& battle = *it;

                // Add the unit to one side depending on who are we attacking
                // However unit must not be already involved
                /// @todo Make it be instead depending on who attacked first in this battle
                if(war->is_attacker(*unit.owner)) {
                    assert(std::find(battle.attackers.begin(), battle.attackers.end(), &unit) == battle.attackers.end());
                    unit.on_battle = true;
                    battle.attackers.push_back(&unit);
                    Eng3D::Log::debug("game", "Adding unit <attacker> to battle of \"" + battle.name + "\"");
                } else {
                    assert(war->is_defender(*unit.owner));
                    assert(std::find(battle.defenders.begin(), battle.defenders.end(), &unit) == battle.defenders.end());
                    unit.on_battle = true;
                    battle.defenders.push_back(&unit);
                    Eng3D::Log::debug("game", "Adding unit <defender> to battle of \"" + battle.name + "\"");
                }
                break;
            }
        }
    }
}

#include <tbb/blocked_range.h>
#include <tbb/combinable.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>

void World::do_tick() {
    profiler.start("AI");
    // Do the AI turns in parallel
    tbb::parallel_for(tbb::blocked_range(nations.begin(), nations.end()), [this](auto& nations_range) {
        for(const auto& nation : nations_range) {
            if(!nation->exists()) {
                return;
            }

            // Diplomatic cooldown
            if(nation->diplomatic_timer != 0) {
                nation->diplomatic_timer--;
            }

            // Research stuff
            Eng3D::Decimal research = nation->get_research_points();
            if(nation->focus_tech != nullptr) {
                Eng3D::Decimal* research_progress = &nation->research[get_id(*nation->focus_tech)];
                *research_progress -= std::min(research, *research_progress);
                if(!(*research_progress)) {
                    // Give the country the modifiers attached to the technology
                    for(auto& mod : nation->focus_tech->modifiers) {
                        nation->modifiers.push_back(mod);
                    }
                    nation->focus_tech = nullptr;
                }
            }
            ai_do_tick(*nation);
        }
    });
    profiler.stop("AI");

    profiler.start("Economy");
    // Every ticks_per_month ticks do an economical tick
    // if(!(time % ticks_per_month)) {
    Economy::do_tick(*this);
    profiler.start("E-packages");
        // g_server->broadcast(Action::NationUpdate::form_packet(nations));
        // g_server->broadcast(Action::ProvinceUpdate::form_packet(provinces));
    profiler.stop("E-packages");
    // }
    profiler.stop("Economy");

    profiler.start("Research");
    std::vector<Eng3D::Decimal> mil_research_pts(nations.size(), 0.f);
    std::vector<Eng3D::Decimal> naval_research_pts(nations.size(), 0.f);
    // Now researches for every country are going to be accounted :)
    for(const auto& nation : nations) {
        assert(nation != nullptr);
        for(const auto& technology : technologies) {
            if(!nation->can_research(technology)) {
                continue;
            }

            Eng3D::Decimal* research_progress = &nation->research[get_id(technology)];
            if(!(*research_progress)) {
                continue;
            }

            Eng3D::Decimal* pts_count;
            if(technology.type == TechnologyType::MILITARY) {
                pts_count = &mil_research_pts[get_id(*nation)];
            } else if(technology.type == TechnologyType::NAVY) {
                pts_count = &naval_research_pts[get_id(*nation)];
            } else {
                continue;
            }

            if(*pts_count <= 0.f) {
                continue;
            }

            const Eng3D::Decimal pts = *pts_count / 4.f;
            *research_progress += pts;
            *pts_count -= pts;
            break;
        }
    }
    profiler.stop("Research");

    profiler.start("Treaties");
    // Do the treaties clauses
    for(const auto& treaty : treaties) {
        assert(treaty != nullptr);

        // Check that the treaty is agreed by all parties before enforcing it
        bool on_effect = !(std::find_if(treaty->approval_status.begin(), treaty->approval_status.end(), [](auto& status) { return (status.second != TreatyApproval::ACCEPTED); }) != treaty->approval_status.end());
        if(!on_effect) {
            continue;
        }

        // Check with treaty
        if(!treaty->in_effect()) {
            continue;
        }

        // Treaties clauses now will be enforced
        Eng3D::Log::debug("game", "Enforcing treaty " + treaty->name);
        for(auto& clause : treaty->clauses) {
            assert(clause != nullptr);
            if(clause->type == TreatyClauseType::MONEY) {
                auto dyn_clause = static_cast<TreatyClause::WarReparations*>(clause);
                if(!dyn_clause->in_effect()) {
                    continue;
                }
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::ANNEX_PROVINCES) {
                auto dyn_clause = static_cast<TreatyClause::AnnexProvince*>(clause);
                if(!dyn_clause->in_effect()) {
                    continue;
                }
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::LIBERATE_NATION) {
                auto dyn_clause = static_cast<TreatyClause::LiberateNation*>(clause);
                if(!dyn_clause->in_effect()) {
                    continue;
                }
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::HUMILIATE) {
                auto dyn_clause = static_cast<TreatyClause::Humiliate*>(clause);
                if(!dyn_clause->in_effect()) {
                    continue;
                }
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::IMPOSE_POLICIES) {
                auto dyn_clause = static_cast<TreatyClause::ImposePolicies*>(clause);
                if(!dyn_clause->in_effect()) {
                    continue;
                }
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::CEASEFIRE) {
                auto dyn_clause = static_cast<TreatyClause::Ceasefire*>(clause);
                if(!dyn_clause->in_effect()) {
                    continue;
                }
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::PUPPET) {
                auto dyn_clause = static_cast<TreatyClause::Puppet*>(clause);
                if(!dyn_clause->in_effect()) {
                    continue;
                }
                dyn_clause->enforce();
            }

            // Rebellion
            if(clause->sender->ref_name == clause->receiver->ref_name) {
                if(clause->sender->owned_provinces.empty()) {
                    /// @todo Delete clause->sender (and fixup references) !!!
                } else if(clause->receiver->owned_provinces.empty()) {
                    /// @todo Delete clause->receiver (and fixup references) !!!
                }
            }
        }
    }
    profiler.stop("Treaties");

    wcmap_mutex.lock();
    profiler.start("Units");
    // Build the "list of battles per province"
    std::vector<std::vector<Battle*>> battles_per_province(this->provinces.size());
    for(auto& war : this->wars) {
        for(auto& battle : war->battles) {
            battles_per_province[this->get_id(*battle.province)].push_back(&battle);
        }
    }
    // Evaluate units
    for(const auto& unit : this->units) {
        unit_do_tick(*unit, battles_per_province);
    }
    profiler.stop("Units");

    // Perform all battles of the active wars
    profiler.start("Battles");
    tbb::combinable<tbb::concurrent_vector<Unit*>> clear_units;
    tbb::parallel_for(tbb::blocked_range(wars.begin(), wars.end()), [this, &clear_units](auto& wars_range) {
        for(const auto& war : wars_range) {
            assert(!war->attackers.empty() && !war->defenders.empty());
            for(size_t j = 0; j < war->battles.size(); j++) {
                auto& battle = war->battles[j];
                assert(battle.province != nullptr);

                // Attackers attack Defenders
                for(auto& attacker : battle.attackers) {
                    assert(attacker != nullptr);
                    for(size_t i = 0; i < battle.defenders.size(); ) {
                        Unit* unit = battle.defenders[i];
                        assert(unit != nullptr);

                        const size_t prev_size = unit->size;
                        attacker->attack(*unit);
                        battle.defender_casualties += prev_size - unit->size;
                        if(!unit->size) {
                            Eng3D::Log::debug("game", "Removing attacker \"" + unit->type->ref_name + "\" unit to battle of \"" + battle.name + "\"");
                            battle.defenders.erase(battle.defenders.begin() + i);
                            assert(unit->province == battle.province);
                            clear_units.local().push_back(unit);
                            continue;
                        }
                        i++;
                    }
                }

                // Defenders attack attackers
                for(auto& defender : battle.defenders) {
                    assert(defender != nullptr);
                    for(size_t i = 0; i < battle.attackers.size(); ) {
                        Unit* unit = battle.attackers[i];
                        assert(unit != nullptr);

                        const size_t prev_size = unit->size;
                        defender->attack(*unit);
                        battle.attacker_casualties += prev_size - unit->size;
                        if(!unit->size) {
                            Eng3D::Log::debug("game", "Removing defender \"" + unit->type->ref_name + "\" unit to battle of \"" + battle.name + "\"");
                            battle.attackers.erase(battle.attackers.begin() + i);
                            assert(unit->province == battle.province);
                            clear_units.local().push_back(unit);
                            continue;
                        }
                        i++;
                    }
                }

                // Once one side has fallen this battle has ended
                if(battle.defenders.empty() || battle.attackers.empty()) {
                    // Defenders defeated
                    if(battle.defenders.empty()) {
                        battle.attackers[0]->owner->control_province(*battle.province);
                        // Clear flags of all units
                        for(auto& unit : battle.attackers) {
                            unit->on_battle = false;
                        }
                        Eng3D::Log::debug("game", "Battle \"" + battle.name + "\": attackers win");
                    }
                    // Defenders won
                    else if(battle.attackers.empty()) {
                        battle.defenders[0]->owner->control_province(*battle.province);
                        for(auto& unit : battle.defenders) {
                            unit->on_battle = false;
                        }
                        Eng3D::Log::debug("game", "Battle \"" + battle.name + "\": defenders win");
                    }
                    // Nobody won?
                    else {
                        Eng3D::Log::debug("game", "Battle \"" + battle.name + "\": nobody won");
                    }

                    war->battles.erase(war->battles.begin() + j);
                    j--;
                    continue;
                }
            }
        }
    });
    profiler.stop("Battles");

    profiler.start("Cleaning");
    clear_units.combine_each([this](const auto& units_clear_list) {
        for(auto& unit : units_clear_list) {
            this->remove(*unit);
            delete unit;
        }
    });
    profiler.stop("Cleaning");
    wcmap_mutex.unlock();

    profiler.start("Events");
    LuaAPI::check_events(lua);
    profiler.stop("Events");

    profiler.start("Send packets");
    g_server->broadcast(Action::UnitUpdate::form_packet(units));
    profiler.stop("Send packets");

    if(!(time % ticks_per_month)) {
        Eng3D::Log::debug("game", std::to_string(time / 12 / ticks_per_month) + "/" + std::to_string((time / ticks_per_month % 12) + 1) + +"/" + std::to_string((time % ticks_per_month) + 1));
    }

    Eng3D::Log::debug("game", "Tick " + std::to_string(time) + " done");
    time++;

    // Tell clients that this tick has been done
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::WORLD_TICK;
    ::serialize(ar, &action);
    packet.data(ar.get_buffer(), ar.size());
    g_server->broadcast(packet);
}
