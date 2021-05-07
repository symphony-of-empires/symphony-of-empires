#ifndef ECONOMY_H
#define ECONOMY_H

#include <stdint.h>
#include <stddef.h>
#include <string>

class Company {
public:
	uint64_t money;
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

	size_t * inputs;
	size_t n_inputs;

	size_t * outputs;
	size_t n_outputs;
};

class Industry {
public:
	size_t owner_id;
	size_t type_id;
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

class PopType {
	std::string name;
	std::string ref_name;
};

#endif