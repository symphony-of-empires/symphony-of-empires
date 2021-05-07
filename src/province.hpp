#ifndef PROVINCE_H
#define PROVINCE_H
#include "pop.hpp"

struct Province {
	std::string name;
	std::string ref_name;
	uint32_t color;
	
	uint64_t population;
	float budget;

	Industry * industries;
	size_t n_industries;

	Product * products;
	size_t n_products;

    std::vector<Pop> pops;
};

#endif