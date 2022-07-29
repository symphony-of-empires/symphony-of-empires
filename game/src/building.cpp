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
//      building.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "building.hpp"
#include "product.hpp"
#include "province.hpp"
#include "world.hpp"
#include "unit.hpp"

//
// Building
//
/// @brief Adds a good by id to a building stockpile
void Building::add_to_stock(const Good& good, const size_t add) {
    stockpile[good.get_id()] += add;
}
