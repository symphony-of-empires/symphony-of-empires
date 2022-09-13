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
#include <limits>

/// @brief A product (based off a Good) which can be bought by POPs, converted by factories and transported
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
            if(this->price_vel > 0.1f)
                this->price_vel -= 0.01f;
            else if(this->price_vel < -0.1f)
                this->price_vel += 0.01f;
            else
                this->price_vel = -0.01f;
        }

        // Set the new price
        this->price = std::max<float>(0.01f, this->price + this->price_vel);
        //this->demand = 0;
    }

    /// @brief Price of the product
    float price = 1.f;
    /// @brief Velocity of change of price of the product
    float price_vel = 0.f;
    /// @brief Total supply of the product
    float supply = 0.f;
    /// @brief Total demand of the product
    float demand = 0.f;
};