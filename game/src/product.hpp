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
//      product.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <deque>
#include "eng3d/decimal.hpp"

#include "province.hpp"
#include "good.hpp"
#include "building.hpp"

// A product (based off a Good) which can be bought by POPs, converted by factories and transported
class Product : public IdEntity<uint16_t> {
public:
    //Product();
    //~Product();
    void close_market(void);

    // Price of the product
    Eng3D::Decimal price;

    // Velocity of change of price of the product
    Eng3D::Decimal price_vel;

    // Total supply of the product
    Eng3D::Number supply;

    // Total demand of the product
    Eng3D::Number demand;

    // History of price, supply and demand for the past 30 days
    std::deque<Eng3D::Decimal> price_history;
    std::deque<Eng3D::Number> supply_history;
    std::deque<Eng3D::Number> demand_history;
};