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

/// @brief A good, mostly serves as a "product type"
struct Good : RefnameEntity<GoodId> {
    Eng3D::StringRef name;
    std::string get_icon_path() const {
        return string_format("gfx/good/%s.png", ref_name.c_str());
    }
};
template<>
struct Eng3D::Deser::Serializer<Good> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, Good>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
    }
};

/// @brief A product (based off a Good) which can be bought by POPs, converted by factories and transported
struct Product : Entity<ProductId> {
    static constexpr float get_price_delta(float supply, float demand) {
        constexpr auto price_elasticity = 0.01f;
        if(supply == 0.f || demand == 0.f)
            return price_elasticity * price_elasticity * (demand - supply);
        const auto sd_ratio = supply / demand;
        return price_elasticity * (1.f - sd_ratio);
    }

    void close_market() {
        // TODO: Supply should **never** be negative
        this->supply = glm::max(this->supply, 0.f);
        // Increase price with more demand
        this->price_delta = this->get_price_delta(this->supply, this->demand);

        // Set the new price
        this->price = glm::clamp(this->price + this->price_delta, glm::epsilon<float>(), 100'000.f);
        if(glm::epsilonEqual(this->price, 0.f, glm::epsilon<float>()))
            this->price_delta = 0.f;
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
    float import_price = 1.f;
    float price_delta = 0.f;
    float supply = 1.f;
    float demand = 1.f;
    float speculative_demand = 0.f;
};
template<>
struct Eng3D::Deser::Serializer<Product> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, Product>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.price);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.price_delta);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.supply);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.demand);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.speculative_demand);
    }
};

