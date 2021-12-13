#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include "policy.hpp"
#include "unified_render/entity.hpp"

//#include "nation.hpp"
class Nation;
class Province;

namespace Diplomacy {
    // Determines if the other nation is a friendly potential ally
    inline bool is_friend(Nation* us, Nation* them);

    // Determines if the other nation is an enemy and potential rival
    inline bool is_foe(Nation* us, Nation* them);
};

enum class TreatyClauseType {
    WAR_REPARATIONS,
    HUMILIATE,
    LIBERATE_NATION,
    IMPOSE_POLICIES,
    ANEXX_PROVINCES,
    CEASEFIRE,
};

namespace TreatyClause {
    class BaseClause {
    public:
        // Type of clause
        enum TreatyClauseType type;

        // Nation who created this clause
        Nation* sender;

        // Nation who should accept/reject this clause
        Nation* receiver;

        // Number of days this clause lasts
        size_t days_duration = 0;

        // Used for 1 time clauses
        bool done = false;

        BaseClause() = default;
        BaseClause(Nation* _sender, Nation* _receiver) {
            sender = _sender;
            receiver = _receiver;
        };
        virtual ~BaseClause() {};

        // Function to determine the "political" cost of this clause, and how much willing the AI
        // is to accept this clause, this is only used by the AI
        virtual unsigned cost(void) {
            return 0;
        };

        // Function to enforce the policy per day (or higher time spans)
        virtual void enforce(void) {};

        // Determines whenever the clause is in effect or not, when it is not in effect
        // then it's removed permanently
        virtual bool in_effect(void) {
            return false;
        };
    };

    /**
    * Makes loser to pay war reparations to the winner
     */
    class WarReparations: public BaseClause {
    public:
        WarReparations(): BaseClause() {
            type = TreatyClauseType::WAR_REPARATIONS;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);

        float amount = 0.f;
    };

    /**
    * Reduces prestige of loser and increments prestige from winner
     */
    class Humiliate: public BaseClause {
    public:
        Humiliate(): BaseClause() {
            type = TreatyClauseType::HUMILIATE;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);

        float amount = 0.f;
    };

    /**
    * Liberates a nation from another
     */
    class LiberateNation: public BaseClause {
    public:
        LiberateNation(): BaseClause() {
            type = TreatyClauseType::LIBERATE_NATION;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);

        Nation* liberated = nullptr;
        std::vector<Province*> provinces;
    };

    /**
    * Imposes a policy to be put in action on a nation
     */
    class ImposePolicies: public BaseClause {
    public:
        ImposePolicies(): BaseClause() {
            type = TreatyClauseType::IMPOSE_POLICIES;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);

        Policies imposed;
    };

    /**
    * Anexxes territory from the loser
     */
    class AnexxProvince: public BaseClause {
    public:
        AnexxProvince(): BaseClause() {
            type = TreatyClauseType::ANEXX_PROVINCES;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);

        std::vector<Province*> provinces;
    };

    /**
    * Calls for a ceasefire
     */
    class Ceasefire: public BaseClause {
    public:
        Ceasefire(): BaseClause() {
            type = TreatyClauseType::CEASEFIRE;
        };
        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);
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
    bool does_participate(Nation* nation);

    std::string name;
    std::vector<TreatyClause::BaseClause*> clauses;

    Nation* receiver, * sender;

    // List of who are involved in the treaty
    std::vector<std::pair<Nation*, TreatyApproval>> approval_status;
};

class War : public IdEntity<uint16_t> {
public:
    std::string name;
    std::vector<Nation*> attackers;
    std::vector<Nation*> defenders;

    std::vector<TreatyClause::BaseClause*> wargoals;
};