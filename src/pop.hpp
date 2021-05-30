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

#include "serializer.hpp"

typedef uint8_t PopTypeId;
class PopType {
public:
	std::string name;
	std::string ref_name;
	float average_budget;
};
template<>
class Serializer<PopType> {
	inline void serialize(std::byte *& output, PopType const& obj) {
		Serializer<std::string>::serialize(output, obj.name);
		Serializer<std::string>::serialize(output, obj.ref_name);
		Serializer<float>::serialize(output, obj.average_budget);
	}
	inline void deserialize(std::byte const *& input, PopType& obj) {
		Serializer<std::string>::deserialize(input, obj.name);
		Serializer<std::string>::deserialize(input, obj.ref_name);
		Serializer<float>::deserialize(input, obj.average_budget);
	}
};

typedef uint16_t CultureId;
class Culture {
public:
	std::string name;
	std::string ref_name;
};
template<>
class Serializer<Culture> {
	inline void serialize(std::byte *& output, Culture const& obj) {
		Serializer<std::string>::serialize(output, obj.name);
		Serializer<std::string>::serialize(output, obj.ref_name);
	}
	inline void deserialize(std::byte const *& input, Culture& obj) {
		Serializer<std::string>::deserialize(input, obj.name);
		Serializer<std::string>::deserialize(input, obj.ref_name);
	}
};

typedef uint8_t ReligionId;
class Religion {
public:
	std::string name;
	std::string ref_name;
};
template<>
class Serializer<Religion> {
	inline void serialize(std::byte *& output, Religion const& obj) {
		Serializer<std::string>::serialize(output, obj.name);
		Serializer<std::string>::serialize(output, obj.ref_name);
	}
	inline void deserialize(std::byte const *& input, Religion& obj) {
		Serializer<std::string>::deserialize(input, obj.name);
		Serializer<std::string>::deserialize(input, obj.ref_name);
	}
};

// Placeholder
class Party;

// Parties themselves will be in a different file
class PartyLoyalty {
public:
	size_t party_id;
	float loyalty;
};
template<>
class Serializer<PartyLoyalty> {
	inline void serialize(std::byte *& output, PartyLoyalty const& obj) {
		Serializer<size_t>::serialize(output, obj.party_id);
		Serializer<float>::serialize(output, obj.loyalty);
	}
	inline void deserialize(std::byte const *& input, PartyLoyalty& obj) {
		Serializer<size_t>::deserialize(input, obj.party_id);
		Serializer<float>::deserialize(input, obj.loyalty);
	}
};

class Issue;

// Above applies
class IssueInterest {
public:
	size_t issue_id;
	float interest;
};
template<>
class Serializer<IssueInterest> {
	inline void serialize(std::byte *& output, IssueInterest const& obj) {
		Serializer<size_t>::serialize(output, obj.issue_id);
		Serializer<float>::serialize(output, obj.interest);
	}
	inline void deserialize(std::byte const *& input, IssueInterest& obj) {
		Serializer<size_t>::deserialize(input, obj.issue_id);
		Serializer<float>::deserialize(input, obj.interest);
	}
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
	
	PopTypeId type_id;
	CultureId culture_id;
	ReligionId religion_id;

	std::vector<PartyLoyalty> party_loyalties;
	std::vector<IssueInterest> issue_interests;
};
template<>
class Serializer<Pop> {
	inline void serialize(std::byte *& output, Pop const& obj) {
		Serializer<size_t>::serialize(output, obj.size);
		Serializer<size_t>::serialize(output, obj.unemployed);

		Serializer<float>::serialize(output, obj.literacy);
		Serializer<float>::serialize(output, obj.militancy);
		Serializer<float>::serialize(output, obj.consciousness);
		Serializer<float>::serialize(output, obj.budget);

		Serializer<float>::serialize(output, obj.life_needs_met);
		Serializer<float>::serialize(output, obj.everyday_needs_met);
		Serializer<float>::serialize(output, obj.luxury_needs_met);
		
		Serializer<PopTypeId>::serialize(output, obj.type_id);
		Serializer<CultureId>::serialize(output, obj.culture_id);
		Serializer<ReligionId>::serialize(output, obj.religion_id);

		Serializer<std::vector<PartyLoyalty>>::serialize(output, obj.party_loyalties);
		Serializer<std::vector<IssueInterest>>::serialize(output, obj.issue_interests);
	}
	inline void deserialize(std::byte const *& input, Pop& obj) {
		Serializer<size_t>::deserialize(input, obj.size);
		Serializer<size_t>::deserialize(input, obj.unemployed);

		Serializer<float>::deserialize(input, obj.literacy);
		Serializer<float>::deserialize(input, obj.militancy);
		Serializer<float>::deserialize(input, obj.consciousness);
		Serializer<float>::deserialize(input, obj.budget);

		Serializer<float>::deserialize(input, obj.life_needs_met);
		Serializer<float>::deserialize(input, obj.everyday_needs_met);
		Serializer<float>::deserialize(input, obj.luxury_needs_met);
		
		Serializer<PopTypeId>::deserialize(input, obj.type_id);
		Serializer<CultureId>::deserialize(input, obj.culture_id);
		Serializer<ReligionId>::deserialize(input, obj.religion_id);

		Serializer<std::vector<PartyLoyalty>>::deserialize(input, obj.party_loyalties);
		Serializer<std::vector<IssueInterest>>::deserialize(input, obj.issue_interests);
	}
};

#endif
