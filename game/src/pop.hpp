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
//      pop.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include "unified_render/entity.hpp"
#include "unified_render/decimal.hpp"
#include "ideology.hpp"

enum class PopGroup {
    BURGEOISE,
    Slave,
    FARMER,
    LABORER,
    Other,
};

class PopType : public RefnameEntity<uint8_t> {
public:
    UnifiedRender::Decimal social_value;
    PopGroup group;
    // The amount of goods needed to satisfy basic needs
    std::vector<float> basic_needs_amount; 
    // The amount of satisfaction each luxury good gives
    std::vector<float> luxury_needs_satisfaction; 
    // The deminishing returns factor of the luxury good satisfaction
    std::vector<float> luxury_needs_deminishing_factor; 
};

#include "culture.hpp"
#include "religion.hpp"
class Pop {
public:
    bool operator==(const Pop& rhs) const;
    Ideology* get_ideology(void) const;

    UnifiedRender::Number size = 0.f;
    UnifiedRender::Decimal life_needs_met = 0.f, everyday_needs_met = 0.f, luxury_needs_met = 0.f;
    UnifiedRender::Decimal literacy = 0.f, militancy = 0.f, con = 0.f;
    UnifiedRender::Decimal budget = 0.f, savings = 0.f;

    PopType* type = nullptr;
    Culture* culture = nullptr;
    Religion* religion = nullptr;

    // Approval % of all the ideologies (1:1)
    std::vector<UnifiedRender::Decimal> ideology_approval;
};