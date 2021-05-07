#include "province.hpp"
#include "world.hpp"

void Province::add_industry(World * world, Industry * industry) {
	IndustryType * type = &world->industry_types[industry->type_id];
	for(size_t i = 0; i < type->n_outputs; i++) {
		size_t good_id = type->outputs[i];


		/* check that product is not already here */
		int is_here = 0;
		uint n_products = this->products.size();
		for(size_t j = 0; j < n_products; j++) {
			if(this->products[j].owner_id == industry->owner_id) {
				is_here = 1;
				break;
			}
		}
		if(is_here) break;

		/* otherwise add it to the province product list */
		Product new_product;
		memset(&new_product, 0, sizeof(Product));
		new_product.industry_id = this->industries.size();
		new_product.good_id = good_id;
		new_product.owner_id = industry->owner_id;
		this->products.push_back(new_product);
	}
	this->industries.push_back(*industry);
}