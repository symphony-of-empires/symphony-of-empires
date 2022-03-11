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
    std::vector<float> good_needs; 
};

#include "culture.hpp"
#include "religion.hpp"
class Pop {
public:
    bool operator==(const Pop& rhs) const;
    Ideology* get_ideology(void) const;

    size_t size;
    size_t unemployed = 0;
    UnifiedRender::Decimal life_needs_met = 0, everyday_needs_met = 0, luxury_needs_met = 0;
    UnifiedRender::Decimal literacy = 0, militancy = 0, con = 0;
    UnifiedRender::Decimal budget = 0, savings = 0;

    PopType* type;
    Culture* culture;
    Religion* religion;

    // Approval % of all the ideologies (1:1)
    std::vector<UnifiedRender::Decimal> ideology_approval;
};