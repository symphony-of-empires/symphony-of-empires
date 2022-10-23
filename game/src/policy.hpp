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
    float min_wage = 1.f; // Minimum wage
    
    inline float difference(const Policies& rhs) const {
        auto diff = 0.f;
        diff += std::abs(rhs.min_wage - this->min_wage);
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
    }
};
