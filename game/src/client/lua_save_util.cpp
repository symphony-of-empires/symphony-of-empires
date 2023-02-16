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

#include "eng3d/serializer.hpp"
#include "eng3d/utils.hpp"
#include "client/game_state.hpp"
#include "world.hpp"
#include "world.hpp"

static void save_province(GameState& gs, FILE* fp, Province& province)
{
    if(province.neighbour_ids.empty()) return;
    if(province.box_area.right == 0 &&
        province.box_area.bottom == 0 &&
        province.box_area.left == gs.world->width &&
        province.box_area.top == gs.world->height) return;

        // Give the province a terrain
    if(gs.world->terrain_types[province.terrain_type_id].is_water_body) {
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
        const auto& commodity = gs.world->commodities[i];
        auto size = province.rgo_size[i];
        if(size) {
            rgo_size_out += string_format("{\"%s\",%zu},", commodity.ref_name.data(), size);
        }
    }

    const char *terrain_type_ref_name = gs.world->terrain_types[province.terrain_type_id].ref_name.data();
    if (province.owner_id == ProvinceId(0))
        terrain_type_ref_name = "sea";

    fprintf(fp, "province=Province:new{ref_name=\"%s\",name=translate(\"%s\"),color=0x%x,terrain=tt_%s,rgo_size={%s}}\n",
        province.ref_name.data(),
        province.name.data(),
        (unsigned int)color,
        terrain_type_ref_name,
        rgo_size_out.data());
    fprintf(fp, "province:register()\n");

    if(gs.world->terrain_types[province.terrain_type_id].is_water_body
    || province.owner_id == ProvinceId(0))
        return;
    
    // Add all essential buildings, such as farms and mines
    for(const auto& building_type : gs.world->building_types)
        province.buildings[building_type].level = building_type.input_ids.empty() ? 10 : 0;
        
    for(const auto& building_type : gs.world->building_types) {
        const auto& building = province.buildings[building_type];
        if(building.level)
            fprintf(fp, "province:create_building(bt_%s,%i)\n", building_type.ref_name.data(), (int)building.level);
    }

    // POPs
    for(const auto& pop : province.pops)
        fprintf(fp, "province:add_pop(pt_%s,%f,%f)\n", gs.world->pop_types[pop.type_id].ref_name.data(), pop.size, pop.literacy);
    for(const auto& language : gs.world->languages)
        if(province.languages[language] > 0.f)
            fprintf(fp, "province:set_language(c_%s,%f)\n", language.ref_name.data(), province.languages[language]);
    for(const auto& religion : gs.world->religions)
        if(province.religions[religion] > 0.f)
            fprintf(fp, "province:set_religion(r_%s,%f)\n", religion.ref_name.data(), province.religions[religion]);
    // Nuclei of the provinces
    for(const auto& nucleus_id : province.nuclei)
        fprintf(fp, "province:add_nucleus(n_%s)\n", gs.world->nations[nucleus_id].ref_name.data());
    // Give province to owner
    fprintf(fp, "province:give_to(n_%s)\n", gs.world->nations[province.owner_id].ref_name.data());
    if(gs.world->nations[province.owner_id].capital_id == gs.world->get_id(province))
        fprintf(fp, "n_%s:set_capital(province)\n", gs.world->nations[province.owner_id].ref_name.data());
    // Units
    for(const auto unit_id : gs.world->unit_manager.get_province_units(province)) {
        auto& unit = gs.world->unit_manager.units[unit_id];
        fprintf(fp, "province:add_unit(ut_%s,%zu)\n", gs.world->unit_types[unit.type_id].ref_name.data(), (size_t)unit.size);
    }
}

