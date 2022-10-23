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

#include "eng3d/serializer.hpp"

#include "building.hpp"
#include "world.hpp"
#include "province.hpp"

/// @brief Checks if the building can produce output (if it has enough input)
bool Building::can_do_output(const Province& province) const {
    auto& world = World::get_instance();
    // Check that we have enough stockpile
    for(const auto& product : province.products)
        if(product.supply == 0.f) return false;
    return this->level > 0.f;
}
