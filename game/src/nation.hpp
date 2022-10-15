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
#include <functional>
#include "eng3d/string.hpp"

#include "objects.hpp"
#include "event.hpp"
#include "policy.hpp"

class Province;
class Ideology;
struct Technology;
class Pop;
class Language;
class Religion;
namespace TreatyClause {
    class BaseClause;
}

struct Ideology;
struct Technology;
class Nation : public RefnameEntity<NationId> {
    void do_diplomacy() {
        /// @todo Fix this formula which is currently broken
        //diplomatic_timer = glm::max((60 * 48) - glm::min(10.f * 48.f, prestige / 100.f), 4.f);
        diplomatic_timer = 48;
    }

    bool can_do_diplomacy() const {
        return diplomatic_timer == 0;
    }
    Nation& operator=(const Nation&) = default;
public:
    /// @brief Diplomatic relations between two nations
    struct Relation {
        bool is_allied() const {
            return alliance > 0.f && !has_war;
        }

        bool has_landpass() const {
            return relation > 0.5f || alliance > 0.f || has_war;
        }

        float relation = 0.f;
        bool has_war = false;
        float alliance = 0.f; // From 0 to 1; 0 = diplomatic alliance, tariiff excemption
                              // 1 = political alliance, after this they can form a single country
    };

    /// @brief Hints for the client on how to display the nation
    struct ClientHint {
        uint32_t color;
        Eng3D::StringRef name;
        IdeologyId ideology_id;
    };

    void declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses = std::vector<TreatyClause::BaseClause*>());
    bool is_ally(const Nation& nation) const;
    bool is_enemy(const Nation& nation) const;
    void auto_relocate_capital();
    void set_policy(const Policies& policies);
    bool is_accepted_language(const Language& language) const;
    bool is_accepted_religion(const Religion& relgion) const;
    float get_tax(const Pop& pop) const;
    void give_province(Province& province);
    void control_province(Province& province);
    const Nation::ClientHint& get_client_hint() const;
    float get_research_points() const;
    bool can_research(const Technology& tech) const;
    void change_research_focus(const Technology& tech);
    void get_allies(std::function<void(const Nation&)> fn) const;

    /// @brief Whetever the nation exists at all - we cannot add nations in-game so we just check
    /// if the nation "exists" at all, this means that it has a presence and a goverment
    /// must own atleast 1 province
    bool exists() const {
        return !(controlled_provinces.empty());
    }

    Eng3D::StringRef name;
    float prestige = 0.1f; // Amount of prestige
    // Total budget of the nation (money in ark), this is not equal to GDP, the GDP is the total sum of the price
    // of all products in the nation, which are volatile unless they are sold
    float budget = 10000.f;
    // Default and can be disabled by the player
    bool ai_controlled = true;
    bool ai_do_cmd_troops = true;

    NationId puppet_master_id; // Pupeeter of this nation (if any)
    ProvinceId capital_id; // The capital of this nation (can be invalid id)
    IdeologyId ideology_id; // Current ideology of the nation
    TechnologyId focus_tech_id; // Current tech being researched
    Policies current_policy; // Current policy of this nation
    uint16_t diplomatic_timer; // Time until a diplomacy can be done
    // Accepted languages in this nation, the accepted languages may have some bonuses on provinces *totally*
    // owned by this nation
    std::vector<float> language_discrim;
    std::vector<float> religion_discrim;
    // List of provinces which are owned by this nation (including partial ownership)
    std::vector<ProvinceId> owned_provinces;
    std::vector<ProvinceId> controlled_provinces;
    std::deque<Event> inbox; // Inbox of the nation; events that require our attention / should be processed
    std::vector<float> research; // Progress on technologies (1:1)
    std::vector<Nation::ClientHint> client_hints; // Hints for the client on how to draw a nation on the client
    std::string client_username; // Used by clients to store usernames from nations - not saved
};
template<>
struct Serializer<Nation::Relation> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Nation::Relation& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.alliance);
        ::deser_dynamic<is_serialize>(ar, obj.has_war);
        ::deser_dynamic<is_serialize>(ar, obj.relation);
    }
};
template<>
struct Serializer<Nation::ClientHint> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Nation::ClientHint& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.color);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ideology_id);
    }
};
template<>
struct Serializer<Nation*> : SerializerReference<World, Nation> {};
template<>
struct Serializer<const Nation*> : SerializerReference<World, const Nation> {};
template<>
struct Serializer<Nation> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Nation& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.puppet_master_id);
        ::deser_dynamic<is_serialize>(ar, obj.prestige);
        ::deser_dynamic<is_serialize>(ar, obj.budget);
        ::deser_dynamic<is_serialize>(ar, obj.capital_id);
        ::deser_dynamic<is_serialize>(ar, obj.language_discrim);
        ::deser_dynamic<is_serialize>(ar, obj.religion_discrim);
        ::deser_dynamic<is_serialize>(ar, obj.owned_provinces);
        ::deser_dynamic<is_serialize>(ar, obj.controlled_provinces);
        ::deser_dynamic<is_serialize>(ar, obj.current_policy);
        ::deser_dynamic<is_serialize>(ar, obj.diplomatic_timer);
        ::deser_dynamic<is_serialize>(ar, obj.inbox);
        ::deser_dynamic<is_serialize>(ar, obj.client_hints);
        ::deser_dynamic<is_serialize>(ar, obj.ideology_id);
        ::deser_dynamic<is_serialize>(ar, obj.research);
        ::deser_dynamic<is_serialize>(ar, obj.focus_tech_id);
    }
};
