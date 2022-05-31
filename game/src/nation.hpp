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
//      nation.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <queue>
#include <deque>
#include <set>
#include <unordered_set>
#include <string>
#include <vector>

#include "eng3d/entity.hpp"
#include "eng3d/string.hpp"

#include "policy.hpp"
#include "event.hpp"

class Province;
class Ideology;
class Technology;
class Pop;
class Culture;
class Religion;
namespace TreatyClause {
    class BaseClause;
}

// Defines a one side relation between a country
// This allows for cases where a country A hates country B, but country B loves country A
class NationRelation {
public:
    NationRelation() {};
    ~NationRelation() {};

    Eng3D::Decimal relation;
    Eng3D::Decimal interest;
    bool has_embargo;
    bool has_war;
    bool has_alliance;
    bool has_defensive_pact;
    bool has_military_access;
    bool has_market_access;
};

// Hints for the client on how to display the nation
class NationClientHint {
public:
    NationClientHint() {};
    ~NationClientHint() {};

    uint32_t color;

    // Alternate name, for example communist Russia would be called USSR
    Eng3D::StringRef alt_name;

    // Ideology to which this hint applies to (nullptr = default fallback)
    Ideology* ideology;
};

class NationModifier : public RefnameEntity<uint16_t> {
public:
    NationModifier() {};
    ~NationModifier() {};

    Eng3D::StringRef name;
    // Modifiers for a nation, which increases/decreases certain stuff
    // They should never be 0, a modifier of 1.0 is equal to no modifer at
    // all. And a modifier of 0.5 would cause a 1/2. Similar to a 2 which
    // would make a x2
    Eng3D::Decimal industry_output_mod = 1.f;
    Eng3D::Decimal industry_input_mod = 1.f;
    Eng3D::Decimal workers_needed_mod = 1.f;
    Eng3D::Decimal salary_paid_mod = 1.f;
    Eng3D::Decimal delivery_cost_mod = 1.f;
    Eng3D::Decimal literacy_learn_mod = 1.f;
    Eng3D::Decimal reproduction_mod = 1.f;
    Eng3D::Decimal death_mod = 1.f;
    Eng3D::Decimal militancy_mod = 1.f;
    Eng3D::Decimal life_needs_met_mod = 1.f;
    Eng3D::Decimal everyday_needs_met_mod = 1.f;
    Eng3D::Decimal luxury_needs_met_mod = 1.f;
    Eng3D::Decimal immigration_attraction = 1.f;
};

class Nation : public RefnameEntity<uint16_t> {
    inline void do_diplomacy();
    inline bool can_do_diplomacy() const;
public:
    Nation() {};
    ~Nation() {};
    void declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses = std::vector<TreatyClause::BaseClause*>());
    bool is_ally(const Nation& nation) const;
    bool is_enemy(const Nation& nation) const;
    bool exists() const;
    void increase_relation(Nation& target);
    void decrease_relation(Nation& target);
    void auto_relocate_capital();
    void set_policy(const Policies& policies);
    bool is_accepted_culture(const Pop& pop) const;
    bool is_accepted_culture(const Culture& culture) const;
    bool is_accepted_religion(const Pop& pop) const;
    bool is_accepted_religion(const Religion& relgion) const;
    Eng3D::Decimal get_tax(const Pop& pop) const;
    void give_province(Province& province);
    void control_province(Province& province);
    const NationClientHint& get_client_hint() const;
    Eng3D::Decimal get_research_points() const;
    bool can_research(const Technology& tech) const;
    void change_research_focus(const Technology& tech);
    std::vector<Nation*> get_allies();
    Eng3D::Decimal get_industry_output_mod();
    Eng3D::Decimal get_industry_input_mod();
    Eng3D::Decimal get_workers_needed_mod();
    Eng3D::Decimal get_salary_paid_mod();
    Eng3D::Decimal get_delivery_cost_mod();
    Eng3D::Decimal get_literacy_learn_mod();
    Eng3D::Decimal get_reproduction_mod();
    Eng3D::Decimal get_death_mod();
    Eng3D::Decimal get_militancy_mod();
    Eng3D::Decimal get_life_needs_met_mod();
    Eng3D::Decimal get_everyday_needs_met_mod();
    Eng3D::Decimal get_luxury_needs_met_mod();
    Eng3D::Decimal get_immigration_attraction_mod();

    Eng3D::StringRef name;
    Nation* puppet_master = nullptr; // Pupeeter of this nation (if any)
    Eng3D::Decimal diplomacy_points; // Amount of diplomacy points available
    Eng3D::Decimal prestige = 0.1f; // Amount of prestige
    Eng3D::Decimal infamy = 0; // Level of infamy
    // 3 key scores used to define a nation's minimum prestige, how willing would the AI
    // be to challenge this nations and other valuable stuff
    Eng3D::Decimal military_score = 0, naval_score = 0, economy_score = 0;
    // Total budget of the nation (money in ark), this is not equal to GDP, the GDP is the total sum of the price
    // of all products in the nation, which are volatile unless they are sold
    Eng3D::Decimal budget;
    // Default and can be disabled by the player
    bool ai_controlled = true;
    bool ai_do_cmd_troops = true;

    Province* capital = nullptr; // The capital of this nation (can be nullptr)
    Ideology* ideology = nullptr; // Current ideology of the nation
    Policies current_policy; // Current policy of this nation
    uint16_t diplomatic_timer; // Time until a diplomacy can be done
    Technology* focus_tech = nullptr; // Current tech being researched

    // Hints for the client on how to draw a nation :)
    std::vector<NationClientHint> client_hints;
    // Accepted cultures in this nation, the accepted cultures may have some bonuses on provinces *totally*
    // owned by this nation
    std::vector<float> culture_discrim;
    std::vector<float> religion_discrim;
    // List of provinces which are owned by this nation (including partial ownership)
    /// @todo Add controlled provinces to serializer
    std::set<Province*> owned_provinces, controlled_provinces;
    std::set<Nation*> neighbours; // Neighbouring nations
    std::vector<NationModifier*> modifiers; //std::vector<std::pair<Technology*, float>> techs;
    std::deque<Event> inbox; // Inbox of the nation; events that require our attention / should be processed
    std::vector<Eng3D::Decimal> research; // Progress on technologies (1:1)
};