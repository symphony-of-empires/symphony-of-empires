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
//      policy.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "objects.hpp"

struct Policies {
    // Economic
    float min_wage = 1.f;

    float factory_profit_tax = 0.1f;
    float burgeoise_tax = 0.1f;
    float pop_tax = 0.1f;

    float private_ownership = 1.f; // 0 - Ownership diallowed, ... - Max % ownable, 1 - Ownership allowed
    float foreign_ownership = 1.f;
    float collective_ownership = 1.f;
    float individual_ownership = 1.f;

    float education_budget = 0.f; // State sponsored education
    float factory_subsidies_budget = 0.f; // Subsidized factories
    
    inline float difference(const Policies& rhs) const {
        auto diff = 0.f;
        diff += std::abs(rhs.min_wage - this->min_wage);
        diff += std::abs(rhs.factory_profit_tax - this->factory_profit_tax);
        diff += std::abs(rhs.burgeoise_tax - this->burgeoise_tax);
        diff += std::abs(rhs.pop_tax - this->pop_tax);
        diff += std::abs(rhs.private_ownership - this->private_ownership);
        diff += std::abs(rhs.foreign_ownership - this->foreign_ownership);
        diff += std::abs(rhs.collective_ownership - this->collective_ownership);
        diff += std::abs(rhs.individual_ownership - this->individual_ownership);
        diff += std::abs(rhs.education_budget - this->education_budget);
        diff += std::abs(rhs.factory_subsidies_budget - this->factory_subsidies_budget);
        return diff;
    }
};
template<>
struct Serializer<Policies> {
    template<bool is_const>
    using type = CondConstType<is_const, Policies>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.min_wage);
        ::deser_dynamic<is_serialize>(ar, obj.factory_profit_tax);
        ::deser_dynamic<is_serialize>(ar, obj.burgeoise_tax);
        ::deser_dynamic<is_serialize>(ar, obj.pop_tax);
        ::deser_dynamic<is_serialize>(ar, obj.private_ownership);
        ::deser_dynamic<is_serialize>(ar, obj.foreign_ownership);
        ::deser_dynamic<is_serialize>(ar, obj.collective_ownership);
        ::deser_dynamic<is_serialize>(ar, obj.individual_ownership);
        ::deser_dynamic<is_serialize>(ar, obj.foreign_ownership);
        ::deser_dynamic<is_serialize>(ar, obj.education_budget);
        ::deser_dynamic<is_serialize>(ar, obj.factory_subsidies_budget);
    }
};
