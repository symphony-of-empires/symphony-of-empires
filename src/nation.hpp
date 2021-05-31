#ifndef NATION_H
#define NATION_H

#include <stdint.h>
#include <string>
#include <vector>
#include "texture.hpp"
#include "province.hpp"

typedef uint16_t NationId;

/**
 * Defines a one side relation between a country
 * This allows for cases where a country A hates country B, but country B loves country A
 */
class NationRelation {
public:
	float relation;
	// Interest of a nation on this nation
	float interest;

	// Whetever commercial operations are allowed on on the target country
	bool has_embargo;
	bool has_war;
	bool has_alliance;
	bool has_defensive_pact;
	bool has_truce;
	bool has_embassy;
	bool has_military_access;
	bool has_market_access;
};
template<>
class Serializer<NationRelation> : public SerializerMemcpy<NationRelation> {};

class Policies {
public:
	enum MigrationPolicy {
		// Nobody can exit the country
		MIGRATION_NOBODY,

		// Only accepted cultures can exit the country
		MIGRATION_ACCEPTED_CULTURES,

		// Everyone can exit, but only if they pay
		MIGRATION_ALL_PAYMENT,

		// Everyone can exit freely
		MIGRATION_ALL,
	};
	MigrationPolicy migration;

	enum ImmigrationPolicy {
		// Nobody can enter the country
		IMMIGRATION_NOBODY,

		// Only accepted cultures can enter the country
		IMMIGRATION_ACCEPTED_CULTURES,

		// Everyone can enter, but at a cost
		IMMIGRATION_ALL_PAYMENT,

		// Everyone can exit freely
		IMMIGRATION_ALL,
	};
	ImmigrationPolicy immigration;

	enum CensorshipPolicy {
		// All media censored
		CENSORSHIP_ALL_CENSORED,

		// Only state run media allowed
		CENSORSHIP_ONLY_STATE,

		// Only "approved" media is allowed
		CENSORSHIP_ONLY_APPROVED,

		// All media allowed
		CENSORSHIP_ALL_ALLOWED,
	};
	CensorshipPolicy censorship;

	enum AutoBuildPolicy {
		// POPs cannot auto build stuff
		AUTO_BUILD_NONE,

		// POPs can only build with approval
		AUTO_BUILD_ONLY_APPROVED,

		// All POPs can build freely
		AUTO_BUILD_ALLOWED,
	};

	// Whetever POPs are able to build infrastructure
	AutoBuildPolicy auto_build_infrastructure;

	// Whatever POPs are able to build factories
	AutoBuildPolicy auto_build_factories;

	bool national_id;

	bool men_suffrage;
	bool men_labour;
	
	bool women_suffrage;
	bool women_labour;

	bool private_property;
	bool public_property;
	bool companies_allowed;

	bool public_education;
	bool private_education;
	bool secular_education;

	bool public_healthcare;
	bool private_healthcare;

	bool social_security;

	// Is slavery allowed?
	bool slavery;

	// Tax for importing products
	float import_tax;

	// Tax for exporting products
	float export_tax;

	// Flat tax imposed on the low-wage pops
	float poor_flat_tax;

	// Flat tax imposed on the medium-wage pops
	float med_flat_tax;

	// Flat tax imposed on the high-wage pops
	float high_flat_tax;
};
template<>
class Serializer<Policies> : public SerializerMemcpy<Policies> {};

#include "event.hpp"
class Nation {
public:
	Nation() {};
	Nation& operator=(const Nation&) = default;
	~Nation() {
		delete default_flag;
	};

	// Whetever thet nation exists at all - we cannot add nations in-game
	// so we just check if the nation "exists" at all, this means that it has
	// a presence and a goverment, must own atleast 1 province
	bool exists;

	// Whetever this nation is controlled by AI
	bool controlled_by_ai;

	// Default transleted/display name
	std::string name;

	// Default reference name
	std::string ref_name;

	// Default color of the country
	uint32_t color;

	// A list with relations with all other nations, mapped 1:1 to the Nation list in the world
	std::vector<NationRelation> relations;

	// Id of the nation that has us on their sphere of influence
	size_t spherer_id;

	// Number of diplomacy points available
	float diplomacy_points;

	// Total number of prestige
	float prestige;

	// Base literacy applied to all pops
	float base_literacy;

	bool is_civilized;

	// Level of infamy
	float infamy;

	// 3 key scores used to define a nation's minimum prestige, how willing would the AI
	// be to challenge this nations and other valuable stuff
	float military_score = 0.f;
	float naval_score = 0.f;
	float economy_score = 0.f;

	// Total budget of the nation (money in ark), this is not equal to GDP, the GDP is the total sum of the price
	// of all products in the nation, which are volatile unless they are sold
	float budget;

	// Primary culture of this nation, may also be changed via events, otherwise it's permanent
	Culture * primary_culture;

	// Accepted cultures in this nation, the accepted cultures may have some bonuses on provinces *totally*
	// owned by this nation
	std::vector<Culture *> accepted_cultures;

	// List of provinces which are owned by this nation (including partial ownership)
	std::vector<Province *> owned_provinces;

	// List of neighbouring nations
	std::vector<Nation *> neighbours;

	// A pointer to a class defining the current policy of this nation
	Policies * current_policy;

	// Inbox of the nation; events that require our attention / should be processed
	std::vector<Event *> inbox;

	// Default flag texture of the country
	Texture * default_flag = nullptr;
};

#endif
