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

// A product (based off a Good) which can be bought by POPs, converted by factories and transported
class Product : public IdEntity<uint16_t> {
public:
    Product() = default;
    ~Product() = default;
    
    inline void close_market() {
        if(this->demand > this->supply) {
            // Increase price with more demand
            this->price_vel += 0.001f * (this->demand - this->supply);
        } else if(this->demand < this->supply) {
            // Increase supply with more demand
            this->price_vel -= 0.001f * (this->supply - this->demand);
        } else {
            // Gravitate towards absolute zero due to volatility decay
            // (i.e, product price becomes stable without market activity)
            if(this->price_vel > 0.1f) {
                this->price_vel -= 0.01f;
            } else if(this->price_vel < -0.1f) {
                this->price_vel += 0.01f;
            } else {
                this->price_vel = -0.01f;
            }
        }

        // Set the new price
        this->price = std::max<float>(0.01f, this->price + this->price_vel);

        // Save prices and stuff onto history (for the charts!)
        this->demand_history.push_back(this->demand);
        if(this->demand_history.size() > 60)
            this->demand_history.pop_front();
        
        this->supply_history.push_back(this->supply);
        if(this->supply_history.size() > 60)
            this->supply_history.pop_front();

        this->price_history.push_back(this->price);
        if(this->price_history.size() > 60)
            this->price_history.pop_front();

        this->demand = 0;
    }

    // Price of the product
    float price;

    // Velocity of change of price of the product
    float price_vel;

    // Total supply of the product
    float supply;

    // Total demand of the product
    float demand;

    // History of price, supply and demand for the past 30 days
    std::deque<float> price_history;
    std::deque<float> supply_history;
    std::deque<float> demand_history;
};