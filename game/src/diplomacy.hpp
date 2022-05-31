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
//      diplomacy.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include "eng3d/entity.hpp"
#include "eng3d/string.hpp"

#include "policy.hpp"

class Nation;
class Province;

namespace Diplomacy {
    // Determines if the other nation is a friendly potential ally
    inline bool is_friend(Nation& us, Nation& them);
    
    // Determines if the other nation is an enemy and potential rival
    inline bool is_foe(Nation& us, Nation& them);
};

enum class TreatyClauseType {
    MONEY,
    HUMILIATE,
    LIBERATE_NATION,
    IMPOSE_POLICIES,
    ANNEX_PROVINCES,
    CEASEFIRE,
    PUPPET,
    //TECHNOLOGY,
};

namespace TreatyClause {
    class BaseClause {
    public:
        BaseClause() = default;
        BaseClause(Nation& _sender, Nation& _receiver) {
            sender = &_sender;
            receiver = &_receiver;
        };
        virtual ~BaseClause() {};

        enum TreatyClauseType type;
        Nation* sender = nullptr; // Who created this clause
        Nation* receiver = nullptr; // Who should accept/reject this clause
        size_t days_duration = 0; // Days this clause lasts
        bool done = false; // Used for 1 time clauses

        // Function to determine the "political" cost of this clause, and how much willing the AI
        // is to accept this clause, this is only used by the AI
        virtual unsigned cost(void) {
            return 0;
        };

        // Function to enforce the policy per day (or higher time spans)
        virtual void enforce(void) {};

        // Determines whenever the clause is in effect or not, when it is not in effect
        // then it's removed permanently
        virtual bool in_effect(void) const {
            return false;
        };
    };

    // Makes loser to pay war reparations to the winner
    class WarReparations: public BaseClause {
    public:
        WarReparations(): BaseClause() {
            type = TreatyClauseType::MONEY;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void) const;

        float amount = 0.f;
    };

    // Reduces prestige of loser and increments prestige from winner
    class Humiliate: public BaseClause {
    public:
        Humiliate(): BaseClause() {
            type = TreatyClauseType::HUMILIATE;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void) const;

        float amount = 0.f;
    };

    // Liberates a nation from another
    class LiberateNation: public BaseClause {
    public:
        LiberateNation(): BaseClause() {
            type = TreatyClauseType::LIBERATE_NATION;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void) const;

        Nation* liberated = nullptr;
        std::vector<Province*> provinces;
    };

    // Imposes a policy to be put in action on a nation
    class ImposePolicies: public BaseClause {
    public:
        ImposePolicies(): BaseClause() {
            type = TreatyClauseType::IMPOSE_POLICIES;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void) const;

        Policies imposed;
    };

    // Annexes territory from the loser
    class AnnexProvince: public BaseClause {
    public:
        AnnexProvince(): BaseClause() {
            type = TreatyClauseType::ANNEX_PROVINCES;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void) const;

        std::vector<Province*> provinces;
    };

    // Calls for a ceasefire
    class Ceasefire: public BaseClause {
    public:
        Ceasefire(): BaseClause() {
            type = TreatyClauseType::CEASEFIRE;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void) const;
    };

    // Makes the receiver the puppet of the sender
    class Puppet: public BaseClause {
    public:
        Puppet(): BaseClause() {
            type = TreatyClauseType::PUPPET;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void) const;
    };
};

enum class TreatyApproval {
    UNDECIDED,
    ACCEPTED,
    DENIED,
    ABSENT,
};

class Treaty : public IdEntity<uint16_t> {
public:
    bool does_participate(Nation& nation);
	bool in_effect(void) const;

    Eng3D::String name;
    Nation* sender; // The one who sent the treaty
    Nation* receiver; // The one who is going to receive this treaty
    // Clauses of this treaty;
    std::vector<TreatyClause::BaseClause*> clauses;
    // List of who are involved in the treaty
    std::vector<std::pair<Nation*, TreatyApproval>> approval_status;
};

class Unit;
class War;
class Battle : public IdEntity<uint16_t> {
public:
    Battle(War& war, Province& province)
        : war{ &war },
        province{ &province }
    {

    }
    ~Battle() {};

    Eng3D::String name;
    
    War* war = nullptr;
    Province* province = nullptr;
    Eng3D::Number attacker_casualties = 0;
    Eng3D::Number defender_casualties = 0;
    std::vector<Unit*> attackers;
    std::vector<Unit*> defenders;
};

class War : public IdEntity<uint16_t> {
public:
    War() {};
    ~War() {};
    bool is_involved(const Nation& nation) const;
    bool is_attacker(const Nation& nation) const;
    bool is_defender(const Nation& nation) const;

    Eng3D::String name;
    std::vector<Nation*> attackers, defenders;
    std::vector<TreatyClause::BaseClause*> wargoals;
    std::vector<Battle> battles;
};