#ifndef ECONOMY_H
#define ECONOMY_H

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>
#include "texture.hpp"

class Company {
public:
	std::string name;
	uint64_t money;
	bool is_transport;
	bool is_retailer;
	bool is_industry;
	std::vector<size_t> operating_provinces;

	bool in_range(size_t prov_id) {
		// Check that province is in our operational list
		for(const auto& op: this->operating_provinces) {
			if(op == prov_id)
				return true;
		}
		return false;
	}

	void name_gen() {
		size_t r = (rand() % 12) + 1;
		for(size_t i = 0; i < r; i++) {
			this->name += 'a' + (rand() % 25);
		}
	}
};

class Good {
public:
	std::string name;
	std::string ref_name;
	bool is_edible;
	Texture * icon = new Texture();
};

class IndustryType {
public:
	std::string name;
	std::string ref_name;
	std::vector<size_t> inputs;
	std::vector<size_t> outputs;
	Texture * image = new Texture();
};

#include <deque>
class Product {
public:
	// Onwer (companyId) of this product
	size_t owner_id;
	
	// Origin province (where this product was made)
	size_t origin_id;
	
	// Industry in province that made this product
	size_t industry_id;
	
	// Good that this product is based on
	size_t good_id;
	
	// Price of the product
	float price;
	
	// Velocity of change of price of the product
	float price_vel;
	
	// Quality of the product
	size_t quality;
	
	// Total supply (worldwide) of the product
	size_t supply;
	
	// Total demand (worldwide) of the product
	size_t demand;

	// Deque for charts and stuff
	std::deque<float> price_history;
	std::deque<size_t> supply_history;
	std::deque<size_t> demand_history;
};

class World;
class Industry {
public:
	size_t get_id(World * world, size_t province_id);
	bool can_do_output(World * world);
	void add_to_stock(World * world, size_t good_id, size_t add);
	
	size_t owner_id;
	
	size_t type_id;
	
	size_t ticks_unoperational;

	float production_cost;
	
	std::vector<size_t> stockpile;
	
	// Used for faster lookups
	std::vector<size_t> output_products;
};

#endif
