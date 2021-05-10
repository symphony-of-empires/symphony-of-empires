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
	void set_pop_size(Pop * pop, uint size);

	std::string name;
	std::string ref_name;
	uint32_t color;
	
	/* readonly */ uint64_t population;
	float budget;

	size_t n_tiles = 0;
	size_t min_x = 65532;
	size_t min_y = 65532;
	size_t max_x = 0;
	size_t max_y = 0;
	
	std::vector<Industry> industries;
	std::vector<Product> products;
    std::vector<Pop> pops;
};

#endif