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
//      pop.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include "eng3d/entity.hpp"

#include "religion.hpp"
#include "ideology.hpp"
#include "language.hpp"

enum class PopGroup : int {
    OTHER = 0x01,
    BURGEOISE = 0x02,
    SLAVE = 0x04,
    FARMER = 0x08,
    LABORER = 0x10,
};

class PopType : public RefnameEntity<uint8_t> {
public:
    PopType() = default;
    ~PopType() = default;

    Eng3D::StringRef name;
    float social_value;
    enum PopGroup group;
    std::vector<float> basic_needs_amount; // Amount of goods needed to satisfy basic needs
    std::vector<float> luxury_needs_satisfaction; // Amount of satisfaction each luxury good gives
    std::vector<float> luxury_needs_deminishing_factor; // Deminishing returns factor of the luxury good satisfaction
};

class Province;
class Pop {
    Pop& operator=(const Pop&) = default;
    friend class Province;
public:
    using Id = uint8_t;

    Pop() = default;
    ~Pop() = default;

    constexpr bool operator==(const Pop& rhs) const {
        return this->type_id == rhs.type_id;
    }

    Ideology& get_ideology() const;
    uint32_t get_type_id() const;

    PopType::Id type_id;
    float size = 0.f;
    float life_needs_met = 0.f;
    float everyday_needs_met = 0.f;
    float luxury_needs_met = 0.f;
    float literacy = 0.f;
    float militancy = 0.f;
    float budget = 0.f;
    std::vector<float> ideology_approval; // Approval % of all the ideologies (1:1)
};