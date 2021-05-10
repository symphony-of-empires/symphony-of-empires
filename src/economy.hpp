#ifndef ECONOMY_H
#define ECONOMY_H

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

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
};

class Good {
public:
	std::string name;
	std::string ref_name;
};

class IndustryType {
public:
	std::string name;
	std::string ref_name;
	std::vector<size_t> inputs;
	std::vector<size_t> outputs;
};

class World;
class Industry {
public:
	size_t owner_id;
	size_t type_id;
	std::vector<size_t> stockpile;

	bool can_do_output();
	void add_to_stock(World * world, size_t good_id, size_t add);
};

class Product {
public:
	size_t owner_id; // Onwer (companyId) of this product
	size_t origin_id; // Origin province (where this product was made)
	size_t industry_id; // Industry in province that made this product
	size_t good_id; // Good that this product is based on
	float price; // Price of the product
	float price_vel; // Velocity of change of price of the product
	size_t quality; // Quality of the product
	size_t supply;
	size_t demand;
};

#endif