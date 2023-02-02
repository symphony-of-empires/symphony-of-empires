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
#include <glm/gtx/compatibility.hpp>
#include "objects.hpp"

/// @brief A commodity, mostly serves as a "product type"
struct Commodity : RefnameEntity<CommodityId> {
    Eng3D::StringRef name;
    std::string get_icon_path() const {
        return string_format("gfx/commodity/%s.png", ref_name.data());
    }
};
template<>
struct Eng3D::Deser::Serializer<Commodity> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Commodity>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
    }
};

/// @brief A product (based off a Commodity) which can be bought by POPs, converted by factories and transported
struct Product : Entity<ProductId> {
    static constexpr float min_price = 0.01f;
    static constexpr float max_price = 999'999.99f;

    float get_price_delta() const {
        constexpr float min_delta = 0.9f; // -10%
        constexpr float max_delta = 1.1; // +10%
        if(demand == 0.f && supply == 0.f)
            return min_delta;
        if(produced == 0.f)
            return min_delta;
        // Produts actually brought, versus the sold amount
        return glm::clamp(bought / produced, min_delta, max_delta);
    }

    float sd_ratio() const {
        if(demand <= 0.f)
            return supply;
        return supply / demand;
    }

    float ds_ratio() const {
        if(supply <= 0.f)
            return demand;
        return demand / supply;
    }

    void close_market() {
        // Increase price with more demand
        this->price_delta = this->get_price_delta();
        assert(this->price_delta >= 0.f);
        // Set the new price
        this->price = glm::clamp(this->price * this->price_delta, min_price, max_price);
        if(glm::epsilonEqual(this->price, 0.f, glm::epsilon<float>()))
            this->price_delta = 0.f;
        
        this->supply -= this->bought;
        this->demand = this->produced = this->bought = 0.f;
    }

    /// @brief Buy a portion of the item
    /// @param wanted_amount Total wanted amount to buy (to add to demand)
    /// @param amount Amount actually bought
    /// @return float Total cost of purchase
    float buy(float wanted_amount, float& amount) {
        if(this->supply < this->bought) return 0.f;
        assert(amount >= 0.f && wanted_amount >= 0.f);

        // Increment demand (how much desire it is for a product)
        this->demand += wanted_amount;

        // Buy the product in question
        amount = glm::clamp(wanted_amount, 0.f, this->supply - this->bought);
        this->bought += amount;
        return this->price * amount;
    }

    /// @brief Produce the product
    /// @param amount Amount to produce
    /// @return float Total gains from production
    float produce(float amount) {
        assert(amount >= 0.f);
        this->produced += amount;
        this->supply += amount;
        return this->price * amount;
    }

    float price = 1.f;
    float price_delta = 0.f;
    float supply = 0.f;
    float demand = 0.f;
    float bought = 0.f;
    float produced = 0.f;
    float global_demand = 0.f;
};
template<>
struct Eng3D::Deser::Serializer<Product> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Product>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.price);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.price_delta);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.supply);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.demand);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.bought);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.produced);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.global_demand);
    }
};

