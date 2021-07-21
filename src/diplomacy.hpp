#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include "nation.hpp"

namespace Diplomacy {
    // Determines if the other nation is a friendly potential ally
    bool is_friend(Nation* us, Nation* them);

    // Determines if the other nation is an enemy and potential rival
    bool is_foe(Nation* us, Nation* them);
};

enum TreatyClauseType {
    TREATY_CLAUSE_WAR_REPARATIONS,
    TREATY_CLAUSE_HUMILIATE,
    TREATY_CLAUSE_LIBERATE_NATION,
    TREATY_CLAUSE_IMPOSE_POLICIES,
    TREATY_CLAUSE_ANEXX_PROVINCES,
    TREATY_CLAUSE_CEASEFIRE,
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

        // Amount to pay per day
        float amount = 0;

        // Number of days this clause lasts
        size_t days_duration = 0;

        // Applies only to "Liberate nation" clauses
        Nation* liberated = nullptr;
        std::vector<Province *> provinces;

        // For "Impose policy" clauses
        Policies imposed;

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
        virtual unsigned cost(void) { return 0; };

        // Function to enforce the policy per day (or higher time spans)
        virtual void enforce(void) {};

        // Determines whenever the clause is in effect or not, when it is not in effect
        // then it's removed permanently
        virtual bool in_effect(void) { return false; };
    };
    
    /**
    * Makes loser to pay war reparations to the winner
     */
    class WarReparations : public BaseClause {
    public:
        enum TreatyClauseType type = TREATY_CLAUSE_WAR_REPARATIONS;

        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);
    };
    
    /**
    * Reduces prestige of loser and increments prestige from winner
     */
    class Humiliate : public BaseClause {
    public:
        enum TreatyClauseType type = TREATY_CLAUSE_HUMILIATE;

        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);
    };
    
    /**
    * Liberates a nation from another
     */
    class LiberateNation : public BaseClause {
    public:
        enum TreatyClauseType type = TREATY_CLAUSE_LIBERATE_NATION;

        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);
    };
    
    /**
    * Imposes a policy to be put in action on a nation
     */
    class ImposePolicies : public BaseClause {
    public:
        enum TreatyClauseType type = TREATY_CLAUSE_IMPOSE_POLICIES;

        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);
    };
    
    /**
    * Anexxes territory from the loser
     */
    class AnexxProvince : public BaseClause {
    public:
        enum TreatyClauseType type = TREATY_CLAUSE_ANEXX_PROVINCES;

        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);
    };
    
    /**
    * Calls for a ceasefire
     */
    class Ceasefire : public BaseClause {
    public:
        enum TreatyClauseType type = TREATY_CLAUSE_CEASEFIRE;

        unsigned cost(void);
        void enforce(void);
        bool in_effect(void);
    };
};

enum TreatyApproval {
    TREATY_APPROVAL_UNDECIDED,
    TREATY_APPROVAL_ACCEPTED,
    TREATY_APPROVAL_DENIED,
    TREATY_APPROVAL_ABSENT,
};

typedef uint32_t TreatyId;
class Treaty {
public:
    using Id = TreatyId;
    
    std::string name;
    std::vector<TreatyClause::BaseClause> clauses;

    Nation* receiver,* sender;

    // List of who are involved in the treaty
    std::vector<std::pair<Nation*, TreatyApproval>> approval_status;
};

class War {
public:
    std::string name;
    std::vector<Nation*> attackers;
    std::vector<Nation*> defenders;
};

#endif
