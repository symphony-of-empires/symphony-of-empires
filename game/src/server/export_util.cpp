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

void Export::export_provinces(World& world) {
    std::ofstream myfile;
    myfile.open("provinces.csv");
    myfile << world.provinces.size() << "\n";
    for(const auto& province : world.provinces) {
        myfile << province.get_pos().x << "," << province.get_pos().y << "\n";
    }
    for(size_t prov_id = 0; prov_id < world.provinces.size(); prov_id++) {
        const auto& prov = world.provinces[prov_id];
        for(const auto neighbour_id : prov.neighbour_ids) {
            if(neighbour_id > prov_id)
                myfile << prov_id << "," << neighbour_id << "\n";
        }
    }
    myfile.close();
}
