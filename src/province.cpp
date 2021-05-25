#include "province.hpp"
#include "economy.hpp"
#include "world.hpp"

Province::~Province() {
	for(auto& industry: this->industries) {
		delete industry;
	} for(auto& product: this->products) {
		delete product;
	} for(auto& pop: this->pops) {
		delete pop;
	}
}

/**
 * Gets ID from pointer
 */
ProvinceId Province::get_id(const World& world) {
	const std::vector<Province *> * provinces = &world.provinces;
	const auto province = std::find(provinces->begin(), provinces->end(), this);
	if(province != provinces->end()) {
		return (ProvinceId)std::distance(provinces->begin(), province);
	}
	return (ProvinceId)-1;
}

/**
 * Adds a new industry in the province and adds it's output
 * products into the world accordingly
 */
void Province::add_industry(World * world, Industry * industry) {
	IndustryType * type = world->industry_types[industry->type_id];
	for(const auto& output: type->outputs) {
		const GoodId good_id = output;
		
		// Check that product is not already in the province
		int is_here = 0;
		const unsigned int n_products = this->products.size();
		for(ProductId j = 0; j < n_products; j++) {
			if(this->products[j]->owner_id == industry->owner_id) {
				is_here = 1;
				break;
			}
		}
		if(is_here)
			break;
		
		// Otherwise add it to the province product list
		Product * new_product = new Product();
		new_product->industry_id = this->industries.size();
		new_product->good_id = good_id;
		new_product->owner_id = industry->owner_id;

		// Find id of this province
		ProvinceId province_id = (ProvinceId)-1;
		for(ProvinceId i = 0; i < world->provinces.size(); i++) {
			if(world->provinces[i]->ref_name != this->ref_name)
				continue;
			
			province_id = i;
			break;
		}

		new_product->demand_history.clear();
		new_product->supply_history.clear();
		new_product->price_history.clear();
		
		// Add the product to the world
		new_product->origin_id = province_id;
		world->products.push_back(new_product);
		
		industry->output_products.push_back(world->products.size() - 1);
	}

	// We will set inputs_satisfied to same size as inputs
	// Industries start with 100 of stockpiles
	industry->stockpile.clear();
	for(IndustryTypeId i = 0; i < world->industry_types[industry->type_id]->inputs.size(); i++) {
		industry->stockpile.push_back(100);
	}

	this->industries.push_back(industry);
}

/** Removes an industry and their products from the entire world
  * this is only used when industries go bankrupt!
  */
void Province::remove_industry(World * world, Industry * industry) {
	ProvinceId province_id = this->get_id(*world);
	IndustryId industry_id = industry->get_id(*world, province_id);

	// Remove products of this industry from world market
	for(ProductId i = 0; i < world->products.size(); i++) {
		Product * product = world->products[i];
		if(product->origin_id == province_id
		&& product->industry_id == industry_id) {
			delete world->products[i];
			world->products[i] = nullptr;
			continue;
		}
	}

	// Remove this industry totally
	delete this->industries[industry_id];
	this->industries[industry_id] = nullptr;

	// We have removed the industry!
}
