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
class Serializer<PopType> : public SerializerMemcpy<PopType> {};

typedef uint16_t CultureId;
class Culture {
public:
	std::string name;
	std::string ref_name;
};
template<>
class Serializer<Culture> : public SerializerMemcpy<Culture> {};

typedef uint8_t ReligionId;
class Religion {
public:
	std::string name;
	std::string ref_name;
};
template<>
class Serializer<Religion> : public SerializerMemcpy<Religion> {};

// Placeholder
class Party;

// Parties themselves will be in a different file
class PartyLoyalty {
public:
	size_t party_id;
	float loyalty;
};
template<>
class Serializer<PartyLoyalty> : public SerializerMemcpy<PartyLoyalty> {};

class Issue;

// Above applies
class IssueInterest {
public:
	size_t issue_id;
	float interest;
};
template<>
class Serializer<IssueInterest> : public SerializerMemcpy<IssueInterest> {};

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
public:
	static inline void serialize(Archive& output, Pop const& obj) {
		::serialize(output, obj.size);
		::serialize(output, obj.unemployed);

		::serialize(output, obj.literacy);
		::serialize(output, obj.militancy);
		::serialize(output, obj.consciousness);
		::serialize(output, obj.budget);

		::serialize(output, obj.life_needs_met);
		::serialize(output, obj.everyday_needs_met);
		::serialize(output, obj.luxury_needs_met);

		::serialize(output, obj.type_id);
		::serialize(output, obj.culture_id);
		::serialize(output, obj.religion_id);

		::serialize(output, obj.party_loyalties);
		::serialize(output, obj.issue_interests);
	}
	static inline void deserialize(Archive& input, Pop& obj) {
		::deserialize(input, obj.size);
		::deserialize(input, obj.unemployed);
		
		::deserialize(input, obj.literacy);
		::deserialize(input, obj.militancy);
		::deserialize(input, obj.consciousness);
		::deserialize(input, obj.budget);

		::deserialize(input, obj.life_needs_met);
		::deserialize(input, obj.everyday_needs_met);
		::deserialize(input, obj.luxury_needs_met);

		::deserialize(input, obj.type_id);
		::deserialize(input, obj.culture_id);
		::deserialize(input, obj.religion_id);

		::deserialize(input, obj.party_loyalties);
		::deserialize(input, obj.issue_interests);
	}
};

#endif
