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
//      building.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include "eng3d/serializer.hpp"

#include "world.hpp"
#include "world.hpp"
#include "world.hpp"

/// @brief Checks if the building can produce output (if it has enough input)
bool Building::can_do_output(const Province& province, const std::vector<CommodityId>& inputs) const {
    auto& world = World::get_instance();
    // Check that we have enough stockpile
    for(const auto& commodity : world.commodities)
        if(std::find(std::begin(inputs), std::end(inputs), commodity.get_id()) != std::end(inputs))
            if(province.products[commodity].supply == 0.f)
                return false;
    return this->level > 0.f;
}

void Building::work_on_unit(const UnitType& unit_type) {
    this->working_unit_type_id.emplace(unit_type.get_id());
    this->req_goods_for_unit = unit_type.req_goods;
}
