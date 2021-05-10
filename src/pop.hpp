#ifndef POP_H
#define POP_H
#include <vector>
#include <string>

class PopType {
public:
	std::string name;
	std::string ref_name;
	std::string on_tick_fn;
	float average_budget;
};

class Culture {
public:
	std::string name;
	std::string ref_name;
};

class Religion {
public:
	std::string name;
	std::string ref_name;
};

// Placeholder
class Party;

// Parties themselves will be in a different file
class PartyLoyalty {
public:
	size_t party_id;
	float loyalty;
};

class Issue;

// Above applies
class IssueInterest {
public:
	size_t issue_id;
	float interest;
};

class Pop {
public:
	uint size;

	float unemployed;
	float literacy;

	float militancy;
	float consciousness;
	
	float budget;
	
	float life_needs_met;
	float everyday_needs_met;
	float luxury_needs_met;
	
	PopType * type;
	Culture * culture;
	Religion * religion;

	std::vector<PartyLoyalty> party_loyalties;
	std::vector<IssueInterest> issue_interests;
};

#endif