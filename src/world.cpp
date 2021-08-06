#include "diplomacy.hpp"
#include "policy.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <cmath>
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif

#include "province.hpp"
#include "economy.hpp"
#include "world.hpp"
#include "binary_image.hpp"
#include "lua_api.hpp"
#include "path.hpp"
#include "print.hpp"
#include "pathfinding.hpp"

#include "serializer.hpp"
#include "io_impl.hpp"
#include "network.hpp"

// Mostly used by clients and lua API
World* g_world;

/**
 * Creates a new world
  */
World::World() {
    g_world = this;

    lua = luaL_newstate();
    luaL_openlibs(lua);

    // Register our API functions
    lua_register(lua, "_", LuaAPI::get_text);

    lua_register(lua, "add_unit_trait", LuaAPI::add_unit_trait);

    lua_register(lua, "add_outpost_type", LuaAPI::add_outpost_type);

    lua_register(lua, "add_good", LuaAPI::add_good);
    lua_register(lua, "get_good", LuaAPI::get_good);

    lua_register(lua, "add_industry_type", LuaAPI::add_industry_type);
    lua_register(lua, "get_industry_type", LuaAPI::get_industry_type);
    lua_register(lua, "add_input_to_industry_type", LuaAPI::add_input_to_industry_type);
    lua_register(lua, "add_output_to_industry_type", LuaAPI::add_output_to_industry_type);

    lua_register(lua, "add_nation", LuaAPI::add_nation);
    lua_register(lua, "get_nation", LuaAPI::get_nation);
    lua_register(lua, "set_nation_primary_culture", LuaAPI::set_nation_primary_culture);
    lua_register(lua, "set_nation_capital", LuaAPI::set_nation_capital);
    lua_register(lua, "set_industry_output_mod", LuaAPI::set_industry_output_mod);
    lua_register(lua, "set_industry_input_mod", LuaAPI::set_industry_input_mod);
    lua_register(lua, "set_workers_needed_mod", LuaAPI::set_workers_needed_mod);
    lua_register(lua, "set_salary_paid_mod", LuaAPI::set_salary_paid_mod);
    lua_register(lua, "set_delivery_cost_mod", LuaAPI::set_delivery_cost_mod);
    lua_register(lua, "set_literacy_learn_mod", LuaAPI::set_literacy_learn_mod);
    lua_register(lua, "set_reproduction_mod", LuaAPI::set_reproduction_mod);
    lua_register(lua, "set_death_mod", LuaAPI::set_death_mod);
    lua_register(lua, "set_militancy_mod", LuaAPI::set_militancy_mod);
    lua_register(lua, "set_consciousness_mod", LuaAPI::set_consciousness_mod);
    lua_register(lua, "set_life_needs_met_mod", LuaAPI::set_life_needs_met_mod);
    lua_register(lua, "set_everyday_needs_met_mod", LuaAPI::set_everyday_needs_met_mod);
    lua_register(lua, "set_luxury_needs_met_mod", LuaAPI::set_luxury_needs_met_mod);
    lua_register(lua, "add_nation_accepted_culture", LuaAPI::add_accepted_culture);
    
    lua_register(lua, "add_province", LuaAPI::add_province);
    lua_register(lua, "get_province", LuaAPI::get_province);
    lua_register(lua, "add_province_industry", LuaAPI::add_province_industry);
    lua_register(lua, "add_province_pop", LuaAPI::add_province_pop);
    lua_register(lua, "give_province_to", LuaAPI::give_province_to);
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

    lua_register(lua, "get_hour", LuaAPI::get_hour);
    lua_register(lua, "get_day", LuaAPI::get_day);
    lua_register(lua, "get_month", LuaAPI::get_month);
    lua_register(lua, "get_year", LuaAPI::get_year);

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
    std::string path = Path::get("scripts");
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
    } for(auto& industry_type: industry_types) {
        delete industry_type;
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
    } for(auto& outpost_type: outpost_types) {
        delete outpost_type;
    } for(auto& unit_trait: unit_traits) {
        delete unit_trait;
    } for(auto& boat_type: boat_types) {
        delete boat_type;
    } for(auto& product: products) {
        delete product;
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
    
    int ret = luaL_dofile(this->lua, Path::get("scripts/init.lua").c_str());
    if(ret) {
        throw std::runtime_error(lua_tostring(lua, -1));
    }

    // Shrink normally-not-resized vectors to give back memory to the OS
    printf("Shrink normally-not-resized vectors to give back memory to the OS\n");
    provinces.shrink_to_fit();
    nations.shrink_to_fit();
    goods.shrink_to_fit();
    industry_types.shrink_to_fit();
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
    printf("Translate all div, pol and topo maps onto this single tile array\n");
    for(size_t i = 0; i < total_size; i++) {
        // Set coordinates for the tiles
        tiles[i].owner_id = (Nation::Id)-1;
        tiles[i].province_id = (Province::Id)-1;
        tiles[i].elevation = topo.buffer[i] & 0x000000ff;
        if(topo.buffer[i] == 0xffff0000) {
            tiles[i].elevation = sea_level + 1;
        }

        // Set infrastructure level
        if(infra.buffer[i] == 0xffffffff
        || infra.buffer[i] == 0xff000000) {
            tiles[i].infra_level = 0;
        } else {
            tiles[i].infra_level = 1;
        }
    }

    // Associate tiles with provinces

    // Build a lookup table for super fast speed on finding provinces
    // 16777216 * 4 = c.a 64 MB, that quite a lot but we delete the table after anyways
    print_info("Building province lookup table");
    Province::Id* color_province_rel_table = new Province::Id[16777216];
    memset(color_province_rel_table, 0xff, sizeof(Province::Id) * 16777216);
    for(const auto& province: provinces) {
        color_province_rel_table[province->color & 0xffffff] = get_id(province);
    }

    // Uncomment this and see a bit more below
    //std::set<uint32_t> colors_found;
    print_info("Associate tiles with provinces");
    for(size_t i = 0; i < total_size; i++) {
        const uint32_t color = div.buffer[i];

        // This "skip the empty stuff" technique works!
        while((div.buffer[i] == 0xffffffff
        || div.buffer[i] == 0xff000000) && i < total_size) {
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
    print_info("Calculate the edges of the province (min and max x and y coordinates)");
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
    print_info("Correcting values for provinces");
    for(auto& province: provinces) {
        province->max_x = std::min(width, province->max_x);
        province->max_y = std::min(height, province->max_y);

        // Add stockpile
        for(size_t i = 0; i < products.size(); i++) {
            province->stockpile.push_back(0);
        }
    }

    // Give owners the entire provinces
    print_info("Give owners the entire provinces");
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
    print_info("Creating neighbours for provinces");
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
    print_info("Creating diplomatic relations");
    for(const auto& nation: this->nations) {
        // Relations between nations start at 0 (and latter modified by lua scripts)
        for(size_t i = 0; i < this->nations.size(); i++) {
            nation->relations.push_back(NationRelation{0.f, false, false, false, false, false, false, false, false, true, false});
        }
    }

    ret = luaL_dofile(this->lua, Path::get("scripts/mod.lua").c_str());
    if(ret) {
        throw std::runtime_error(lua_tostring(this->lua, -1));
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
    print_info("World fully intiialized");
}

#include <deque>
#include <mutex>
extern std::mutex render_province_mutex;
extern std::deque<size_t> render_province;

#include "economy.hpp"
void World::do_tick() {
    // Each tick == 30 minutes
    switch(time % (24* 2)) {
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

        products_mutex.lock();
        for(const auto& product: g_world->products) {
            // Broadcast to clients
            Packet packet = Packet();
            Archive ar = Archive();
                
            enum ActionType action = ACTION_PRODUCT_UPDATE;
            ::serialize(ar, &action);

            ::serialize(ar, &product); // ProductRef
            ::serialize(ar, product); // ProductObj
                
            packet.data(ar.get_buffer(), ar.size());
            g_server->broadcast(packet);
        }
        products_mutex.unlock();
        
        nations_mutex.lock();
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
        nations_mutex.unlock();
        break;
    // 18:00
    case 36:
        Economy::do_phase_4(*this);
        break;
    // 24:00, this is where clients are sent all information **at once**
    case 47:
        {
            size_t i;
            
            nations_mutex.lock();
            for(const auto& nation: g_world->nations) {
                // Broadcast to clients
                Packet packet = Packet();
                Archive ar = Archive();
                
                enum ActionType action = ACTION_NATION_UPDATE;
                ::serialize(ar, &action);

                ::serialize(ar, &nation); // NationRef
                ::serialize(ar, nation); // NationObj
                
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
            }
            nations_mutex.unlock();
            
            i = 0;
            provinces_mutex.lock();
            for(const auto& province: g_world->provinces) {
                // Broadcast to clients
                Packet packet = Packet();
                Archive ar = Archive();
                
                enum ActionType action = ACTION_PROVINCE_UPDATE;
                ::serialize(ar, &action);

                ::serialize(ar, &province); // ProvinceRef
                ::serialize(ar, province); // ProvinceObj
                
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
                
                i++;
            }
            provinces_mutex.unlock();
        }
        break;
    default:
        break;
    }

    // Evaluate boats
    boats_mutex.lock();
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
            Boat* enemy = nearest_foe;

            // Calculate the attack of our unit
            float attack_mod = 0.f;
            for(const auto& trait: unit->traits) {
                attack_mod *= trait->attack_mod;
            }
            const float attack = unit->type->attack * attack_mod;

            // Calculate the defense of the enemy
            float defense_mod = 0.f;
            for(const auto& trait: unit->traits) {
                defense_mod *= trait->defense_mod;
            }
            const float enemy_defense = std::max(0.1f, enemy->type->defense * defense_mod);

            // Calculate the total damage dealt by our unit to the enemy
            const float damage_dealt = unit->size * std::min(10.f, std::max(.05f, unit->experience))
                * (attack / std::pow(std::min(0.f, enemy_defense), 2))
                * std::max(0.1f, unit->morale) * unit->supply
            ;
            
            // Deal with the morale loss of the enemy
            float enemy_fanaticism = 0.f;
            for(const auto& trait: enemy->traits) {
                enemy_fanaticism *= trait->morale_mod;
            }
            enemy->morale -= 10.f * enemy_fanaticism * damage_dealt / enemy->size;

            // Our unit receives half of the morale
            unit->morale += 5.f * enemy_fanaticism * damage_dealt / enemy->size;

            // Deal the damage
            enemy->size -= damage_dealt;
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
        Packet packet = Packet(0);
        Archive ar = Archive();
        enum ActionType action = ACTION_BOAT_UPDATE;
        ::serialize(ar, &action);
        ::serialize(ar, &boat);
        ::serialize(ar, boat);
        packet.data(ar.get_buffer(), ar.size());
        g_server->broadcast(packet);
    }
    boats_mutex.unlock();
    
    // Evaluate units
    units_mutex.lock();
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
        if(nearest_foe != nullptr
        && unit->owner->relations[get_id(nearest_foe->owner)].has_war == false) {
            Unit* enemy = nearest_foe;

            // Calculate the attack of our unit
            float attack_mod = 0.f;
            for(const auto& trait: unit->traits) {
                attack_mod *= trait->attack_mod;
            }
            const float attack = unit->type->attack * attack_mod;

            // Calculate the defense of the enemy
            float defense_mod = 0.f;
            for(const auto& trait: unit->traits) {
                defense_mod *= trait->defense_mod;
            }
            const float enemy_defense = std::max(0.1f, enemy->type->defense * defense_mod
                * (enemy->defensive_ticks * enemy->type->position_defense))
            ;

            // Calculate the total damage dealt by our unit to the enemy
            const float damage_dealt = unit->size * std::min(10.f, std::max(.05f, unit->experience))
                * (attack / std::pow(std::min(0.f, enemy_defense), 2))
                * std::max(0.1f, unit->morale) * unit->supply
            ;
            
            // Deal with the morale loss of the enemy
            float enemy_fanaticism = 0.f;
            for(const auto& trait: enemy->traits) {
                enemy_fanaticism *= trait->morale_mod;
            }
            enemy->morale -= 10.f * enemy_fanaticism * damage_dealt / enemy->size;

            // Our unit receives half of the morale
            unit->morale += 5.f * enemy_fanaticism * damage_dealt / enemy->size;

            // Deal the damage
            enemy->size -= damage_dealt;
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
                    if(products[j]->good->is_edible
                    && unit->supply <= (unit->type->supply_consumption * 10.f)) {
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
        }
    }
    
    for(const auto& unit: units) {
        // Broadcast to clients
        Packet packet = Packet(0);
        Archive ar = Archive();
        enum ActionType action = ACTION_UNIT_UPDATE;
        ::serialize(ar, &action);
        ::serialize(ar, &unit);
        ::serialize(ar, unit);
        packet.data(ar.get_buffer(), ar.size());
        g_server->broadcast(packet);
    }
    units_mutex.unlock();

    // Do the treaties clauses
    treaties_mutex.lock();
    for(const auto& treaty: treaties) {
        // Check that the treaty is agreed by all parties before enforcing it
        bool is_on_effect = true;
        for(const auto& status: treaty->approval_status) {
            if(status.second != TREATY_APPROVAL_ACCEPTED) {
                is_on_effect = false;
                break;
            }
        }

        if(!is_on_effect)
            continue;

        // And also check that there is atleast 1 clause that is on effect
        for(const auto& clause: treaty->clauses) {
            switch(clause->type) {
            case TREATY_CLAUSE_WAR_REPARATIONS:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::WarReparations*>(clause);
                    is_on_effect = dyn_clause->in_effect();
                }
                break;
            case TREATY_CLAUSE_ANEXX_PROVINCES:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::AnexxProvince*>(clause);
                    is_on_effect = dyn_clause->in_effect();
                }
                break;
            case TREATY_CLAUSE_LIBERATE_NATION:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::LiberateNation*>(clause);
                    is_on_effect = dyn_clause->in_effect();
                }
                break;
            case TREATY_CLAUSE_HUMILIATE:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::Humiliate*>(clause);
                    is_on_effect = dyn_clause->in_effect();
                }
                break;
            case TREATY_CLAUSE_IMPOSE_POLICIES:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::ImposePolicies*>(clause);
                    is_on_effect = dyn_clause->in_effect();
                }
                break;
            case TREATY_CLAUSE_CEASEFIRE:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::Ceasefire*>(clause);
                    is_on_effect = dyn_clause->in_effect();
                }
                break;
            default:
                break;
            }

            if(is_on_effect) {
                break;
            }
        }
        if(!is_on_effect)
            continue;
        
        // Treaties clauses now will be enforced
        print_info("Enforcing treaty %s", treaty->name.c_str());
        for(auto& clause: treaty->clauses) {
            switch(clause->type) {
            case TREATY_CLAUSE_WAR_REPARATIONS:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::WarReparations*>(clause);
                    if(!dyn_clause->in_effect())
                        goto next_iter;
                    dyn_clause->enforce();
                }
                break;
            case TREATY_CLAUSE_ANEXX_PROVINCES:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::AnexxProvince*>(clause);
                    if(!dyn_clause->in_effect())
                        goto next_iter;
                    dyn_clause->enforce();
                }
                break;
            case TREATY_CLAUSE_LIBERATE_NATION:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::LiberateNation*>(clause);
                    if(!dyn_clause->in_effect())
                        goto next_iter;
                    dyn_clause->enforce();
                }
                break;
            case TREATY_CLAUSE_HUMILIATE:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::Humiliate*>(clause);
                    if(!dyn_clause->in_effect())
                        goto next_iter;
                    dyn_clause->enforce();
                }
                break;
            case TREATY_CLAUSE_IMPOSE_POLICIES:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::ImposePolicies*>(clause);
                    if(!dyn_clause->in_effect())
                        goto next_iter;
                    dyn_clause->enforce();
                }
                break;
            case TREATY_CLAUSE_CEASEFIRE:
                {
                    auto dyn_clause = dynamic_cast<TreatyClause::Ceasefire*>(clause);
                    if(!dyn_clause->in_effect())
                        goto next_iter;
                    dyn_clause->enforce();
                }
                break;
            default:
                break;
            }
        
        next_iter:
            ;
        }
    }
    treaties_mutex.unlock();
    
    // Tell clients that this tick has been done
    Packet packet = Packet(0);
    Archive ar = Archive();
    enum ActionType action = ACTION_WORLD_TICK;
    ::serialize(ar, &action);
    ::serialize(ar, &g_world->time);
    packet.data(ar.get_buffer(), ar.size());
    g_server->broadcast(packet);
    
    LuaAPI::check_events(lua);
    
    time++;
}

