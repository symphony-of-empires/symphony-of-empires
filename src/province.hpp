#ifndef PROVINCE_H
#define PROVINCE_H

#include <stdint.h>
#include <string>
#include "pop.hpp"
#include "economy.hpp"

class World;
class Province {
public:
	void add_industry(World * world, Industry * industry);

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