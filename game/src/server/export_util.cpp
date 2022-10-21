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
//      server/trade.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "server/export_util.hpp"
#include "world.hpp"
#include "province.hpp"

#include <iostream>
#include <fstream>

void Export::export_provinces(const World& world) {
    std::ofstream myfile;
    myfile.open("provinces.csv");
    myfile << world.provinces.size() << "\n";
    for(auto& province : world.provinces) {
        bool is_water = world.terrain_types[province.terrain_type_id].is_water_body;
        myfile << province.get_pos().x << "," << province.get_pos().y
            << "," << std::string(is_water ? "water" : "land") << "\n";
    }

    auto world_size = glm::vec2(world.width, world.height);
    for(size_t i = 0; i < world.provinces.size(); i++) {
        const auto& prov = world.provinces[i];
        for(auto& neighbour_id : prov.neighbour_ids) {
            const auto& neighbour = world.provinces[neighbour_id];
            auto distance = prov.euclidean_distance(neighbour, world_size, 1.);
            bool is_water = world.terrain_types[prov.terrain_type_id].is_water_body
                || world.terrain_types[neighbour.terrain_type_id].is_water_body;
            if(is_water) distance *= 0.1;
            if(static_cast<size_t>(neighbour_id) > i)
                myfile << i << "," << static_cast<size_t>(neighbour_id) << "," << distance << "\n";
        }
    }
    myfile.close();
}
