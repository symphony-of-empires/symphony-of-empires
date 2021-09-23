#include "../diplomacy.hpp"
#include "../policy.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif
#include <libintl.h>
#include <locale.h>

#include "../province.hpp"
#include "economy.hpp"
#include "../world.hpp"
#include "../binary_image.hpp"
#include "lua_api.hpp"
#include "../path.hpp"
#include "../print.hpp"
#include "../serializer.hpp"
#include "../io_impl.hpp"
#include "server_network.hpp"

#if (__cplusplus < 201703L)
namespace std {
    template<typename T>
    constexpr inline T clamp(T n, T min, T max) {
        return std::min<T>(max, std::max<T>(n, min));
    }
}
#endif

/**
 * Creates a new world
  */
World::World() {
    g_world = this;

    lua = luaL_newstate();
    luaL_openlibs(lua);

    // Register our API functions
    lua_register(lua, "_", LuaAPI::get_text);

    lua_register(lua, "add_technology", LuaAPI::add_technology);
    lua_register(lua, "get_technology", LuaAPI::get_technology);
    lua_register(lua, "add_req_tech_to_tech", LuaAPI::add_req_tech_to_tech);

    lua_register(lua, "add_unit_trait", LuaAPI::add_unit_trait);

    lua_register(lua, "add_building_type", LuaAPI::add_building_type);

    lua_register(lua, "add_good", LuaAPI::add_good);
    lua_register(lua, "get_good", LuaAPI::get_good);

    lua_register(lua, "add_industry_type", LuaAPI::add_industry_type);
    lua_register(lua, "get_industry_type", LuaAPI::get_industry_type);
    lua_register(lua, "add_input_to_industry_type", LuaAPI::add_input_to_industry_type);
    lua_register(lua, "add_output_to_industry_type", LuaAPI::add_output_to_industry_type);
    lua_register(lua, "add_req_good_to_industry_type", LuaAPI::add_req_good_to_industry_type);

    lua_register(lua, "add_nation", LuaAPI::add_nation);
    lua_register(lua, "get_nation", LuaAPI::get_nation);
    lua_register(lua, "get_provinces_owned_by_nation", LuaAPI::get_provinces_owned_by_nation);
    lua_register(lua, "get_provinces_with_nucleus_by_nation", LuaAPI::get_provinces_with_nucleus_by_nation);
    lua_register(lua, "set_nation_primary_culture", LuaAPI::set_nation_primary_culture);
    lua_register(lua, "set_nation_capital", LuaAPI::set_nation_capital);
    lua_register(lua, "add_nation_accepted_culture", LuaAPI::add_accepted_culture);
    lua_register(lua, "add_nation_client_hint", LuaAPI::add_nation_client_hint);
    lua_register(lua, "get_nation_policies", LuaAPI::get_nation_policies);
    lua_register(lua, "set_nation_policies", LuaAPI::set_nation_policies);
    lua_register(lua, "set_nation_ideology", LuaAPI::set_nation_ideology);
    
    lua_register(lua, "add_province", LuaAPI::add_province);
    lua_register(lua, "get_province", LuaAPI::get_province);
    lua_register(lua, "get_province_by_id", LuaAPI::get_province_by_id);
    lua_register(lua, "add_province_industry", LuaAPI::add_province_industry);
    lua_register(lua, "add_province_pop", LuaAPI::add_province_pop);
    lua_register(lua, "give_province_to", LuaAPI::give_province_to);
    lua_register(lua, "get_province_owner", LuaAPI::get_province_owner);
    lua_register(lua, "get_province_controller", LuaAPI::get_province_controller);
    lua_register(lua, "get_province_neighbours", LuaAPI::get_province_neighbours);
    lua_register(lua, "multiply_province_militancy_global", LuaAPI::multiply_province_militancy_global);
    lua_register(lua, "multiply_province_militancy_by_culture", LuaAPI::multiply_province_militancy_by_culture);
    lua_register(lua, "multiply_province_militancy_by_religion", LuaAPI::multiply_province_militancy_by_religion);
    lua_register(lua, "multiply_province_con_global", LuaAPI::multiply_province_con_global);
    lua_register(lua, "multiply_province_con_by_culture", LuaAPI::multiply_province_con_by_culture);
    lua_register(lua, "multiply_province_con_by_religion", LuaAPI::multiply_province_con_by_religion);
    lua_register(lua, "rename_province", LuaAPI::rename_province);
    lua_register(lua, "add_province_nucleus", LuaAPI::add_province_nucleus);
    lua_register(lua, "add_province_owner", LuaAPI::add_province_owner);

    lua_register(lua, "add_company", LuaAPI::add_company);

    lua_register(lua, "add_event", LuaAPI::add_event);
    lua_register(lua, "get_event", LuaAPI::get_event);
    lua_register(lua, "add_event_receivers", LuaAPI::add_event_receivers);

    lua_register(lua, "add_descision", LuaAPI::add_descision);

    lua_register(lua, "add_pop_type", LuaAPI::add_pop_type);
    lua_register(lua, "get_pop_type", LuaAPI::get_pop_type);

    lua_register(lua, "add_culture", LuaAPI::add_culture);
    lua_register(lua, "get_culture", LuaAPI::get_culture);

    lua_register(lua, "add_religion", LuaAPI::add_religion);
    lua_register(lua, "get_religion", LuaAPI::get_religion);

    lua_register(lua, "add_unit_type", LuaAPI::add_unit_type);
    lua_register(lua, "get_unit_type", LuaAPI::get_unit_type);
    lua_register(lua, "add_req_good_unit_type", LuaAPI::add_req_good_unit_type);

    lua_register(lua, "add_boat_type", LuaAPI::add_boat_type);
    lua_register(lua, "get_boat_type", LuaAPI::get_boat_type);
    lua_register(lua, "add_req_good_boat_type", LuaAPI::add_req_good_boat_type);

    lua_register(lua, "add_ideology", LuaAPI::add_ideology);
    lua_register(lua, "get_ideology", LuaAPI::get_ideology);

    lua_register(lua, "get_hour", LuaAPI::get_hour);
    lua_register(lua, "get_day", LuaAPI::get_day);
    lua_register(lua, "get_month", LuaAPI::get_month);
    lua_register(lua, "get_year", LuaAPI::get_year);

    /*
    const struct luaL_Reg ideology_meta[] = {
        { "__gc", [](lua_State* L) {
            return 0;
        }},
        { "__index", [](lua_State* L) {
            print_info("__index?");
            return 0;
        }},
        { "__newindex", [](lua_State* L) {
            Ideology* ideology = (Ideology*)luaL_checkudata(L, 1, "Ideology");
            std::string member = luaL_checkstring(L, 2);
            if(member == "ref_name") {
                ideology->ref_name = luaL_checkstring(L, 3);
            } else if(member == "name") {
                ideology->name = luaL_checkstring(L, 3);
            }
            print_info("__newindex?");
            return 0;
        }},
        { NULL, NULL }
    };
    const luaL_Reg ideology_methods[] = {
        { "new", [](lua_State* L) {
            Ideology* ideology = (Ideology*)lua_newuserdata(L, sizeof(Ideology));
            ideology->ref_name = luaL_checkstring(L, 1);
            ideology->name = luaL_checkstring(L, 2);
            return 1;
        }},
        { "register", [](lua_State* L) {
            Ideology* ideology = (Ideology*)luaL_checkudata(L, 1, "Ideology");
            Ideology* new_ideology = new Ideology(*ideology);
            g_world->ideologies.push_back(new_ideology);
            return 0;
        }},
        { "get", [](lua_State* L) {
            std::string ref_name = lua_tostring(L, 2);
            auto result = std::find_if(g_world->ideologies.begin(), g_world->ideologies.end(),
            [&ref_name](const auto& o) { return (o->ref_name == ref_name); });
            if(result == g_world->ideologies.end())
                throw LuaAPI::Exception("Ideology " + ref_name + " not found");
            
            Ideology* ideology = (Ideology*)luaL_checkudata(L, 1, "Ideology");
            *ideology = **result;
            return 0;
        }},
        { NULL, NULL }
    };
    LuaAPI::register_new_table(lua, "Ideology", ideology_meta, ideology_methods);
    */

    // Constants for ease of readability
    lua_pushboolean(lua, true);
    lua_setglobal(lua, "EVENT_CONDITIONS_MET");
    lua_pushboolean(lua, false);
    lua_setglobal(lua, "EVENT_CONDITIONS_UNMET");

    lua_pushboolean(lua, true);
    lua_setglobal(lua, "EVENT_DO_MANY_TIMES");
    lua_pushboolean(lua, false);
    lua_setglobal(lua, "EVENT_DO_ONE_TIME");

    // TODO: The. name. is. fucking. long.
    lua_register(lua, "add_op_province_to_company", LuaAPI::add_op_province_to_company);

    // Set path for `require` statements in lua, this will allow us to require
    // without using data/scripts
    lua_getglobal(lua, "package");
    lua_getfield(lua, -1, "path");
    std::string curr_path = lua_tostring(lua, -1);

    // Add all scripts onto the path (with glob operator '?')
    curr_path.append(";");
    std::string path = Path::get_dir("scripts");
    curr_path.append(path + "/?.lua");
    lua_pop(lua, 1);
    lua_pushstring(lua, curr_path.c_str());
    lua_setfield(lua, -2, "path");
    lua_pop(lua, 1);
}

