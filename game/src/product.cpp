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
//      product.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "product.hpp"

//
// Product
//
/*
Product::Product(void) {

}

Product::~Product(void) {
    
}
//*/

// "Close" the product for this specific day, which means the equilibrium of price
// will change according to the changes in the market
void Product::close_market(void) {
    // Increase price with more demand
    if(this->demand > this->supply) {
        this->price_vel += 0.001f * (this->demand - this->supply);
    }
    // Increase supply with more demand
    else if(this->demand < this->supply) {
        this->price_vel -= 0.001f * (this->supply - this->demand);
    }
    // Gravitate towards absolute zero due to volatility decay
    // (i.e, product price becomes stable without market activity)
    else {
        if(this->price_vel > 0.1f) {
            this->price_vel -= 0.01f;
        } else if(this->price_vel < -0.1f) {
            this->price_vel += 0.01f;
        } else {
            this->price_vel = -0.01f;
        }
    }

    // Set the new price
    this->price = std::max<UnifiedRender::Decimal>(0.01f, this->price + this->price_vel);

    // Save prices and stuff onto history (for the charts!)
    this->demand_history.push_back(this->demand);
    if(this->demand_history.size() > 60) {
        this->demand_history.pop_front();
    }

    this->supply_history.push_back(this->supply);
    if(this->supply_history.size() > 60) {
        this->supply_history.pop_front();
    }

    this->price_history.push_back(this->price);
    if(this->price_history.size() > 60) {
        this->price_history.pop_front();
    }

    this->demand = 0;
}