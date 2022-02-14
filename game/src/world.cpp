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
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif

#include "io_impl.hpp"
#include "province.hpp"
#include "world.hpp"
#include "diplomacy.hpp"
#include "policy.hpp"
#include "server/lua_api.hpp"
#include "server/server_network.hpp"
#include "action.hpp"
#include "server/economy.hpp"

#include "unified_render/binary_image.hpp"
#include "unified_render/path.hpp"
#include "unified_render/log.hpp"
#include "unified_render/serializer.hpp"
#include "unified_render/state.hpp"
#include "unified_render/byteswap.hpp"
#include "unified_render/locale.hpp"

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
    const size_t idx = world.get_id(this);

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

// Obtains a tile from the world safely, and makes sure that it is in bounds
Tile& World::get_tile(size_t x, size_t y) const {
    if(x >= width || y >= height) {
        throw std::runtime_error("Tile out of bounds");
    }
    return tiles[x + y * width];
}

Tile& World::get_tile(size_t idx) const {
    if(idx >= width * height) {
        throw std::runtime_error("Tile index exceeds boundaries");
    }
    return tiles[idx];
}

void ai_do_tick(Nation* nation, World* world);

// Creates a new world
World::World() {
    g_world = this;

    lua = luaL_newstate();
    luaL_openlibs(lua);

    // Register our API functions
    lua_register(lua, "add_terrain_type", LuaAPI::add_terrain_type);
    lua_register(lua, "get_terrain_type", LuaAPI::get_terrain_type);

    lua_register(lua, "add_technology", LuaAPI::add_technology);
    lua_register(lua, "get_technology", LuaAPI::get_technology);
    lua_register(lua, "add_req_tech_to_tech", LuaAPI::add_req_tech_to_tech);

    lua_register(lua, "set_nation_mod_to_invention", LuaAPI::set_nation_mod_to_invention);

    lua_register(lua, "add_unit_trait", LuaAPI::add_unit_trait);

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

    lua_register(lua, "get_friends_of_nation", LuaAPI::get_friends_of_nation);
    lua_register(lua, "get_enemies_of_nation", LuaAPI::get_enemies_of_nation);
    lua_register(lua, "get_allies_of_nation", LuaAPI::get_allies_of_nation);
    lua_register(lua, "get_warenemies_of_nation", LuaAPI::get_warenemies_of_nation);
    lua_register(lua, "get_embargoed_of_nation", LuaAPI::get_embargoed_of_nation);

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

    lua_register(lua, "add_nation_mod", LuaAPI::add_nation_mod);
    lua_register(lua, "get_nation_mod", LuaAPI::get_nation_mod);

    lua_register(lua, "add_province", LuaAPI::add_province);
    lua_register(lua, "get_province", LuaAPI::get_province);
    lua_register(lua, "get_province_by_id", LuaAPI::get_province_by_id);
    lua_register(lua, "add_province_industry", LuaAPI::add_province_industry);
    lua_register(lua, "add_province_pop", LuaAPI::add_province_pop);
    lua_register(lua, "set_province_terrain", LuaAPI::set_province_terrain);
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

    lua_register(lua, "add_descision", LuaAPI::add_descision);

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

    const struct luaL_Reg ideology_meta[] ={
        { "__gc", [](lua_State* L) {
            UnifiedRender::Log::debug("lua", "__gc?");
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
            UnifiedRender::Log::debug("lua", "__index?");
            return 1;
        }},
        { "__newindex", [](lua_State* L) {
            Ideology** ideology = (Ideology**)luaL_checkudata(L, 1, "Ideology");
            std::string member = luaL_checkstring(L, 2);
            if(member == "ref_name") {
                (*ideology)->ref_name = luaL_checkstring(L, 3);
            } else if(member == "name") {
                (*ideology)->name = luaL_checkstring(L, 3);
            }
            UnifiedRender::Log::debug("lua", "__newindex?");
            return 0;
        }},
        { NULL, NULL }
    };
    const luaL_Reg ideology_methods[] ={
        { "new", [](lua_State* L) {
            Ideology** ideology = (Ideology**)lua_newuserdata(L, sizeof(Ideology*));
            *ideology = new Ideology();
            luaL_setmetatable(L, "Ideology");

            (*ideology)->ref_name = luaL_checkstring(L, 1);
            (*ideology)->name = luaL_optstring(L, 2, (*ideology)->ref_name.c_str());

            UnifiedRender::Log::debug("lua", "__new?");
            return 1;
        }},
        { "register", [](lua_State* L) {
            Ideology** ideology = (Ideology**)luaL_checkudata(L, 1, "Ideology");
            g_world->insert(*ideology);
            UnifiedRender::Log::debug("lua", "New ideology " + (*ideology)->ref_name);
            UnifiedRender::Log::debug("lua", "__register?");
            return 0;
        }},
        { "get", [](lua_State* L) {
            const std::string ref_name = lua_tostring(L, 1);
            auto result = std::find_if(g_world->ideologies.begin(), g_world->ideologies.end(),
            [&ref_name](const auto& o) { return (o->ref_name == ref_name); });
            if(result == g_world->ideologies.end())
                throw LuaAPI::Exception("Ideology " + ref_name + " not found");

            Ideology** ideology = (Ideology**)lua_newuserdata(L, sizeof(Ideology*));
            *ideology = *result;
            luaL_setmetatable(L, "Ideology");

            UnifiedRender::Log::debug("lua", "__get?");
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
    delete[] tiles;

    for(auto& religion : religions) {
        delete religion;
    } for(auto& unit_type : unit_types) {
        delete unit_type;
    } for(auto& event : events) {
        delete event;
    } for(auto& pop_type : pop_types) {
        delete pop_type;
    } for(auto& culture : cultures) {
        delete culture;
    } for(auto& good : goods) {
        delete good;
    } for(auto& province : provinces) {
        delete province;
    } for(auto& nation : nations) {
        delete nation;
    } for(auto& building_type : building_types) {
        delete building_type;
    } for(auto& unit_trait : unit_traits) {
        delete unit_trait;
    } for(auto& unit : units) {
        delete unit;
    } for(auto& ideology : ideologies) {
        delete ideology;
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
#ifdef windows
            std::string m_path;
            for(auto& c : path) {
                if(c == '\\') {
                    m_path += "\\\\";
                }
                else {
                    m_path += c;
                }
            }
#else
            std::string m_path = path;
#endif
            files_buf += "print(\"" + m_path + "\")";
            files_buf += "f = loadfile(\"" + m_path + "\")\n";
            files_buf += "f()\n";
        }
    }
    UnifiedRender::Log::debug("game", "files_buf: " + files_buf);

    if(luaL_loadstring(world.lua, files_buf.c_str()) != LUA_OK || lua_pcall(world.lua, 0, 0, 0) != LUA_OK) {
        throw LuaAPI::Exception(lua_tostring(world.lua, -1));
    }
}

void World::load_initial(void) {
    const std::vector<std::string> init_files = {
        "terrain_types",
        "ideologies", "cultures", "nations",  "unit_traits", "building_types",
        "technology", "religions", "pop_types", "good_types", "industry_types",
        "unit_types", "boat_types", "provinces", "init"
    };
    lua_exec_all_of(*this, init_files, "lua/entities");

    // Shrink normally-not-resized vectors to give back memory to the OS
    UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("Shrink normally-not-resized vectors to give back memory to the OS"));
    provinces.shrink_to_fit();
    nations.shrink_to_fit();
    goods.shrink_to_fit();
    unit_types.shrink_to_fit();
    cultures.shrink_to_fit();
    religions.shrink_to_fit();
    pop_types.shrink_to_fit();
    ideologies.shrink_to_fit();
    building_types.shrink_to_fit();

    // Associate tiles with province
    std::unique_ptr<BinaryImage> div = std::unique_ptr<BinaryImage>(new BinaryImage(Path::get("map/provinces.png")));
    width = div->width;
    height = div->height;

    // Sea is	<= sea_level
    // Rivers	== sea_level + 1
    // Land is	> sea_level + 1
    const size_t total_size = width * height;
    sea_level = 126;
    tiles = new Tile[total_size];
    if(tiles == nullptr) {
        throw std::runtime_error("Out of memory");
    }

    if(div->width != width || div->height != height) {
        throw std::runtime_error("Province map size mismatch");
    }

    // Build a lookup table for super fast speed on finding provinces
    // 16777216 * 4 = c.a 64 MB, that quite a lot but we delete the table after anyways
    UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("Building the province lookup table"));
    std::vector<Province::Id> province_color_table(16777216, (Province::Id)-1);
    for(auto& province : provinces) {
        province_color_table[province->color & 0xffffff] = get_id(province);
    }

    /*if(1) {
        for(auto& province : provinces) {
            province->n_tiles = 0;
        }
        
        for(unsigned int i = 0; i < total_size; ) {
            const Province::Id province_id = province_color_table[div->buffer.get()[i] & 0xffffff];
            if(province_id >= provinces.size()) {
                i++;
                continue;
            }

            while(div->buffer.get()[i] == provinces[province_id]->color) {
                provinces[province_id]->n_tiles++;
                i++;
            }
        }

        for(unsigned int i = 0; i < provinces.size(); i++) {
            auto& province = *provinces[i];
            if(!province.n_tiles) {
                remove(&province);

                for(auto& nation : nations) {
                    nation->owned_provinces.erase(&province);
                    if(nation->capital == &province) {
                        nation->capital = nullptr;
                    }
                }

                for(size_t j = 0; j < province.get_buildings().size(); j++) {
                    auto& building = province.get_buildings()[j];

                    if(building.province != &province) {
                        continue;
                    }

                    /*remove(&building);
                    delete &building;
                    j--;/
                }

                delete &province;
                i--;
                continue;
            }
        }

        std::fill(province_color_table.begin(), province_color_table.end(), (Province::Id)-1);
        for(auto& province : provinces) {
            province_color_table[province->color & 0xffffff] = get_id(province);
        }
    }*/

    uint32_t checksum = 1;
    for(auto& province : provinces) {
        checksum += province->ref_name.at(0);
    }
    checksum *= width * height * provinces.size();
    bool recalc_province = true;
    try {
        Archive ar = Archive();
        ar.from_file("province_cache.dat");

        // If the cache has the same checksum as us then we just have to read from the file
        uint32_t cache_checksum;
        ::deserialize(ar, &cache_checksum);
        if(checksum == cache_checksum) {
            recalc_province = false;
            for(auto& province : provinces) {
                ::deserialize(ar, &province->n_tiles);
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
        UnifiedRender::Log::error("cache", e.what());
    }

    if(recalc_province) {
        // Uncomment this and see a bit more below
        std::set<uint32_t> colors_found;
        UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("Associate tiles with provinces"));
        for(size_t i = 0; i < total_size; ) {
            const Province::Id province_id = province_color_table[div->buffer.get()[i] & 0xffffff];
            if(province_id == (Province::Id)-1) {
                // Uncomment this and see below
                colors_found.insert(div->buffer.get()[i]);
                i++;
                continue;
            }

            const uint32_t rel_color = provinces[province_id]->color;
            while(div->buffer.get()[i] == rel_color) {
                tiles[i].province_id = province_id;
                provinces[province_id]->n_tiles++;
                i++;
            }
        }
        div.reset(nullptr);

        /* Uncomment this for auto-generating lua code for unregistered provinces */
        FILE* fp = fopen("unknown.lua", "w+t");
        if(fp) {
            for(const auto& color_raw : colors_found) {
                uint32_t color = color_raw << 8;
                fprintf(fp, "province = Province:new{ ref_name = \"province_%06x\", color = 0x%06x }\n", static_cast<uintptr_t>(bswap32(color)), static_cast<uintptr_t>(bswap32(color)));
                fprintf(fp, "province.name = _(\"Province_%06x\")\n", static_cast<uintptr_t>(bswap32(color)));
                fprintf(fp, "province:register()\n");
            }
            fclose(fp);
        }

        fp = fopen("unused.txt", "w+t");
        if(fp) {
            for(int i = 0; i < province_color_table.size(); i++) {
                uint32_t color = i << 8;
                if(province_color_table[i] == (Province::Id)-1) {
                    fprintf(fp, "%06x\n", static_cast<uintptr_t>(bswap32(color)));
                }
            }
            fclose(fp);
        }

        // Calculate the edges of the province (min and max x and y coordinates)
        UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("Calculate the edges of the province (min and max x and y coordinates)"));
        for(size_t j = 0; j < height; j++) {
            for(size_t i = 0; i < width; i++) {
                Tile& tile = get_tile(i, j);
                if(tile.province_id >= provinces.size()) {
                    tile.province_id = static_cast<Province::Id>(1);
                    continue;
                }

                auto& province = *provinces[tile.province_id];
                province.max_x = std::max(province.max_x, i);
                province.max_y = std::max(province.max_y, j);
                province.min_x = std::min(province.min_x, i);
                province.min_y = std::min(province.min_y, j);
            }
        }

        // Correct stuff from provinces
        UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("Correcting values for provinces"));
        for(auto& province : provinces) {
            province->max_x = std::min(width, province->max_x);
            province->max_y = std::min(height, province->max_y);
        }

        // Neighbours
        UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("Creating neighbours for provinces"));
        for(size_t i = 0; i < total_size; i++) {
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

        Archive ar = Archive();
        ::serialize(ar, &checksum);
        for(const auto& province : provinces) {
            ::serialize(ar, &province->n_tiles);
            ::serialize(ar, &province->neighbours);
            ::serialize(ar, &province->max_x);
            ::serialize(ar, &province->max_y);
            ::serialize(ar, &province->min_x);
            ::serialize(ar, &province->min_y);
        }

        for(size_t i = 0; i < width * height; i++) {
            ::serialize(ar, &tiles[i]);
        }
        ar.to_file("province_cache.dat");
    }

    // Create diplomatic relations between nations
    UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("Creating diplomatic relations"));
    for(const auto& nation : this->nations) {
        // Relations between nations start at 0 (and latter modified by lua scripts)
        nation->relations.resize(this->nations.size(), NationRelation{ 0.f, false, false, false, false, false, false, false, false, true, false });
    }
    UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("World partially intiialized"));

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

        UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("Relocating capital of [" + nation->ref_name + "]"));
        nation->auto_relocate_capital();
    }
}

