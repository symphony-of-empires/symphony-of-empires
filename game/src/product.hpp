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
//      product.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <deque>
#include <limits>
#include <glm/glm.hpp>
#include "objects.hpp"

/// @brief A good, mostly serves as a "product type"
struct Good : RefnameEntity<GoodId> {
    Eng3D::StringRef name;
    std::string get_icon_path() const {
        return string_format("gfx/good/%s.png", ref_name.c_str());
    }
};
template<>
struct Serializer<Good> {
    template<bool is_const>
    using type = CondConstType<is_const, Good>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
    }
};

/// @brief A product (based off a Good) which can be bought by POPs, converted by factories and transported
struct Product : Entity<ProductId> {
    void close_market() {
        if(this->demand > this->supply) {
            // Increase price with more demand
            this->price_delta += 0.001f * (this->demand - this->supply);
        } else if(this->demand < this->supply) {
            // Increase supply with more demand
            this->price_delta -= 0.001f * (this->supply - this->demand);
        } else {
            // Gravitate towards absolute zero due to volatility decay
            // (i.e, product price becomes stable without market activity)
            if(this->price_delta > 0.1f) this->price_delta -= 0.01f;
            else if(this->price_delta < -0.1f) this->price_delta += 0.01f;
            else this->price_delta = -0.01f;
        }

        // Set the new price
        this->price = glm::clamp(this->price + this->price_delta, 0.01f, 100'000.f);
        this->demand = 0.f;
    }

    /// @brief Buy a portion of the item
    /// @param amount Amount to buy
    /// @return float Total cost of purchase
    float buy(float amount) {
        this->demand += amount;
        this->supply -= glm::min(amount, this->supply);
        return this->price * amount;
    }

    /// @brief Produce the product
    /// @param amount Amount to produce
    /// @return float Total gains from production
    float produce(float amount) {
        this->supply += amount;
        return this->price * amount;
    }

    float price = 1.f;
    float price_delta = 0.f;
    float supply = 1.f;
    float demand = 1.f;
};
template<>
struct Serializer<Product> {
    template<bool is_const>
    using type = CondConstType<is_const, Product>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.price);
        ::deser_dynamic<is_serialize>(ar, obj.price_delta);
        ::deser_dynamic<is_serialize>(ar, obj.supply);
        ::deser_dynamic<is_serialize>(ar, obj.demand);
    }
};