World::~World() {
    lua_close(lua);
    delete[] tiles;

    for(auto& religion: religions) {
        delete religion;
    } for(auto& unit_type: unit_types) {
        delete unit_type;
    } for(auto& event: events) {
        delete event;
    } for(auto& company: companies) {
        delete company;
    } for(auto& pop_type: pop_types) {
        delete pop_type;
    } for(auto& culture: cultures) {
        delete culture;
    } for(auto& good: goods) {
        delete good;
    } for(auto& province: provinces) {
        delete province;
    } for(auto& nation: nations) {
        delete nation;
    } for(auto& building_type: building_types) {
        delete building_type;
    } for(auto& unit_trait: unit_traits) {
        delete unit_trait;
    } for(auto& boat_type: boat_types) {
        delete boat_type;
    } for(auto& product: products) {
        delete product;
    } for(auto& boat: boats) {
        delete boat;
    } for(auto& unit: units) {
        delete unit;
    } for(auto& ideology: ideologies) {
        delete ideology;
    }
}

void World::load_mod(void) {
    BinaryImage topo(Path::get("map_topo.png"));
    BinaryImage div(Path::get("map_div.png"));
    BinaryImage infra(Path::get("map_infra.png"));

    width = topo.width;
    height = topo.height;

    // Check that size of all maps match
    if(topo.width != width || topo.height != height) {
        throw std::runtime_error("Topographic map size mismatch");
    } else if(div.width != width || div.height != height) {
        throw std::runtime_error("Province map size mismatch");
    }

    const size_t total_size = width * height;

    // Sea is	<= sea_level
    // Rivers	== sea_level + 1
    // Land is	> sea_level + 1
    sea_level = 126;
    tiles = new Tile[total_size];
    if(tiles == nullptr) {
        throw std::runtime_error("Out of memory");
    }
    
    int ret;
    
    std::string final_buf = "require('api')\n";
    std::vector<std::string> files_text;

    files_text = Path::get_data("scripts/unit_traits.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/building_types.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/cultures.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/ideology.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/technology.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/religions.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/pop_types.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/good_types.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/industry_types.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/unit_types.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/boat_types.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/nations.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/companies.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/provinces.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }
    files_text = Path::get_data("scripts/init.lua");
    for(const auto& text: files_text) { final_buf += text; final_buf += "\n"; }

    if(luaL_loadstring(lua, final_buf.c_str()) != LUA_OK
    || lua_pcall(lua, 0, 0, 0) != LUA_OK) {
        throw LuaAPI::Exception(lua_tostring(lua, -1));
    }

    // Shrink normally-not-resized vectors to give back memory to the OS
    print_info(gettext("Shrink normally-not-resized vectors to give back memory to the OS"));
    provinces.shrink_to_fit();
    nations.shrink_to_fit();
    goods.shrink_to_fit();
    unit_types.shrink_to_fit();
    cultures.shrink_to_fit();
    religions.shrink_to_fit();
    pop_types.shrink_to_fit();

    for(auto& province: provinces) {
        province->max_x = 0;
        province->max_y = 0;
        province->min_x = UINT32_MAX;
        province->min_y = UINT32_MAX;
    }

    // Translate all div, pol and topo maps onto this single tile array
    print_info(gettext("Translate all div, pol and topo maps onto this single tile array"));
    for(size_t i = 0; i < total_size; i++) {
        // Set coordinates for the tiles
        tiles[i].owner_id = (Nation::Id)-1;
        tiles[i].province_id = (Province::Id)-1;
        tiles[i].elevation = topo.buffer[i] & 0x000000ff;
        if(topo.buffer[i] == 0xffff0000) {
            tiles[i].elevation = sea_level + 1;
        }

        // Set infrastructure level
        if(infra.buffer[i] == 0xffffffff || infra.buffer[i] == 0xff000000) {
            tiles[i].infra_level = 0;
        } else {
            tiles[i].infra_level = 1;
        }
    }

    // Associate tiles with provinces

    // Build a lookup table for super fast speed on finding provinces
    // 16777216 * 4 = c.a 64 MB, that quite a lot but we delete the table after anyways
    print_info(gettext("Building the province lookup table"));
    Province::Id* color_province_rel_table = new Province::Id[16777216];
    memset(color_province_rel_table, 0xff, sizeof(Province::Id) * 16777216);
    for(const auto& province: provinces) {
        color_province_rel_table[province->color & 0xffffff] = get_id(province);
    }

    // Uncomment this and see a bit more below
    //std::set<uint32_t> colors_found;
    print_info(gettext("Associate tiles with provinces"));
    for(size_t i = 0; i < total_size; i++) {
        const uint32_t color = div.buffer[i];

        // This "skip the empty stuff" technique works!
        while(i < total_size && (div.buffer[i] == 0xffffffff || div.buffer[i] == 0xff000000)) {
            ++i;
        }

        if(!(i < total_size))
            break;

        const Province::Id province_id = color_province_rel_table[div.buffer[i] & 0xffffff];
        if(province_id == (Province::Id)-1) {
            // Uncomment this and see below
            //colors_found.insert(color);
            continue;
        }

        const uint32_t rel_color = provinces[province_id]->color;
        while(div.buffer[i] == rel_color) {
            tiles[i].province_id = province_id;
            provinces[province_id]->n_tiles++;
            ++i;
        }
        --i;
    }
    delete[] color_province_rel_table;

    /* Uncomment this for auto-generating lua code for unregistered provinces */
    /*for(const auto& color_raw: colors_found) {
        uint32_t color = color_raw << 8;

        printf("province = Province:create{ ref_name = \"province_%x\", color = 0x%06x }\n", __bswap_32(color), __bswap_32(color));
        printf("province.name = _(\"Province_%x\")\n", __bswap_32(color));
        printf("Province:register(province)\n");
        printf("Province:add_pop(province, artisan, arabic, islamic, 1000, 0.5)\n");
        printf("Province:add_pop(province, farmer, arabic, islamic, 1000, 0.5)\n");
        printf("Province:add_pop(province, soldier, arabic, islamic, 1000, 0.5)\n");
        printf("Province:add_pop(province, craftsmen, arabic, islamic, 1000, 0.5)\n");
        printf("Province:add_pop(province, bureaucrat, arabic, islamic, 1000, 0.5)\n");
        printf("Province:add_pop(province, aristocrat, arabic, islamic, 1000, 0.5)\n");
        printf("Province:add_pop(province, clergymen, arabic, islamic, 1000, 0.5)\n");
        printf("Province:add_pop(province, laborer, arabic, islamic, 1000, 0.5)\n");
        printf("Province:add_pop(province, entrepreneur, arabic, islamic, 1000, 0.5)\n");
    }*/

    // Calculate the edges of the province (min and max x and y coordinates)
    print_info(gettext("Calculate the edges of the province (min and max x and y coordinates)"));
    for(size_t j = 0; j < height; j++) {
        for(size_t i = 0; i < width; i++) {
            Tile& tile = get_tile(i, j);
            if(tile.province_id == (Province::Id)-1)
                continue;

            Province* province = provinces[tile.province_id];
            province->max_x = std::max(province->max_x, i);
            province->max_y = std::max(province->max_y, j);
            province->min_x = std::min(province->min_x, i);
            province->min_y = std::min(province->min_y, j);
        }
    }

    // Correct stuff from provinces
    print_info(gettext("Correcting values for provinces"));
    for(auto& province: provinces) {
        province->max_x = std::min(width, province->max_x);
        province->max_y = std::min(height, province->max_y);

        // Add stockpile
        for(size_t i = 0; i < products.size(); i++) {
            province->stockpile.push_back(0);
        }
    }

    // Give owners the entire provinces
    print_info(gettext("Give owners the entire provinces"));
    for(auto& nation: nations) {
        for(auto& province: nation->owned_provinces) {
            const Province::Id province_id = get_id(province);
            for(size_t x = province->min_x; x <= province->max_x; x++) {
                for(size_t y = province->min_y; y <= province->max_y; y++) {
                    Tile& tile = get_tile(x, y);

                    if(tile.province_id != province_id)
                        continue;

                    const Nation::Id nation_id = get_id(province->owner);
                    tile.owner_id = nation_id;
                }
            }
        }
        nation_changed_tiles.clear();
    }

    // Neighbours
    print_info(gettext("Creating neighbours for provinces"));
    for(size_t i = 0; i < total_size; i++) {
        const Tile* tile = &this->tiles[i];
        const Tile* other_tile;
        if(tile->owner_id != (Nation::Id)-1) {
            Nation* nation = this->nations[this->tiles[i].owner_id];

            // Up neighbour
            if(i > this->width) {
                other_tile = &this->tiles[i - this->width];
                if(other_tile->owner_id != tile->owner_id
                && other_tile->owner_id != (Nation::Id)-1) {
                    nation->neighbours.insert(this->nations[other_tile->owner_id]);
                }
            }
            // Down neighbour
            if(i < (this->width* this->height) - this->width) {
                other_tile = &this->tiles[i + this->width];
                if(other_tile->owner_id != tile->owner_id
                && other_tile->owner_id != (Nation::Id)-1) {
                    nation->neighbours.insert(this->nations[other_tile->owner_id]);
                }
            }
            // Left neighbour
            if(i > 1) {
                other_tile = &this->tiles[i - 1];
                if(other_tile->owner_id != tile->owner_id
                && other_tile->owner_id != (Nation::Id)-1) {
                    nation->neighbours.insert(this->nations[other_tile->owner_id]);
                }
            }
            // Right neighbour
            if(i < (this->width* this->height) - 1) {
                other_tile = &this->tiles[i + 1];
                if(other_tile->owner_id != tile->owner_id
                && other_tile->owner_id != (Nation::Id)-1) {
                    nation->neighbours.insert(this->nations[other_tile->owner_id]);
                }
            }
        }
        
        if(tile->province_id != (Province::Id)-1) {
            Province* province = this->provinces[this->tiles[i].province_id];

            // Up neighbour
            if(i > this->width) {
                other_tile = &this->tiles[i - this->width];
                if(other_tile->province_id != tile->province_id
                && other_tile->province_id != (Province::Id)-1) {
                    province->neighbours.insert(this->provinces[other_tile->province_id]);
                }
            }
            // Down neighbour
            if(i < (this->width* this->height) - this->width) {
                other_tile = &this->tiles[i + this->width];
                if(other_tile->province_id != tile->province_id
                && other_tile->province_id != (Province::Id)-1) {
                    province->neighbours.insert(this->provinces[other_tile->province_id]);
                }
            }
            // Left neighbour
            if(i > 1) {
                other_tile = &this->tiles[i - 1];
                if(other_tile->province_id != tile->province_id
                && other_tile->province_id != (Province::Id)-1) {
                    province->neighbours.insert(this->provinces[other_tile->province_id]);
                }
            }
            // Right neighbour
            if(i < (this->width* this->height) - 1) {
                other_tile = &this->tiles[i + 1];
                if(other_tile->province_id != tile->province_id
                && other_tile->province_id != (Province::Id)-1) {
                    province->neighbours.insert(this->provinces[other_tile->province_id]);
                }
            }
        }
    }

    // Create diplomatic relations between nations
    print_info(gettext("Creating diplomatic relations"));
    for(const auto& nation: this->nations) {
        // Relations between nations start at 0 (and latter modified by lua scripts)
        for(size_t i = 0; i < this->nations.size(); i++) {
            nation->relations.push_back(NationRelation{0.f, false, false, false, false, false, false, false, false, true, false});
        }
    }

    final_buf = "require('api')\n\n";
    files_text = Path::get_data("scripts/mod.lua");
    for(const auto& text: files_text) { final_buf += text; }
    
    if(luaL_loadstring(lua, final_buf.c_str()) != LUA_OK
    || lua_pcall(lua, 0, 0, 0) != LUA_OK) {
        throw LuaAPI::Exception(lua_tostring(lua, -1));
    }
    
    // Default init for policies
    for(auto& nation: this->nations) {
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
    print_info(gettext("World fully intiialized"));
}

#include "../actions.hpp"
#include "economy.hpp"
void World::do_tick() {
    std::lock_guard<std::recursive_mutex> lock(world_mutex);
    std::lock_guard<std::recursive_mutex> lock2(tiles_mutex);

    // AI and stuff
    // Just random shit to make the world be like more alive
    for(const auto& nation: nations) {
        if(nation->exists() == false)
            continue;
        
        if(rand() % 1000 > 990) {
            Province *target = provinces[rand() % provinces.size()];
            if(target->owner == nullptr) {
                Packet packet = Packet();
                Archive ar = Archive();
                ActionType action = ActionType::PROVINCE_COLONIZE;
                ::serialize(ar, &action);
                ::serialize(ar, &target);
                ::serialize(ar, target);
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);

                nation->give_province(*this, *target);
                print_info("Conquering %s for %s", target->name.c_str(), nation->name.c_str());
            }
        }

        if(rand() % 100 > 98.f) {
            Nation *target = nullptr;
            while(target == nullptr || target->exists() == false) {
                target = nations[rand() % nations.size()];
            }
            nation->increase_relation(*target);
        } else if(rand() % 100 > 98.f) {
            Nation *target = nullptr;
            while(target == nullptr || target->exists() == false) {
                target = nations[rand() % nations.size()];
            }
            nation->decrease_relation(*target);
        }

        // Rarely nations will change policies
        if(rand() % 100 > 50) {
            Policies new_policy = nation->current_policy;

            if(rand() % 100 > 50.f) {
                new_policy.import_tax += 0.1f * (rand() % 10);
            } else if(rand() % 100 > 50.f) {
                new_policy.import_tax -= 0.1f * (rand() % 10);
            }

            if(rand() % 100 > 50.f) {
                new_policy.export_tax += 0.1f * (rand() % 10);
            } else if(rand() % 100 > 50.f) {
                new_policy.export_tax -= 0.1f * (rand() % 10);
            }

            if(rand() % 100 > 50.f) {
                new_policy.domestic_export_tax += 0.1f * (rand() % 10);
            } else if(rand() % 100 > 50.f) {
                new_policy.domestic_export_tax -= 0.1f * (rand() % 10);
            }

            if(rand() % 100 > 50.f) {
                new_policy.domestic_import_tax += 0.1f * (rand() % 10);
            } else if(rand() % 100 > 50.f) {
                new_policy.domestic_import_tax -= 0.1f * (rand() % 10);
            }

            if(rand() % 100 > 50.f) {
                new_policy.industry_tax += 0.1f * (rand() % 10);
            } else if(rand() % 100 > 50.f) {
                new_policy.industry_tax -= 0.1f * (rand() % 10);
            }

            nation->set_policy(new_policy);
        }

        if(nation->diplomatic_timer != 0) {
            nation->diplomatic_timer--;
        }

        // Accepting/rejecting treaties
        if(std::rand() % 1000 > 10) {
            for(auto& treaty: treaties) {
                for(auto& part: treaty->approval_status) {
                    if(part.first == nation) {
                        if(part.second == TreatyApproval::TREATY_APPROVAL_ACCEPTED
                        || part.second == TreatyApproval::TREATY_APPROVAL_DENIED) {
                            break;
                        }

                        if(std::rand() % 50 >= 25) {
                            print_info("We, %s, deny the treaty of %s", treaty->name.c_str());
                            part.second = TreatyApproval::TREATY_APPROVAL_DENIED;
                        } else {
                            print_info("We, %s, accept the treaty of %s", treaty->name.c_str());
                            part.second = TreatyApproval::TREATY_APPROVAL_ACCEPTED;
                        }
                    }
                }
            }
        }

        // Build an building randomly?
        if(std::rand() % 1000 > 950) {
            bool can_build = false;
            for(const auto& province: nation->owned_provinces) {
                if(get_id(&province->get_occupation_controller(*this)) != g_world->get_id(nation)) {
                    can_build = true;
                    break;
                }
            }

            if(!can_build) {
                continue;
            }

            // Select random province
            Province *target = nullptr;
            while(target == nullptr || get_id(&target->get_occupation_controller(*this)) != g_world->get_id(nation)) {
                auto it = std::begin(nation->owned_provinces);
                std::advance(it, std::rand() % nation->owned_provinces.size());
                target = *it;
            }

            Tile *tile = nullptr;
            int x_coord, y_coord;
            while(tile == nullptr) {
                x_coord = std::clamp<size_t>((std::rand() % (target->max_x - target->min_x)) + target->min_x, target->min_x, target->max_x);
                y_coord = std::clamp<size_t>((std::rand() % (target->max_y - target->min_y)) + target->min_y, target->min_y, target->max_y);
                tile = &get_tile(x_coord, y_coord);

                // If tile is land AND NOT part of target province OR NOT of ownership of nation
                if(tile->elevation > sea_level
                && (tile->province_id != get_id(target) || tile->owner_id != get_id(nation))) {
                    tile = nullptr;
                }
            }

            // Now build the building
            Building* building = new Building();
            building->x = x_coord;
            building->y = y_coord;
            building->owner = nation;
            building->working_unit_type = nullptr;
            building->working_boat_type = nullptr;
            building->req_goods_for_unit = std::vector<std::pair<Good*, size_t>>();
            building->req_goods = std::vector<std::pair<Good*, size_t>>();
            building->type = building_types.at(std::rand() % building_types.size());
            if(building->type->is_factory == true) {
                building->budget = 100.f;
                building->corporate_owner = companies.at(std::rand() % companies.size());
                building->corporate_owner->operating_provinces.insert(building->get_province(*this));
                
                // Add a product for each output
                for(const auto& output: building->type->outputs) {
                    Product* new_product = new Product();
                    new_product->building = building;
                    new_product->good = output;
                    new_product->owner = building->corporate_owner;
                    new_product->origin = building->get_province(*this);

                    building->output_products.push_back(new_product);
                    products.push_back(new_product);

                    building->employees_needed_per_output.push_back(500);

                    // Add an element representing this product on all the province's stockpile
                    for(auto& province: provinces) {
                        province->stockpile.push_back(0);
                    }
                }

                // We will set inputs_satisfied to same size as inputs
                // Industries start with 100 of stockpiles
                building->stockpile.insert(building->stockpile.begin(), building->type->inputs.size(), 100);
            }
            g_world->buildings.push_back(building);

            // Broadcast the addition of the building to the clients
            {
                Packet packet = Packet();
                Archive ar = Archive();
                ActionType action = ActionType::BUILDING_ADD;
                ::serialize(ar, &action);
                ::serialize(ar, building);
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
            }
            print_info("Building of %s(%i), from %s built on %s", building->type->name.c_str(), (int)get_id(building->type), nation->name.c_str(), target->name.c_str());
        }
    }

    // Each tick == 30 minutes
    switch(time % (24 * 2)) {
    // 3:00
    case 6:
        Economy::do_phase_1(*this);
        break;
    // 7:30
    // Busy hour, newspapers come out and people get mad
    case 15:
        Economy::do_phase_2(*this);

        // Calculate prestige for today (newspapers come out!)
        for(auto& nation: this->nations) {
            const float decay_per_cent = 5.f;
            const float max_modifier = 10.f;
            const float min_prestige = std::max<float>(0.5f, ((nation->naval_score + nation->military_score + nation->economy_score) / 2));

            // Prestige cannot go below min prestige
            nation->prestige = std::max<float>(nation->prestige, min_prestige);
            nation->prestige -= (nation->prestige* (decay_per_cent / 100.f))* fmin(fmax(1, nation->prestige - min_prestige) / min_prestige, max_modifier);
        }
        break;
    // 12:00
    case 24:
        Economy::do_phase_3(*this);
        for(const auto& product: g_world->products) {
            // Broadcast to clients
            Packet packet = Packet();
            Archive ar = Archive();
            ActionType action = ActionType::PRODUCT_UPDATE;
            ::serialize(ar, &action);
            ::serialize(ar, &product); // ProductRef
            ::serialize(ar, product); // ProductObj
            packet.data(ar.get_buffer(), ar.size());
            g_server->broadcast(packet);
        }

        for(auto& nation: this->nations) {
            float economy_score = 0.f;
            for(const auto& province: nation->owned_provinces) {
                // Calculate economy score of nations
                for(const auto& pop: province->pops) {
                    economy_score += pop.budget;
                }
                
                // Also calculates GDP
                for(const auto& product: g_world->products) {
                    nation->gdp += product->price* province->stockpile[g_world->get_id(product)];
                }
            }
            nation->economy_score = economy_score / 100.f;
        }
        break;
    // 18:00
    case 36:
        Economy::do_phase_4(*this);
        break;
    // 24:00, this is where clients are sent all information **at once**
    case 47:
        {
            for(const auto& nation: g_world->nations) {
                // Broadcast to clients
                Packet packet = Packet();
                Archive ar = Archive();
                ActionType action = ActionType::NATION_UPDATE;
                ::serialize(ar, &action);
                ::serialize(ar, &nation); // NationRef
                ::serialize(ar, nation); // NationObj
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
            }

            for(const auto& province: g_world->provinces) {
                // Broadcast to clients
                Packet packet = Packet();
                Archive ar = Archive();
                ActionType action = ActionType::PROVINCE_UPDATE;
                ::serialize(ar, &action);
                ::serialize(ar, &province); // ProvinceRef
                ::serialize(ar, province); // ProvinceObj
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
            }
        }
        break;
    default:
        break;
    }

    // Evaluate boats
    for(size_t i = 0; i < boats.size(); i++) {
        Boat* unit = boats[i];
        if(unit->size <= 0) {
            g_world->boats.erase(boats.begin() + i);
            break;
        }
        
        // Count friends and foes in range (and find nearest foe)
        size_t n_friends = 0;
        size_t n_foes = 0;
        Boat* nearest_foe = nullptr;
        Boat* nearest_friend = nullptr;
        for(size_t j = 0; j < g_world->boats.size(); j++) {
            Boat* other_unit = g_world->boats[j];
            if(unit->owner == other_unit->owner) {
                // Only when very close
                if(std::abs(unit->x - other_unit->x) >= 4.f && std::abs(unit->y - other_unit->y) >= 4.f)
                    continue;
                
                n_friends++;
                
                if(nearest_friend == nullptr) {
                    nearest_friend = other_unit;
                }
                
                // Find nearest friend
                if(std::abs(unit->x - other_unit->x) < std::abs(unit->x - nearest_friend->x)
                && std::abs(unit->y - other_unit->y) < std::abs(unit->y - nearest_friend->y)) {
                    nearest_friend = other_unit;
                }
            } else {
                // Foes from many ranges counts
                if(std::abs(unit->x - other_unit->x) >= 1.f && std::abs(unit->y - other_unit->y) >= 1.f)
                    continue;
                
                n_foes++;
                
                if(nearest_foe == nullptr) {
                    nearest_foe = other_unit;
                }

                // Find nearest foe
                if(std::abs(unit->x - other_unit->x) < std::abs(unit->x - nearest_foe->x)
                && std::abs(unit->y - other_unit->y) < std::abs(unit->y - nearest_foe->y)) {
                    nearest_foe = other_unit;
                }
            }
        }

        // This code stops the "wiggly" movement due to floating point differences
        if((unit->x != unit->tx || unit->y != unit->ty)
        && (std::abs(unit->x - unit->tx) >= 0.2f || std::abs(unit->y - unit->ty) >= 0.2f)) {
            float end_x, end_y;
            const float speed = 0.1f;

            end_x = unit->x;
            end_y = unit->y;
            
            // Move towards target
            if(unit->x > unit->tx)
                end_x -= speed;
            else if(unit->x < unit->tx)
                end_x += speed;

            if(unit->y > unit->ty)
                end_y -= speed;
            else if(unit->y < unit->ty)
                end_y += speed;
            
            // Boats cannot go on land
            if(get_tile(end_x, end_y).elevation > sea_level) {
                continue;
            }

            unit->x = end_x;
            unit->y = end_y;
        }
        
        // Make the unit attack automatically
        // and we must be at war with the owner of this unit to be able to attack the unit
        if(nearest_foe != nullptr
        && unit->owner->relations[get_id(nearest_foe->owner)].has_war == false) {
            unit->attack(*nearest_foe);
        }

        // North and south do not wrap
        unit->y = std::max<float>(0.f, unit->y);
        unit->y = std::min<float>(height, unit->y);

        // West and east do wrap
        if(unit->x <= 0.f) {
            unit->x = width - 1.f;
        } else if(unit->x >= width) {
            unit->x = 0.f;
        }

        // Set nearby tiles as owned
        // TODO: Make it conquer multiple tiles
        Tile& tile = get_tile(unit->x, unit->y);
        if(tile.owner_id != get_id(unit->owner)) {
            tile.owner_id = get_id(unit->owner);

            std::lock_guard<std::recursive_mutex> lock(nation_changed_tiles_mutex);
            nation_changed_tiles.push_back(&get_tile(unit->x, unit->y));
        }
    }
    
    for(const auto& boat: g_world->boats) {
        // Broadcast to clients
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::BOAT_UPDATE;
        ::serialize(ar, &action);
        ::serialize(ar, &boat);
        ::serialize(ar, boat);
        packet.data(ar.get_buffer(), ar.size());
        g_server->broadcast(packet);
    }

    // Evaluate units
    for(size_t i = 0; i < units.size(); i++) {
        Unit* unit = units[i];
        if(unit->size <= 0) {
            g_world->units.erase(units.begin() + i);
            break;
        }
        
        // Count friends and foes in range (and find nearest foe)
        size_t n_friends = 0;
        size_t n_foes = 0;
        Unit* nearest_foe = nullptr;
        Unit* nearest_friend = nullptr;
        for(size_t j = 0; j < g_world->units.size(); j++) {
            Unit* other_unit = g_world->units[j];
            if(unit->owner == other_unit->owner) {
                // Only when very close
                if(std::abs(unit->x - other_unit->x) >= 4.f && std::abs(unit->y - other_unit->y) >= 4.f)
                    continue;
                
                n_friends++;
                
                if(nearest_friend == nullptr) {
                    nearest_friend = other_unit;
                }
                
                // Find nearest friend
                if(std::abs(unit->x - other_unit->x) < std::abs(unit->x - nearest_friend->x)
                && std::abs(unit->y - other_unit->y) < std::abs(unit->y - nearest_friend->y)) {
                    nearest_friend = other_unit;
                }
            } else {
                // Foes from many ranges counts
                if(std::abs(unit->x - other_unit->x) >= 1.f && std::abs(unit->y - other_unit->y) >= 1.f)
                    continue;
                
                n_foes++;
                
                if(nearest_foe == nullptr) {
                    nearest_foe = other_unit;
                }

                // Find nearest foe
                if(std::abs(unit->x - other_unit->x) < std::abs(unit->x - nearest_foe->x)
                && std::abs(unit->y - other_unit->y) < std::abs(unit->y - nearest_foe->y)) {
                    nearest_foe = other_unit;
                }
            }
        }

        if((unit->x != unit->tx || unit->y != unit->ty)
        && (std::abs(unit->x - unit->tx) >= 0.2f || std::abs(unit->y - unit->ty) >= 0.2f)) {
            float end_x, end_y;
            const float speed = 0.1f;

            end_x = unit->x;
            end_y = unit->y;

            // Move towards target
            if(unit->x > unit->tx)
                end_x -= speed;
            else if(unit->x < unit->tx)
                end_x += speed;

            if(unit->y > unit->ty)
                end_y -= speed;
            else if(unit->y < unit->ty)
                end_y += speed;
            
            // This code prevents us from stepping onto water tiles (but allows for rivers)
            if(get_tile(end_x, end_y).elevation <= sea_level) {
                continue;
            }

            unit->x = end_x;
            unit->y = end_y;
        }
        
        // Make the unit attack automatically
        // and we must be at war with the owner of this unit to be able to attack the unit
        if(nearest_foe != nullptr && unit->owner->is_enemy(*nearest_foe->owner)) {
            unit->attack(*nearest_foe);
        }

        // Unit is on a non-wasteland part of the map
        if(get_tile(unit->x, unit->y).province_id != (Province::Id)-1) {
            Province* province = provinces[get_tile(unit->x, unit->y).province_id];
            const Nation* nation = province->owner;
            bool free_supplies = false;

            // Unit is on domestic soil, so we have to check the domsetic policy for free military supplies
            if(unit->owner == nation) {
                // No-cost supplies
                if(unit->owner->current_policy.free_supplies) {
                    free_supplies = true;
                }
            }
            // Unit is on foreign soil, we check relations to see if we can take free military supplies
            else {
                // No-cost supplies
                if(unit->owner->relations[get_id(nation)].free_supplies) {
                    free_supplies = true;
                }
            }

            if(free_supplies == true) {
                // Take anything you want, it's not needed to fucking pay at all! :)
                for(size_t j = 0; j < province->stockpile.size(); j++) {
                    if(!province->stockpile[j])
                        continue;

                    // We will take your food pleseantly
                    if(products[j]->good->is_edible && unit->supply <= (unit->type->supply_consumption * 10.f)) {
                        float bought = std::min(unit->size, province->stockpile[j]);
                        province->stockpile[j] -= bought;

                        unit->supply += bought / unit->size;
                        unit->morale += bought / unit->size;
                    }
                    // Fuck you, we are also taking your luxury because it's free
                    else {
                        float bought = std::min((rand() + 1) % unit->size, province->stockpile[j]);
                        province->stockpile[j] -= bought;

                        // Yes, we are super happy with your voluntary gifts to the honourable
                        // units of some nation
                        unit->morale += bought / unit->size;
                    }
                }
            } else {
                // Buy stuff and what we are able to buy normally
                for(size_t j = 0; j < province->stockpile.size(); j++) {
                    // Must be edible and there must be stock
                    if(!products[j]->good->is_edible || !province->stockpile[j])
                        continue;

                    if(products[j]->price * unit->size <= unit->budget) {
                        size_t bought = std::min(province->stockpile[j], unit->size);
                        province->stockpile[j] -= bought;
                        unit->supply = bought / unit->size;

                        // Pay (including taxes)
                        const float paid = (products[j]->price * unit->size) * province->owner->current_policy.med_flat_tax;
                        province->owner->budget += paid;
                        unit->budget -= paid;
                    }

                    // We will stop buying if we are satisfied
                    if(unit->supply >= (unit->type->supply_consumption * 1.f))
                        break;
                }
            }
        }

        // North and south do not wrap
        unit->y = std::max<float>(0.f, unit->y);
        unit->y = std::min<float>(height, unit->y);

        // West and east do wrap
        if(unit->x <= 0.f) {
            unit->x = width - 1.f;
        } else if(unit->x >= width) {
            unit->x = 0.f;
        }

        // Set nearby tiles as owned
        // TODO: Make it conquer multiple tiles
        Tile& tile = get_tile(unit->x, unit->y);
        if(tile.owner_id != get_id(unit->owner)) {
            tile.owner_id = get_id(unit->owner);

            std::lock_guard<std::recursive_mutex> lock(nation_changed_tiles_mutex);
            nation_changed_tiles.push_back(&get_tile(unit->x, unit->y));
            {
                std::pair<size_t, size_t> coord = std::make_pair((size_t)unit->x, (size_t)unit->y);
                // Broadcast to clients
                Packet packet = Packet(0);
                Archive ar = Archive();
                
                ActionType action = ActionType::TILE_UPDATE;
                ::serialize(ar, &action);
                ::serialize(ar, &coord.first);
                ::serialize(ar, &coord.second);
                ::serialize(ar, &tile);
                
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
            }
        }
    }
    
    for(const auto& unit: units) {
        // Broadcast to clients
        Packet packet = Packet();
        Archive ar = Archive();
        
        ActionType action = ActionType::UNIT_UPDATE;
        ::serialize(ar, &action);
        ::serialize(ar, &unit);
        ::serialize(ar, unit);
        
        packet.data(ar.get_buffer(), ar.size());
        g_server->broadcast(packet);
    }

    // Do the treaties clauses
    for(const auto& treaty: treaties) {
        // Check that the treaty is agreed by all parties before enforcing it
        bool on_effect = !(std::find_if(treaty->approval_status.begin(), treaty->approval_status.end(), [](auto& status) { return (status.second != TREATY_APPROVAL_ACCEPTED); }) != treaty->approval_status.end());
        if(!on_effect)
            continue;

        // And also check that there is atleast 1 clause that is on effect
        bool is_on_effect = false;
        for(const auto& clause: treaty->clauses) {
            if(clause->type == TREATY_CLAUSE_WAR_REPARATIONS) {
                auto dyn_clause = dynamic_cast<TreatyClause::WarReparations*>(clause);
                is_on_effect = dyn_clause->in_effect();
            } else if(clause->type == TREATY_CLAUSE_ANEXX_PROVINCES) {
                auto dyn_clause = dynamic_cast<TreatyClause::AnexxProvince*>(clause);
                is_on_effect = dyn_clause->in_effect();
            } else if(clause->type == TREATY_CLAUSE_LIBERATE_NATION) {
                auto dyn_clause = dynamic_cast<TreatyClause::LiberateNation*>(clause);
                is_on_effect = dyn_clause->in_effect();
            } else if(clause->type == TREATY_CLAUSE_HUMILIATE) {
                auto dyn_clause = dynamic_cast<TreatyClause::Humiliate*>(clause);
                is_on_effect = dyn_clause->in_effect();
            } else if(clause->type == TREATY_CLAUSE_IMPOSE_POLICIES) {
                auto dyn_clause = dynamic_cast<TreatyClause::ImposePolicies*>(clause);
                is_on_effect = dyn_clause->in_effect();
            } else if(clause->type == TREATY_CLAUSE_CEASEFIRE) {
                auto dyn_clause = dynamic_cast<TreatyClause::Ceasefire*>(clause);
                is_on_effect = dyn_clause->in_effect();
            }

            if(is_on_effect)
                break;
        }
        if(!is_on_effect)
            continue;
        
        // Treaties clauses now will be enforced
        print_info("Enforcing treaty %s", treaty->name.c_str());
        for(auto& clause: treaty->clauses) {
            if(clause->type == TREATY_CLAUSE_WAR_REPARATIONS) {
                auto dyn_clause = dynamic_cast<TreatyClause::WarReparations*>(clause);
                if(!dyn_clause->in_effect())
                    goto next_iter;
                dyn_clause->enforce();
            } else if(clause->type == TREATY_CLAUSE_ANEXX_PROVINCES) {
                auto dyn_clause = dynamic_cast<TreatyClause::AnexxProvince*>(clause);
                if(!dyn_clause->in_effect())
                    goto next_iter;
                dyn_clause->enforce();
            } else if(clause->type == TREATY_CLAUSE_LIBERATE_NATION) {
                auto dyn_clause = dynamic_cast<TreatyClause::LiberateNation*>(clause);
                if(!dyn_clause->in_effect())
                    goto next_iter;
                dyn_clause->enforce();
            } else if(clause->type == TREATY_CLAUSE_HUMILIATE) {
                auto dyn_clause = dynamic_cast<TreatyClause::Humiliate*>(clause);
                if(!dyn_clause->in_effect())
                    goto next_iter;
                dyn_clause->enforce();
            } else if(clause->type == TREATY_CLAUSE_IMPOSE_POLICIES) {
                auto dyn_clause = dynamic_cast<TreatyClause::ImposePolicies*>(clause);
                if(!dyn_clause->in_effect())
                    goto next_iter;
                dyn_clause->enforce();
            } else if(clause->type == TREATY_CLAUSE_CEASEFIRE) {
                auto dyn_clause = dynamic_cast<TreatyClause::Ceasefire*>(clause);
                if(!dyn_clause->in_effect())
                    goto next_iter;
                dyn_clause->enforce();
            }
        
        next_iter:
            ;
        }
    }

    LuaAPI::check_events(lua);

    //print_info("Tick %zu done", (size_t)time);
    time++;
    
    // Tell clients that this tick has been done
    Packet packet = Packet(0);
    Archive ar = Archive();
    ActionType action = ActionType::WORLD_TICK;
    ::serialize(ar, &action);
    packet.data(ar.get_buffer(), ar.size());
    g_server->broadcast(packet);
}
