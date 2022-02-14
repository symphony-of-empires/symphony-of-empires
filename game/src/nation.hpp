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

#include "unified_render/entity.hpp"

#include "policy.hpp"
#include "province.hpp"
#include "diplomacy.hpp"
#include "event.hpp"
#include "ideology.hpp"
#include "pop.hpp"
class Technology;

// Defines a one side relation between a country
// This allows for cases where a country A hates country B, but country B loves country A
class NationRelation {
public:
    UnifiedRender::Decimal relation;
    // Interest of a nation on this nation
    UnifiedRender::Decimal interest;

    // Whetever commercial operations are allowed on on the target country
    bool has_embargo;
    bool has_war;
    bool has_alliance;
    bool has_defensive_pact;
    bool has_truce;
    bool has_embassy;
    bool has_military_access;
    bool has_market_access;

    // A nation can have free supplies to feed their soldiers
    bool free_supplies;
};

// Hints for the client on how to display the nation
class NationClientHint {
public:
    uint32_t color;

    // Alternate name, for example communist Russia would be called USSR
    std::string alt_name;

    // Ideology to which this hint applies to (nullptr = default fallback)
    Ideology* ideology;
};

class NationModifier : public RefnameEntity<uint16_t> {
public:
    // Modifiers for a nation, which increases/decreases certain stuff
    // They should never be 0, a modifier of 1.0 is equal to no modifer at
    // all. And a modifier of 0.5 would cause a 1/2. Similar to a 2 which
    // would make a x2
    UnifiedRender::Decimal industry_output_mod = 1.f;
    UnifiedRender::Decimal industry_input_mod = 1.f;
    UnifiedRender::Decimal workers_needed_mod = 1.f;
    UnifiedRender::Decimal salary_paid_mod = 1.f;
    UnifiedRender::Decimal delivery_cost_mod = 1.f;
    UnifiedRender::Decimal literacy_learn_mod = 1.f;
    UnifiedRender::Decimal reproduction_mod = 1.f;
    UnifiedRender::Decimal death_mod = 1.f;
    UnifiedRender::Decimal militancy_mod = 1.f;
    UnifiedRender::Decimal con_mod = 1.f;
    UnifiedRender::Decimal life_needs_met_mod = 1.f;
    UnifiedRender::Decimal everyday_needs_met_mod = 1.f;
    UnifiedRender::Decimal luxury_needs_met_mod = 1.f;
    UnifiedRender::Decimal immigration_attraction = 1.f;
};

class Nation : public RefnameEntity<uint16_t> {
    inline void do_diplomacy();
    inline bool can_do_diplomacy();
public:
    //Nation();
    //~Nation();
    void declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses = std::vector<TreatyClause::BaseClause*>());
    bool is_ally(const Nation& nation);
    bool is_enemy(const Nation& nation);
    bool exists(void);
    void increase_relation(Nation& target);
    void decrease_relation(Nation& target);
    void auto_relocate_capital(void);
    void set_policy(Policies& policies);
    bool is_accepted_culture(const Pop& pop) const;
    bool is_accepted_culture(const Culture& culture) const;
    bool is_accepted_religion(const Pop& pop) const;
    bool is_accepted_religion(const Religion& relgion) const;
    UnifiedRender::Decimal get_tax(const Pop& pop) const;
    void give_province(Province& province);
    const NationClientHint& get_client_hint(void);
    UnifiedRender::Decimal get_research_points(void) const;
    bool can_research(const Technology* tech) const;
    void change_research_focus(Technology* tech);
    std::vector<Nation*> get_allies(void);
    UnifiedRender::Decimal get_industry_output_mod(void);
    UnifiedRender::Decimal get_industry_input_mod(void);
    UnifiedRender::Decimal get_workers_needed_mod(void);
    UnifiedRender::Decimal get_salary_paid_mod(void);
    UnifiedRender::Decimal get_delivery_cost_mod(void);
    UnifiedRender::Decimal get_literacy_learn_mod(void);
    UnifiedRender::Decimal get_reproduction_mod(void);
    UnifiedRender::Decimal get_death_mod(void);
    UnifiedRender::Decimal get_militancy_mod(void);
    UnifiedRender::Decimal get_con_mod(void);
    UnifiedRender::Decimal get_life_needs_met_mod(void);
    UnifiedRender::Decimal get_everyday_needs_met_mod(void);
    UnifiedRender::Decimal get_luxury_needs_met_mod(void);
    UnifiedRender::Decimal get_immigration_attraction_mod(void);

    // Nation that has us on their sphere of influence
    // This value is equal to the nation with highest influence on us in the
    // relations vector
    Nation* spherer;

    // A list with relations with all other nations, mapped 1:1 to the Nation list in the world
    std::vector<NationRelation> relations;

    // Number of diplomacy points available
    UnifiedRender::Decimal diplomacy_points;

    // Total number of prestige
    UnifiedRender::Decimal prestige = 0.1f;

    // Base literacy applied to all pops
    UnifiedRender::Decimal base_literacy;

    // Determines if the nation is civilized
    bool is_civilized;

    // Level of infamy
    UnifiedRender::Decimal infamy = 0;

    // 3 key scores used to define a nation's minimum prestige, how willing would the AI
    // be to challenge this nations and other valuable stuff
    UnifiedRender::Decimal military_score = 0, naval_score = 0, economy_score = 0;

    // Total budget of the nation (money in ark), this is not equal to GDP, the GDP is the total sum of the price
    // of all products in the nation, which are volatile unless they are sold
    UnifiedRender::Decimal budget;

    // Total GDP of the nation
    UnifiedRender::Decimal gdp = 0;

    // The capital of this nation (can be nullptr)
    Province* capital = nullptr;

    // Hints for the client on how to draw a nation :)
    std::vector<NationClientHint> client_hints;
    Ideology* ideology = nullptr;

    // Accepted cultures in this nation, the accepted cultures may have some bonuses on provinces *totally*
    // owned by this nation
    std::unordered_set<Culture*> accepted_cultures;
    std::unordered_set<Religion*> accepted_religions;

    // List of provinces which are owned by this nation (including partial ownership)
    std::set<Province*> owned_provinces;

    // List of neighbouring nations
    std::set<Nation*> neighbours;

    // A pointer to a class defining the current policy of this nation
    Policies current_policy;

    // Time until a diplomatic action can be done
    uint16_t diplomatic_timer;

    //std::vector<std::pair<Technology*, float>> techs;
    std::vector<NationModifier*> modifiers;

    // Inbox of the nation; events that require our attention / should be processed
    std::deque<Event*> inbox;

    // Progress on technologies (1:1)
    std::vector<UnifiedRender::Decimal> research;

    // Current focused tech
    Technology* focus_tech = nullptr;

    // Default and can be disabled by the player
    bool ai_do_policies = true;
    bool ai_do_research = true;
    bool ai_do_diplomacy = true;
    bool ai_do_cmd_troops = true;
    bool ai_do_unit_production = true;
    bool ai_do_build_production = true;
    bool ai_handle_treaties = true;
    bool ai_handle_events = true;
};