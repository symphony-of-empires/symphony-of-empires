#ifndef POP_H
#define POP_H
#include <vector>

struct PopType {
    char * name;
    float average_budget;
};

struct Culture {
    char * name;
};

struct Religion {
    char * name;
};

// placeholder
struct Party;

// parties themselves will be in a different file
struct PartyLoyalty {
    Party * party;
    float loyalty;
};

struct Issue;

// above applies
struct IssueInterest {
    Issue * issue;
    float interest;
};

struct Pop {
    unsigned int size;
    float militancy;
    float consciousness;
    float unemployment;
    float budget;
    float life_needs_met;
    float everyday_needs_met;
    float luxury_needs_met;
    float literacy;
    
    PopType * type;
    Culture * culture;
    Religion * religion;

    std::vector<PartyLoyalty> party_loyalties;
    std::vector<IssueInterest> issue_interests;
    

};

#endif