// TODO: Do a template or something to remove code duplication

Nation::Id World::get_id(const Nation* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(nations_mutex);
    return get_id_ptr<Nation::Id>(ptr, nations);
}

Province::Id World::get_id(const Province* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(provinces_mutex);
    return get_id_ptr<Province::Id>(ptr, provinces);
}

Product::Id World::get_id(const Product* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(products_mutex);
    return get_id_ptr<Product::Id>(ptr, products);
}

Good::Id World::get_id(const Good* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(goods_mutex);
    return get_id_ptr<Good::Id>(ptr, goods);
}

Culture::Id World::get_id(const Culture* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(cultures_mutex);
    return get_id_ptr<Culture::Id>(ptr, cultures);
}

Company::Id World::get_id(const Company* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(companies_mutex);
    return get_id_ptr<Company::Id>(ptr, companies);
}

IndustryType::Id World::get_id(const IndustryType* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(industry_types_mutex);
    return get_id_ptr<IndustryType::Id>(ptr, industry_types);
}

Industry::Id World::get_id(const Province& province, const Industry* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(provinces_mutex);
    return ((ptrdiff_t)ptr - (ptrdiff_t)&province.industries[0]) / sizeof(Industry);
}

Event::Id World::get_id(const Event* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(events_mutex);
    return get_id_ptr<Event::Id>(ptr, events);
}

