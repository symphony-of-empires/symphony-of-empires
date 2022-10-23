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
//      client/lua_save_util.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "lua_save_util.hpp"

#include <filesystem>

#include "eng3d/utils.hpp"
#include "client/game_state.hpp"
#include "world.hpp"

static void save_province(GameState& gs, FILE* fp, Province& province)
{
    if(province.neighbour_ids.empty()) return;
    if(province.box_area.right == 0 &&
        province.box_area.bottom == 0 &&
        province.box_area.left == gs.world->width &&
        province.box_area.top == gs.world->height) return;

        // Give the province a terrain
    if(gs.world->terrain_types[province.terrain_type_id].is_water_body && (Nation::is_valid(province.controller_id) || Nation::is_valid(province.owner_id))) {
        for(auto& terrain : gs.world->terrain_types) {
            if(terrain.is_water_body) continue;
            province.terrain_type_id = terrain;
            break;
        }
    }

    // Sea provinces dont have pops or RGOs
    if(gs.world->terrain_types[province.terrain_type_id].is_water_body)
        province.rgo_size.clear();

    // RGO
    const uint32_t color = std::byteswap<std::uint32_t>((province.color & 0x00ffffff) << 8);
    std::string rgo_size_out = "";
    for(size_t i = 0; i < province.rgo_size.size(); i++) {
        const auto& good = gs.world->goods[i];
        auto size = province.rgo_size[i];
        if(size)
            rgo_size_out += string_format("{\"%s\",%zu}", good.ref_name.c_str(), size);
    }
    fprintf(fp, "province=Province:new{ref_name=\"%s\",name=translate(\"%s\"),color=0x%x,terrain=tt_%s,rgo_size={%s}}\n",
        province.ref_name.c_str(),
        province.name.c_str(),
        (unsigned int)color,
        gs.world->terrain_types[province.terrain_type_id].ref_name.c_str(),
        rgo_size_out.c_str());
    fprintf(fp, "province:register()\n");

    if(gs.world->terrain_types[province.terrain_type_id].is_water_body)
        return;
    
    for(auto& building : province.buildings)
        building.level = 0;
    province.buildings[rand() % province.buildings.size()].level = 10;
    province.buildings[rand() % province.buildings.size()].level = 10;
    for(const auto& building_type : gs.world->building_types) {
        const auto& building = province.buildings[building_type];
        if(building.level)
            fprintf(fp, "province:update_building(bt_%s,%i)\n", building_type.ref_name.c_str(), (int)building.level);
    }

    // POPs
    for(const auto& language : gs.world->languages)
        if(province.languages[language])
            fprintf(fp, "province:set_language(c_%s,%f)\n", language.ref_name.c_str(), province.languages[language]);
    for(const auto& religion : gs.world->religions)
        if(province.religions[religion])
            fprintf(fp, "province:set_religion(r_%s,%f)\n", religion.ref_name.c_str(), province.religions[religion]);
    for(const auto& pop : province.pops)
        fprintf(fp, "province:add_pop(pt_%s,%f,%f)\n", gs.world->pop_types[pop.type_id].ref_name.c_str(), pop.size, pop.literacy);
    // Nuclei of the provinces
    for(const auto& nucleus_id : province.nuclei)
        fprintf(fp, "province:add_nucleus(n_%s)\n", gs.world->nations[nucleus_id].ref_name.c_str());
    // Give province to owner
    if(Nation::is_valid(province.owner_id)) {
        fprintf(fp, "province:give_to(n_%s)\n", gs.world->nations[province.owner_id].ref_name.c_str());
        if(gs.world->nations[province.owner_id].capital_id == gs.world->get_id(province))
            fprintf(fp, "n_%s:set_capital(province)\n", gs.world->nations[province.owner_id].ref_name.c_str());
    }
    // Units
    for(const auto unit_id : gs.world->unit_manager.get_province_units(province)) {
        auto& unit = gs.world->unit_manager.units[unit_id];
        fprintf(fp, "province:add_unit(ut_%s,%zu)\n", gs.world->unit_types[unit.type_id].ref_name.c_str(), (size_t)unit.size);
    }
}

