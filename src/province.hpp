#ifndef PROVINCE_H
#define PROVINCE_H
#include "pop.hpp"

struct Province {
	std::string name;
	std::string ref_name;
	uint32_t color;
	
	uint64_t population;
	float budget;

	std::vector<Industry> industries;

	std::vector<Product> products;

    std::vector<Pop> pops;
};

#endif