void LUA_util::save(GameState& gs, const std::string_view savefile_path) {
    if(gs.editor) {
        std::filesystem::create_directory("editor");
        std::filesystem::create_directory("editor/lua");
        std::filesystem::create_directory("editor/lua/entities");
        std::filesystem::create_directory("editor/map");
        // Provinces
        auto fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/provinces.lua", "wt"), fclose);
        size_t cnt = 0;
        for(const auto& building_type : gs.world->building_types)
            fprintf(fp.get(), "bt_%s=BuildingType:get(\"%s\")\n", building_type.ref_name.data(), building_type.ref_name.data());
        for(const auto& unit_type : gs.world->unit_types)
            fprintf(fp.get(), "ut_%s=UnitType:get(\"%s\")\n", unit_type.ref_name.data(), unit_type.ref_name.data());
        for(const auto& terrain_type : gs.world->terrain_types)
            fprintf(fp.get(), "tt_%s=TerrainType:get(\"%s\")\n", terrain_type.ref_name.data(), terrain_type.ref_name.data());
        for(const auto& pop_type : gs.world->pop_types)
            fprintf(fp.get(), "pt_%s=PopType:get(\"%s\")\n", pop_type.ref_name.data(), pop_type.ref_name.data());
        for(const auto& language : gs.world->languages)
            fprintf(fp.get(), "c_%s=Language:get(\"%s\")\n", language.ref_name.data(), language.ref_name.data());
        for(const auto& religion : gs.world->religions)
            fprintf(fp.get(), "r_%s=Religion:get(\"%s\")\n", religion.ref_name.data(), religion.ref_name.data());
        for(const auto& nation : gs.world->nations)
            fprintf(fp.get(), "n_%s=Nation:get(\"%s\")\n", nation.ref_name.data(), nation.ref_name.data());

        // First add provinces with pops, then the provinces **without** pops
        for(auto& province : gs.world->provinces) {
            if(province.is_populated())
                save_province(gs, fp.get(), province);
            cnt++;
        }
        for(auto& province : gs.world->provinces) {
            if(!province.is_populated())
                save_province(gs, fp.get(), province);
            cnt++;
        }
        fp.reset();

        // Terrain types
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/terrain_types.lua", "wt"), fclose);
        cnt = 0;
        for(const auto& terrain_type : gs.world->terrain_types) {
            const uint32_t color = std::byteswap<std::uint32_t>((terrain_type.color & 0x00ffffff) << 8);
            fprintf(fp.get(), "TerrainType:new{ref_name=\"%s\",name=translate(\"%s\"),color=0x%x,is_water_body=%s}:register()\n", terrain_type.ref_name.data(), terrain_type.name.data(), (unsigned int)color, terrain_type.is_water_body ? "true" : "false");
            cnt++;
        }
        fp.reset();

        // Religions
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/religions.lua", "wt"), fclose);
        cnt = 0;
        for(const auto& religion : gs.world->religions) {
            const uint32_t color = std::byteswap<std::uint32_t>((religion.color & 0x00ffffff) << 8);
            fprintf(fp.get(), "Religion:new{ref_name=\"%s\",name=translate(\"%s\"),color=0x%x}:register()\n", religion.ref_name.data(), religion.name.data(), (unsigned int)color);
            cnt++;
        }
        fp.reset();

        // Pop types
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/pop_types.lua", "wt"), fclose);
        cnt = 0;
        for(const auto& pop_type : gs.world->pop_types) {
            fprintf(fp.get(), "PopType:new{ ref_name=\"%s\",name=translate(\"%s\"),social_value=%f}\n", pop_type.ref_name.data(), pop_type.name.data(), pop_type.social_value);
            cnt++;
        }
        fp.reset();

        // Unit types
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/unit_types.lua", "wt"), fclose);
        cnt = 0;
        fprintf(fp.get(), "local v = {}\n");
        for(const auto& unit_type : gs.world->unit_types) {
            if(unit_type.is_ground == true && unit_type.is_naval == false) {
                fprintf(fp.get(), "v=UnitType:new{ref_name=\"%s\",name=translate(\"%s\"),defense=%f,attack=%f,health=%f,speed=%f}\n", unit_type.ref_name.data(), unit_type.name.data(), unit_type.defense, unit_type.attack, unit_type.max_health, unit_type.speed);
            } else if(unit_type.is_ground == false && unit_type.is_naval == true) {
                fprintf(fp.get(), "v=BoatType:new{ref_name=\"%s\",name=translate(\"%s\"),defense=%f,attack=%f,health=%f,speed=%f}\n", unit_type.ref_name.data(), unit_type.name.data(), unit_type.defense, unit_type.attack, unit_type.max_health, unit_type.speed);
            } else if(unit_type.is_ground == true && unit_type.is_naval == true) {
                fprintf(fp.get(), "v=AirplaneType:new{ref_name=\"%s\",name=translate(\"%s\"),defense=%f,attack=%f,health=%f,speed=%f}\n", unit_type.ref_name.data(), unit_type.name.data(), unit_type.defense, unit_type.attack, unit_type.max_health, unit_type.speed);
            }
            fprintf(fp.get(), "v:register()\n");
            for(const auto& [good_id, amount] : unit_type.req_goods)
                fprintf(fp.get(), "v:requires_good(Commodity:get(\"%s\"), %f)\n", gs.world->commodities[good_id].ref_name.data(), amount);
            cnt++;
        }
        fp.reset();

        // Commodity types
        fp = std::unique_ptr<FILE, int (*)(FILE*)>(fopen("editor/lua/entities/good_types.lua", "wt"), fclose);
        fprintf(fp.get(), "-- Generated by editor :)\n");
        for(const auto& good_type : gs.world->commodities)
            fprintf(fp.get(), "Commodity:new{ref_name=\"%s\",name=translate(\"%s\")}:register()\n", good_type.ref_name.data(), good_type.name.data());
        fp.reset();

        gs.ui_ctx.prompt("Save", "Editor data saved! (check editor folder)");
    } else {
        Eng3D::Deser::Archive ar{};
        const auto nation_id = gs.curr_nation->get_id();
        Eng3D::Deser::serialize(ar, nation_id);
        Eng3D::Deser::serialize(ar, *gs.world);
        ar.to_file(savefile_path);
        gs.ui_ctx.prompt("Save", "Saved sucessfully!");
    }
}

void LUA_util::load(GameState& gs, const std::string_view savefile_path) {
    gs.paused = true;

    Eng3D::Deser::Archive ar{};
    ar.from_file(savefile_path);
    auto nation_id = gs.curr_nation->get_id();
    Eng3D::Deser::deserialize(ar, nation_id);
    Eng3D::Deser::deserialize(ar, *gs.world);

    /// @todo Events aren't properly saved yet
    gs.world->events.clear();
    gs.world->taken_decisions.clear();
    for(auto& nation : gs.world->nations)
        nation.inbox.clear();
    gs.world->load_mod();
    gs.ui_ctx.prompt("Loaded", "Loaded savefile");
}
