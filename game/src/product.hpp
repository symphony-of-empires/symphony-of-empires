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
#include "province.hpp"
#include "good.hpp"
#include "building.hpp"

// A product (based off a Good) which can be bought by POPs, converted by factories and transported
// accross the world
class Product : public IdEntity<uint16_t> {
public:
    //Product();
    //~Product();
    void close_market(void);

    // Good that this product is based on
    Good* good;

    // Price of the product
    float price;

    // Velocity of change of price of the product
    float price_vel;

    // Total supply (worldwide) of the product
    size_t supply;

    // Total demand (worldwide) of the product
    size_t demand;

    // History of price, supply and demand for the past 30 days
    std::deque<float> price_history;
    std::deque<size_t> supply_history;
    std::deque<size_t> demand_history;
};