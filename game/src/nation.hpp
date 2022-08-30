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
class Language;
class Religion;
namespace TreatyClause {
    class BaseClause;
}

// Included due to ids
#include "province.hpp"

// Defines a one side relation between a country
// This allows for cases where a country A hates country B, but country B loves country A
class NationRelation {
public:
    constexpr NationRelation() = default;
    ~NationRelation() = default;

    float relation = 0.f;
    bool has_embargo = false;
    bool has_war = false;
    bool has_alliance = false;
    bool has_defensive_pact = false;
    bool has_military_access = false;
    bool has_market_access = false;
};

// Hints for the client on how to display the nation
class NationClientHint {
public:
    NationClientHint() = default;
    ~NationClientHint() = default;

    uint32_t color;
    Eng3D::StringRef alt_name; // Alternate name, for example communist Russia would be called USSR
    Ideology* ideology = nullptr; // Ideology to which this hint applies to (nullptr = default fallback)
};

class NationModifier : public RefnameEntity<uint16_t> {
public:
    NationModifier() = default;
    ~NationModifier() = default;

    Eng3D::StringRef name;
    // Modifiers for a nation, which increases/decreases certain stuff
    // They should never be 0, a modifier of 1.0 is equal to no modifer at
    // all. And a modifier of 0.5 would cause a 1/2. Similar to a 2 which
    // would make a x2
    float industry_output_mod = 1.f;
    float industry_input_mod = 1.f;
    float workers_needed_mod = 1.f;
    float salary_paid_mod = 1.f;
    float delivery_cost_mod = 1.f;
    float literacy_learn_mod = 1.f;
    float reproduction_mod = 1.f;
    float death_mod = 1.f;
    float militancy_mod = 1.f;
    float life_needs_met_mod = 1.f;
    float everyday_needs_met_mod = 1.f;
    float luxury_needs_met_mod = 1.f;
    float immigration_attraction = 1.f;
};

class Nation : public RefnameEntity<uint16_t> {
    inline void do_diplomacy();
    inline bool can_do_diplomacy() const;

    Nation & operator=(const Nation&) = default;
public:
    Nation() = default;
    ~Nation() = default;
    void declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses = std::vector<TreatyClause::BaseClause*>());
    bool is_ally(const Nation& nation) const;
    bool is_enemy(const Nation& nation) const;
    bool exists() const;
    void auto_relocate_capital();
    void set_policy(const Policies& policies);
    bool is_accepted_language(const Pop& pop) const;
    bool is_accepted_language(const Language& language) const;
    bool is_accepted_religion(const Pop& pop) const;
    bool is_accepted_religion(const Religion& relgion) const;
    float get_tax(const Pop& pop) const;
    void give_province(Province& province);
    void control_province(Province& province);
    const NationClientHint& get_client_hint() const;
    float get_research_points() const;
    bool can_research(const Technology& tech) const;
    void change_research_focus(const Technology& tech);
    std::vector<Nation*> get_allies();
    float get_industry_output_mod();
    float get_industry_input_mod();
    float get_workers_needed_mod();
    float get_salary_paid_mod();
    float get_delivery_cost_mod();
    float get_literacy_learn_mod();
    float get_reproduction_mod();
    float get_death_mod();
    float get_militancy_mod();
    float get_life_needs_met_mod();
    float get_everyday_needs_met_mod();
    float get_luxury_needs_met_mod();
    float get_immigration_attraction_mod();

    Eng3D::StringRef name;
    Nation* puppet_master = nullptr; // Pupeeter of this nation (if any)
    float diplomacy_points; // Amount of diplomacy points available
    float prestige = 0.1f; // Amount of prestige
    float infamy = 0; // Level of infamy
    // 3 key scores used to define a nation's minimum prestige, how willing would the AI
    // be to challenge this nations and other valuable stuff
    float military_score = 0.f, naval_score = 0.f, economy_score = 0.f;
    // Total budget of the nation (money in ark), this is not equal to GDP, the GDP is the total sum of the price
    // of all products in the nation, which are volatile unless they are sold
    float budget = 0.f;
    // Default and can be disabled by the player
    bool ai_controlled = true;
    bool ai_do_cmd_troops = true;

    Province::Id capital_id = Province::invalid(); // The capital of this nation (can be invalid id)
    Ideology* ideology = nullptr; // Current ideology of the nation
    Policies current_policy; // Current policy of this nation
    uint16_t diplomatic_timer; // Time until a diplomacy can be done
    Technology* focus_tech = nullptr; // Current tech being researched
    // Accepted languages in this nation, the accepted languages may have some bonuses on provinces *totally*
    // owned by this nation
    std::vector<float> language_discrim;
    std::vector<float> religion_discrim;
    // List of provinces which are owned by this nation (including partial ownership)
    /// @todo Add controlled provinces to serializer
    std::set<uint16_t> owned_provinces;
    std::set<uint16_t> controlled_provinces;
    std::vector<NationModifier*> modifiers;
    std::deque<Event> inbox; // Inbox of the nation; events that require our attention / should be processed
    std::vector<float> research; // Progress on technologies (1:1)

    std::vector<NationClientHint> client_hints; // Hints for the client on how to draw a nation on the client
    std::string client_username; // Used by clients to store usernames from nations - not saved
};