void LUA_util::save(GameState& gs, const std::string& savefile_path) {
    if(gs.editor) {
        std::filesystem::create_directory("editor");
        std::filesystem::create_directory("editor/lua");
        std::filesystem::create_directory("editor/lua/entities");
        std::filesystem::create_directory("editor/map");
        // Provinces
        auto fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/provinces.lua", "wt"), fclose);
        size_t cnt = 0;
        for(const auto& building_type : gs.world->building_types)
            fprintf(fp.get(), "bt_%s=BuildingType:get(\"%s\")\n", building_type.ref_name.c_str(), building_type.ref_name.c_str());
        for(const auto& unit_type : gs.world->unit_types)
            fprintf(fp.get(), "ut_%s=UnitType:get(\"%s\")\n", unit_type.ref_name.c_str(), unit_type.ref_name.c_str());
        for(const auto& terrain_type : gs.world->terrain_types)
            fprintf(fp.get(), "tt_%s=TerrainType:get(\"%s\")\n", terrain_type.ref_name.c_str(), terrain_type.ref_name.c_str());
        for(const auto& pop_type : gs.world->pop_types)
            fprintf(fp.get(), "pt_%s=PopType:get(\"%s\")\n", pop_type.ref_name.c_str(), pop_type.ref_name.c_str());
        for(const auto& language : gs.world->languages)
            fprintf(fp.get(), "c_%s=Language:get(\"%s\")\n", language.ref_name.c_str(), language.ref_name.c_str());
        for(const auto& religion : gs.world->religions)
            fprintf(fp.get(), "r_%s=Religion:get(\"%s\")\n", religion.ref_name.c_str(), religion.ref_name.c_str());
        for(const auto& nation : gs.world->nations)
            fprintf(fp.get(), "n_%s=Nation:get(\"%s\")\n", nation.ref_name.c_str(), nation.ref_name.c_str());

        // First add provinces with pops, then the provinces **without** pops
        for(auto& province : gs.world->provinces) {
            if(province.is_populated() || Nation::is_valid(province.owner_id))
                save_province(gs, fp.get(), province);
            cnt++;
        }
        for(auto& province : gs.world->provinces) {
            if(!province.is_populated() && Nation::is_invalid(province.owner_id))
                save_province(gs, fp.get(), province);
            cnt++;
        }
        fp.reset();

        // Terrain types
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/terrain_types.lua", "wt"), fclose);
        cnt = 0;
        for(const auto& terrain_type : gs.world->terrain_types) {
            const uint32_t color = std::byteswap<std::uint32_t>((terrain_type.color & 0x00ffffff) << 8);
            fprintf(fp.get(), "TerrainType:new{ref_name=\"%s\",name=translate(\"%s\"),color=0x%x,is_water_body=%s}:register()\n", terrain_type.ref_name.c_str(), terrain_type.name.c_str(), (unsigned int)color, terrain_type.is_water_body ? "true" : "false");
            cnt++;
        }
        fp.reset();

        // Religions
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/religions.lua", "wt"), fclose);
        cnt = 0;
        for(const auto& religion : gs.world->religions) {
            const uint32_t color = std::byteswap<std::uint32_t>((religion.color & 0x00ffffff) << 8);
            fprintf(fp.get(), "Religion:new{ref_name=\"%s\",name=translate(\"%s\"),color=0x%x}:register()\n", religion.ref_name.c_str(), religion.name.c_str(), (unsigned int)color);
            cnt++;
        }
        fp.reset();

        // Pop types
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/pop_types.lua", "wt"), fclose);
        cnt = 0;
        for(const auto& pop_type : gs.world->pop_types) {
            std::string extra_arg = "";
            switch(pop_type.group) {
            case PopGroup::BURGEOISE: extra_arg = ",is_burgeoise=true"; break;
            case PopGroup::LABORER: extra_arg = ",is_laborer=true"; break;
            case PopGroup::SLAVE: extra_arg = ",is_slave=true"; break;
            case PopGroup::SOLDIER: extra_arg = ",is_soldier=true"; break;
            case PopGroup::ARTISAN: extra_arg = ",is_artisan=true"; break;
            default: break;
            }
            fprintf(fp.get(), "PopType:new{ ref_name=\"%s\",name=translate(\"%s\"),social_value=%f%s}\n", pop_type.ref_name.c_str(), pop_type.name.c_str(), pop_type.social_value, extra_arg.c_str());
            cnt++;
        }
        fp.reset();

        // Unit types
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/unit_types.lua", "wt"), fclose);
        cnt = 0;
        fprintf(fp.get(), "local v = {}\n");
        for(const auto& unit_type : gs.world->unit_types) {
            if(unit_type.is_ground == true && unit_type.is_naval == false) {
                fprintf(fp.get(), "v=UnitType:new{ref_name=\"%s\",name=translate(\"%s\"),defense=%f,attack=%f,health=%f,speed=%f}\n", unit_type.ref_name.c_str(), unit_type.name.c_str(), unit_type.defense, unit_type.attack, unit_type.max_health, unit_type.speed);
            } else if(unit_type.is_ground == false && unit_type.is_naval == true) {
                fprintf(fp.get(), "v=BoatType:new{ref_name=\"%s\",name=translate(\"%s\"),defense=%f,attack=%f,health=%f,speed=%f}\n", unit_type.ref_name.c_str(), unit_type.name.c_str(), unit_type.defense, unit_type.attack, unit_type.max_health, unit_type.speed);
            } else if(unit_type.is_ground == true && unit_type.is_naval == true) {
                fprintf(fp.get(), "v=AirplaneType:new{ref_name=\"%s\",name=translate(\"%s\"),defense=%f,attack=%f,health=%f,speed=%f}\n", unit_type.ref_name.c_str(), unit_type.name.c_str(), unit_type.defense, unit_type.attack, unit_type.max_health, unit_type.speed);
            }
            fprintf(fp.get(), "v:register()\n");
            for(const auto& [good_id, amount] : unit_type.req_goods)
                fprintf(fp.get(), "v:requires_good(Good:get(\"%s\"), %f)\n", gs.world->goods[good_id].ref_name.c_str(), amount);
            cnt++;
        }
        fp.reset();

        // Good types
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/good_types.lua", "wt"), fclose);
        fprintf(fp.get(), "-- Generated by editor :)\n");
        for(const auto& good_type : gs.world->goods)
            fprintf(fp.get(), "Good:new{ref_name=\"%s\",name=translate(\"%s\")}:register()\n", good_type.ref_name.c_str(), good_type.name.c_str());
        fp.reset();

        gs.ui_ctx.prompt("Save", "Editor data saved! (check editor folder)");
    } else {
        Archive ar{};
        ::serialize(ar, gs.curr_nation);
        ::serialize(ar, *gs.world);
        ar.to_file(savefile_path);
        gs.ui_ctx.prompt("Save", "Saved sucessfully!");
    }
}

void LUA_util::load(GameState& gs, const std::string& savefile_path) {
    gs.paused = true;

    Archive ar{};
    ar.from_file(savefile_path);
    ::deserialize(ar, gs.curr_nation);
    ::deserialize(ar, *gs.world);

    /// @todo Events aren't properly saved yet
    gs.world->events.clear();
    gs.world->taken_decisions.clear();
    for(auto& nation : gs.world->nations)
        nation.inbox.clear();
    gs.world->load_mod();
    gs.ui_ctx.prompt("Loaded", "Loaded savefile");
}
