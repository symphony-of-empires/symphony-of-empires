#include "province.hpp"
#include "economy.hpp"
#include "world.hpp"

/**
* Gets ID from pointer
 */
ProvinceId Province::get_id(const World& world) {
    const std::vector<Province *>* provinces = &world.provinces;
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
void Province::add_industry(World& world, Industry* industry) {
    IndustryType* type = industry->type;

    // Add a product for each output
    for(const auto& output: type->outputs) {
        Product* new_product = new Product();
        new_product->industry = industry;
        new_product->good = output;
        new_product->owner = industry->owner;
        new_product->demand_history.clear();
        new_product->supply_history.clear();
        new_product->price_history.clear();
        new_product->origin = this;

        industry->output_products.push_back(new_product);
        world.products.push_back(new_product);
    }

    // We will set inputs_satisfied to same size as inputs
    // Industries start with 100 of stockpiles
    industry->stockpile.clear();
    for(size_t i = 0; i < industry->type->inputs.size(); i++) {
        industry->stockpile.push_back(100);
    }

    industries.push_back(*industry);
}

/** Removes an industry and their products from the entire world
 * this is only used when industries go bankrupt!
  */
void Province::remove_industry(World& world, Industry* industry) {
    // Remove products of this industry from world market
    for(ProductId i = 0; i < world.products.size(); i++) {
        Product* product = world.products[i];
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
