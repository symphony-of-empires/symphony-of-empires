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
	std::vector<float> relations;
	std::vector<size_t> operating_provinces;
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

class Industry {
public:
	size_t owner_id;
	size_t type_id;
	std::vector<size_t> stockpile;
};

class Product {
public:
	size_t owner_id;
	size_t industry_id;
	size_t good_id;
	float price;
	float price_vel;
	size_t supply;
	size_t demand;
};

#endif