UnitType::Id World::get_id(const UnitType* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(unit_types_mutex);
    return get_id_ptr<UnitType::Id>(ptr, unit_types);
}

BoatType::Id World::get_id(const BoatType* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(boat_types_mutex);
    return get_id_ptr<BoatType::Id>(ptr, boat_types);
}

UnitTrait::Id World::get_id(const UnitTrait* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(unit_traits_mutex);
    return get_id_ptr<UnitTrait::Id>(ptr, unit_traits);
}

Unit::Id World::get_id(const Unit* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(units_mutex);
    return get_id_ptr<Unit::Id>(ptr, units);
}

Boat::Id World::get_id(const Boat* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(boats_mutex);
    return get_id_ptr<Boat::Id>(ptr, boats);
}

OutpostType::Id World::get_id(const OutpostType* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(outpost_types_mutex);
    return get_id_ptr<OutpostType::Id>(ptr, outpost_types);
}

Outpost::Id World::get_id(const Outpost* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(outposts_mutex);
    return get_id_ptr<Outpost::Id>(ptr, outposts);
}

Treaty::Id World::get_id(const Treaty* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(treaties_mutex);
    return get_id_ptr<Treaty::Id>(ptr, treaties);
}

size_t World::get_id(const Tile* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(tiles_mutex);
    return ((ptrdiff_t)ptr - (ptrdiff_t)tiles) / sizeof(Tile);
}

// Obtains a tile from the world safely, and makes sure that it is in bounds
Tile& World::get_tile(size_t x, size_t y) const {
    std::lock_guard<std::recursive_mutex> lock(tiles_mutex);
    if(x >= width || y >= height) {
        throw std::runtime_error("Tile out of bounds");
    }
    return tiles[x + y * width];
}

Tile& World::get_tile(size_t idx) const {
    std::lock_guard<std::recursive_mutex> lock(tiles_mutex);
    if(idx >= width * height) {
        throw std::runtime_error("Tile index exceeds boundaries");
    }
    return tiles[idx];
}
