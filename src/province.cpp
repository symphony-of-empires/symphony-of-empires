#include "province.hpp"
#include "economy.hpp"
#include "world.hpp"

Province::~Province() {
	for(auto& product: this->products) {
		delete product;
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
void Province::add_industry(World& world, Industry * industry) {
	IndustryType * type = industry->type;
	
	industries.push_back(*industry);
	
	for(const auto& output: type->outputs) {
		// Check that product is not already in the province
		int is_here = 0;
		const unsigned int n_products = this->products.size();
		for(ProductId j = 0; j < n_products; j++) {
			if(this->products[j]->owner == industry->owner) {
				is_here = 1;
				break;
			}
		}
		if(is_here)
			break;
		
		// Otherwise add it to the province product list
		this->industries.resize(this->industries.size() + 1);

		Product * new_product = new Product();
		new_product->industry = industry;
		new_product->good = output;
		new_product->owner = industry->owner;

		new_product->demand_history.clear();
		new_product->supply_history.clear();
		new_product->price_history.clear();
		
		// Add the product to the world
		new_product->origin = this;
		world.products.push_back(new_product);
		
		industry->output_products.push_back(new_product);
	}

	// We will set inputs_satisfied to same size as inputs
	// Industries start with 100 of stockpiles
	industry->stockpile.clear();
	for(size_t i = 0; i < industry->type->inputs.size(); i++) {
		industry->stockpile.push_back(100);
	}
}

/** Removes an industry and their products from the entire world
  * this is only used when industries go bankrupt!
  */
void Province::remove_industry(World& world, Industry * industry) {
	// Remove products of this industry from world market
	for(ProductId i = 0; i < world.products.size(); i++) {
		Product * product = world.products[i];
		if(product->origin == this
		&& product->industry == industry) {
			delete product;
			world.products[i] = nullptr;
			continue;
		}
	}

	// Remove this industry totally
	size_t industry_id = world.get_id(*this, industry);
	industries.erase(industries.begin() + industry_id);

	// We have removed the industry!
}

size_t Province::total_pops(void) const {
	size_t total = 0;
	for(const auto& pop: pops) {
		total += pop.size;
	}
	return total;
}