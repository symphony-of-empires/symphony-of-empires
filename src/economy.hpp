#ifndef ECONOMY_H
#define ECONOMY_H

#include <stdint.h>
#include <stddef.h>
#include <string>

typedef struct {
	uint64_t money;
}Company;

typedef struct {
	std::string name;
	std::string ref_name;
}Good;

typedef struct {
	std::string name;
	std::string ref_name;

	size_t * inputs;
	size_t n_inputs;

	size_t * outputs;
	size_t n_outputs;
}IndustryType;

typedef struct {
	size_t owner_id;
	size_t type_id;
}Industry;

typedef struct {
	size_t owner_id;
	size_t industry_id;
	size_t good_id;
	
	float price;
	float price_vel;

	size_t supply;
	size_t demand;
}Product;

#endif