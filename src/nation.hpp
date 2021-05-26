#ifndef NATION_H
#define NATION_H

#include <stdint.h>
#include <string>
#include <vector>
#include "texture.hpp"
#include "province.hpp"

typedef uint16_t NationId;

class NationRelation {
public:
	float relation;
	
	// Interest of a nation on this nation
	float interest;

	bool has_embargo;
	bool has_war;
	bool has_alliance;
	bool has_defensive_pact;
	bool has_truce;
	bool has_embassy;
	bool has_military_access;
	bool has_market_access;
};

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
	AutoBuildPolicy auto_build_infrastructure;
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

class Nation {
public:
	Nation() {};
	Nation& operator=(const Nation&) = default;
	~Nation() {
		delete default_flag;
	}

	// Default transleted/display name
	std::string name;

	// Default reference name
	std::string ref_name;

	// Default color of the country
	uint32_t color;

	// Default flag texture of the country
	Texture * default_flag = nullptr;

	// Relations with all other countries
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

	float military_score = 0.f;
	float naval_score = 0.f;
	float economy_score = 0.f;

	Culture * primary_culture;
	std::vector<Culture *> accepted_cultures;

	// Used for evaluations
	std::vector<Province *> owned_provinces;
	std::vector<Nation *> neighbours;

	Policies * current_policy;
	
	// Budget of the nation
	float budget;
};

#endif
