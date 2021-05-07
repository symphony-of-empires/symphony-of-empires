#ifndef POP_H
#define POP_H
#include <vector>
#include <string>

class PopType {
public:
	std::string name;
	float average_budget;
};

class Culture {
public:
	std::string name;
};

class Religion {
public:
	std::string name;
};

// placeholder
class Party;

// parties themselves will be in a different file
class PartyLoyalty {
public:
	Party * party;
	float loyalty;
};

class Issue;

// above applies
class IssueInterest {
public:
	Issue * issue;
	float interest;
};

class Pop {
public:
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