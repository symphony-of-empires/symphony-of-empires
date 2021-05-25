#ifndef ECONOMY_H
#define ECONOMY_H

#include <algorithm>
#include <string>
#include <vector>
#include <deque>
#include <cstdint>
#include <cstddef>

#include "texture.hpp"
#include "array_ops.hpp"
#include "province.hpp"

namespace Economy {
	void do_phase_1(World& world);
	void do_phase_2(World& world);
	void do_phase_3(World& world);
	void do_phase_4(World& world);
}

typedef uint16_t CompanyId;
class Company {
public:
	std::string name;
	uint64_t money;
	bool is_transport;
	bool is_retailer;
	bool is_industry;
	std::vector<ProvinceId> operating_provinces;

	inline bool in_range(ProvinceId prov_id) {
		return (std::find(this->operating_provinces.begin(), this->operating_provinces.end(), prov_id)
			!= this->operating_provinces.end());
	}

	void name_gen() {
		size_t r = (rand() % 12) + 1;
		for(size_t i = 0; i < r; i++) {
			this->name += 'a' + (rand() % 25);
		}
	}
};

typedef uint8_t GoodId;
class Good {
public:
	~Good() {
		delete icon;
	}

	std::string name;
	std::string ref_name;
	bool is_edible;
	Texture * icon = nullptr;
};

typedef uint8_t IndustryTypeId;
class IndustryType {
public:
	~IndustryType() {
		delete image;
	}

	std::string name;
	std::string ref_name;
	std::vector<size_t> inputs;
	std::vector<size_t> outputs;
	Texture * image = nullptr;
};

class World;

typedef uint16_t IndustryId;
class Industry {
public:
	size_t get_id(const World& world, size_t province_id);
	bool can_do_output(const World& world);
	void add_to_stock(const World& world, size_t good_id, size_t add);
	
	size_t owner_id;
	
	size_t type_id;
	
	size_t ticks_unoperational;

	float production_cost;
	
	std::vector<size_t> stockpile;
	
	// Used for faster lookups
	std::vector<size_t> output_products;
};

typedef uint16_t ProductId;
class Product {
public:
	// Onwer (companyId) of this product
	CompanyId owner_id;
	
	// Origin province (where this product was made)
	ProvinceId origin_id;
	
	// Industry in province that made this product
	IndustryId industry_id;
	
	// Good that this product is based on
	GoodId good_id;
	
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

#endif
