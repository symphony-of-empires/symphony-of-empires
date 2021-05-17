#ifndef POP_H
#define POP_H
#include <vector>
#include <string>

enum {
	POP_TYPE_ENTRPRENEUR = 0,
	POP_TYPE_ARTISAN = 1,
	POP_TYPE_CRAFTSMEN = 2,
	POP_TYPE_BUREAUCRAT = 3,
	POP_TYPE_ARISTOCRAT = 4,
	POP_TYPE_CLERGYMEN = 5,
	POP_TYPE_FARMER = 6,
	POP_TYPE_SOLDIER = 7,
	POP_TYPE_OFFICER = 8,
	POP_TYPE_LABORER = 9,
	POP_TYPE_SLAVE = 10
};

class PopType {
public:
	std::string name;
	std::string ref_name;
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
	size_t size;
	size_t unemployed;
	float literacy;

	float militancy;
	float consciousness;
	
	float budget;
	
	float life_needs_met;
	float everyday_needs_met;
	float luxury_needs_met;
	
	size_t type_id;
	size_t culture_id;
	size_t religion_id;

	std::vector<PartyLoyalty> party_loyalties;
	std::vector<IssueInterest> issue_interests;
};

#endif