void World::load_mod(void) {
    const std::vector<std::string> mod_files ={
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
    }
    UnifiedRender::Log::debug("game", UnifiedRender::Locale::translate("World fully intiialized"));
}

void World::do_tick() {
    std::scoped_lock lock(world_mutex);

    profiler.start("AI");
    // AI and stuff
    // Just random shit to make the world be like more alive
    std::for_each(nations.begin(), nations.end(), [this](auto& nation) {
        if(nation->exists()) {
            // Diplomatic cooldown
            if(nation->diplomatic_timer != 0) {
                nation->diplomatic_timer--;
            }

            // Research stuff
            UnifiedRender::Decimal research = nation->get_research_points();
            if(nation->focus_tech != nullptr) {
                UnifiedRender::Decimal* research_progress = &nation->research[get_id(nation->focus_tech)];
                *research_progress -= std::min(research, *research_progress);
                if(!(*research_progress)) {
                    // Give the country the modifiers attached to the technology
                    for(auto& mod : nation->focus_tech->modifiers) {
                        nation->modifiers.push_back(mod);
                    }
                    nation->focus_tech = nullptr;
                }
            }
            ai_do_tick(nation, this);
        }
    });
    profiler.stop("AI");

    profiler.start("Economy");
    // Every ticks_per_month ticks do an economical tick
    if(!(time % ticks_per_month)) {
        Economy::do_tick(*this);

        // Calculate prestige for today (newspapers come out!)
        for(auto& nation : this->nations) {
            const UnifiedRender::Decimal decay_per_cent = 5.f;
            const UnifiedRender::Decimal max_modifier = 10.f;
            const UnifiedRender::Decimal min_prestige = std::max<UnifiedRender::Decimal>(0.5f, ((nation->naval_score + nation->military_score + nation->economy_score) / 2));

            // Prestige cannot go below min prestige
            nation->prestige = std::max<UnifiedRender::Decimal>(nation->prestige, min_prestige);
            nation->prestige -= (nation->prestige * (decay_per_cent / 100.f)) * std::min<UnifiedRender::Decimal>(std::max<UnifiedRender::Decimal>(1, nation->prestige - min_prestige) / (min_prestige + 1), max_modifier);

            float economy_score = 0.f;
            for(const auto& province : nation->owned_provinces) {
                // Calculate economy score of nations
                for(const auto& pop : province->pops) {
                    economy_score += pop.budget;
                }
            }
            nation->economy_score = economy_score / 100.f;
        }
        g_server->broadcast(Action::NationUpdate::form_packet(nations));
        g_server->broadcast(Action::ProvinceUpdate::form_packet(provinces));
    }
    profiler.stop("Economy");

    profiler.start("Units");
    // Evaluate units
    std::vector<UnifiedRender::Decimal> mil_research_pts(nations.size(), 0.f);
    std::vector<UnifiedRender::Decimal> naval_research_pts(nations.size(), 0.f);
    for(size_t i = 0; i < units.size(); i++) {
        Unit* unit = units[i];
        if(unit->on_battle) {
            continue;
        }

        bool can_take = true;
        for(auto& other_unit : unit->province->get_units()) {
            if(other_unit->on_battle) {
                continue;
            }

            // Must not our unit and only if we are at war
            if(other_unit->owner == unit->owner || !unit->owner->relations[get_id(other_unit->owner)].has_war) {
                continue;
            }

            can_take = false;

            // If there is another unit of a country we are at war with we will start a battle
            for(auto& war : wars) {
                if(!war->is_involved(*unit->owner)) {
                    continue;
                }
                
                auto it = std::find_if(war->battles.begin(), war->battles.end(), [&unit](const auto& e) {
                    return &e.province == unit->province;
                });
                
                // Create a new battle if none is occurring on this province
                unit->target = nullptr;
                if(it == war->battles.end()) {
                    Battle battle = Battle(*war, *unit->province);
                    battle.name = "Battle of " + unit->province->name;
                    if(war->is_attacker(*unit->owner)) {
                        battle.attackers.push_back(unit);
                        battle.defenders.push_back(other_unit);
                    } else {
                        battle.attackers.push_back(other_unit);
                        battle.defenders.push_back(unit);
                    }
                    unit->on_battle = true;
                    other_unit->on_battle = true;
                    war->battles.push_back(battle);
                    UnifiedRender::Log::debug("game", "New battle of \"" + battle.name + "\"");
                    break;
                } else {
                    Battle& battle = *it;

                    // Add the unit to one side depending on who are we attacking
                    // However unit must not be already involved
                    // TODO: Make it be instead depending on who attacked first in this battle
                    if(war->is_attacker(*unit->owner)) {
                        if(std::find(battle.attackers.begin(), battle.attackers.end(), unit) == battle.attackers.end()) {
                            battle.attackers.push_back(unit);
                            unit->on_battle = true;
                        }
                    } else if(war->is_defender(*unit->owner)) {
                        if(std::find(battle.defenders.begin(), battle.defenders.end(), unit) == battle.defenders.end()) {
                            battle.defenders.push_back(unit);
                            unit->on_battle = true;
                        }
                    }
                    UnifiedRender::Log::debug("game", "Adding unit to battle of \"" + battle.name + "\"");
                    break;
                }
                break;
            }
        }

        if(unit->province->controller != nullptr && can_take) {
            unit->owner->give_province(*unit->province);
        }

        if(unit->target != nullptr && unit->can_move()) {
            if(unit->move_progress) {
                unit->move_progress -= std::min<UnifiedRender::Decimal>(unit->move_progress, unit->get_speed());
            } else {
                unit->set_province(*unit->target);
            }
        }
    }
    profiler.stop("Units");

    // Perform all battles of the active wars
    profiler.start("Battles");
    std::for_each(wars.begin(), wars.end(), [this](auto& war) {
        for(size_t j = 0; j < war->battles.size(); j++) {
            Battle& battle = war->battles[j];

            // Battles are stored for historic purpouses
            if(battle.ended) {
                continue;
            }

            // Attackers attack Defenders
            for(auto& attacker : battle.attackers) {
                for(size_t i = 0; i < battle.defenders.size(); ) {
                    Unit* unit = battle.defenders[i];
                    const size_t prev_size = unit->size;
                    attacker->attack(*unit);
                    battle.defender_casualties += prev_size - unit->size;
                    if(!unit->size) {
                        UnifiedRender::Log::debug("game", "Removing attacker \"" + unit->type->ref_name + "\" unit to battle of \"" + battle.name + "\"");
                        battle.defenders.erase(battle.defenders.begin() + i);
                        if(unit->province != nullptr) {
                            auto it = std::find(unit->province->units.begin(), unit->province->units.end(), unit);
                            unit->province->units.erase(it);
                        }
                        remove(unit);
                        delete unit;
                        continue;
                    }
                    i++;
                }
            }

            // Defenders attack Attackers
            for(auto& defender : battle.defenders) {
                for(size_t i = 0; i < battle.attackers.size(); ) {
                    Unit* unit = battle.attackers[i];
                    const size_t prev_size = unit->size;
                    defender->attack(*unit);
                    battle.attacker_casualties += prev_size - unit->size;
                    if(!unit->size) {
                        UnifiedRender::Log::debug("game", "Removing defender \"" + unit->type->ref_name + "\" unit to battle of \"" + battle.name + "\"");
                        battle.attackers.erase(battle.attackers.begin() + i);
                        if(unit->province != nullptr) {
                            auto it = std::find(unit->province->units.begin(), unit->province->units.end(), unit);
                            unit->province->units.erase(it);
                        }
                        remove(unit);
                        delete unit;
                        continue;
                    }
                    i++;
                }
            }

            // Once one side has fallen this battle has ended
            if(battle.defenders.empty() || battle.attackers.empty()) {
                battle.ended = true;
                continue;
            }
        }
    });
    profiler.stop("Battles");

    profiler.start("Research");
    // Now researches for every country are going to be accounted :)
    for(const auto& nation : nations) {
        for(const auto& tech : technologies) {
            if(!nation->can_research(tech)) {
                continue;
            }

            UnifiedRender::Decimal* research_progress = &nation->research[get_id(tech)];
            if(!(*research_progress)) {
                continue;
            }

            UnifiedRender::Decimal* pts_count;
            if(tech->type == TechnologyType::MILITARY) {
                pts_count = &mil_research_pts[get_id(nation)];
            } else if(tech->type == TechnologyType::NAVY) {
                pts_count = &naval_research_pts[get_id(nation)];
            } else {
                continue;
            }

            if(*pts_count <= 0.f) {
                continue;
            }
            
            const UnifiedRender::Decimal pts = *pts_count / 4.f;
            *research_progress += pts;
            *pts_count -= pts;
            break;
        }
    }
    profiler.stop("Research");

    profiler.start("Send packets");
    {
        // Broadcast to clients
        UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
        Archive ar = Archive();
        ActionType action = ActionType::UNIT_UPDATE;
        ::serialize(ar, &action);
        Unit::Id size = units.size();
        ::serialize(ar, &size);
        for(const auto& unit : units) {
            ::serialize(ar, &unit);
            ::serialize(ar, unit);
        }
        packet.data(ar.get_buffer(), ar.size());
        g_server->broadcast(packet);
    }
    profiler.stop("Send packets");

    profiler.start("Treaties");
    // Do the treaties clauses
    for(const auto& treaty : treaties) {
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
        UnifiedRender::Log::debug("game", "Enforcing treaty " + treaty->name);
        for(auto& clause : treaty->clauses) {
            if(clause->type == TreatyClauseType::WAR_REPARATIONS) {
                auto dyn_clause = static_cast<TreatyClause::WarReparations*>(clause);
                if(!dyn_clause->in_effect()) {
                    continue;
                }
                dyn_clause->enforce();
            } else if(clause->type == TreatyClauseType::ANEXX_PROVINCES) {
                auto dyn_clause = static_cast<TreatyClause::AnexxProvince*>(clause);
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
            }

            // Rebellion
            if(clause->sender->ref_name == clause->receiver->ref_name) {
                if(clause->sender->owned_provinces.empty()) {
                    // TODO: Delete clause->sender (and fixup references) !!!
                } else if(clause->receiver->owned_provinces.empty()) {
                    // TODO: Delete clause->receiver (and fixup references) !!!
                }
            }
        }
    }
    profiler.stop("Treaties");

    profiler.start("Events");
    LuaAPI::check_events(lua);
    profiler.stop("Events");

    if(!(time % ticks_per_month)) {
        UnifiedRender::Log::debug("game", std::to_string(time / 12 / ticks_per_month) + "/" + std::to_string((time / ticks_per_month % 12) + 1) + + "/" + std::to_string((time % ticks_per_month) + 1));
    }

    UnifiedRender::Log::debug("game", "Tick " + std::to_string(time) + " done");
    time++;

    // Tell clients that this tick has been done
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::WORLD_TICK;
    ::serialize(ar, &action);
    packet.data(ar.get_buffer(), ar.size());
    g_server->broadcast(packet);
}
