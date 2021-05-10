#include "province.hpp"
#include "world.hpp"

void Province::add_industry(World * world, Industry * industry) {
	IndustryType * type = &world->industry_types[industry->type_id];
	for(const auto& output: type->outputs) {
		const size_t good_id = output;

		// Check that product is not already in the province
		int is_here = 0;
		const unsigned int n_products = this->products.size();
		for(size_t j = 0; j < n_products; j++) {
			if(this->products[j].owner_id == industry->owner_id) {
				is_here = 1;
				break;
			}
		}
		if(is_here) break;

		// Otherwise add it to the province product list
		Product new_product;
		memset(&new_product, 0, sizeof(Product));
		new_product.industry_id = this->industries.size();
		new_product.good_id = good_id;
		new_product.owner_id = industry->owner_id;

		// Find id of this province
		size_t province_id = (size_t)-1;
		for(size_t i = 0; i < world->provinces.size(); i++) {
			if(world->provinces[i].ref_name != this->ref_name)
				continue;
			province_id = i;
			break;
		}
		new_product.origin_id = province_id;

		world->products.push_back(new_product);
	}

	// We will set inputs_satisfied to same size as inputs
	for(size_t i = 0; i < world->industry_types[industry->type_id].inputs.size(); i++) {
		industry->stockpile.push_back(0);
	}

	this->industries.push_back(*industry);
}
