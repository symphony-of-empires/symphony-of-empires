#include "province.hpp"
#include "world.hpp"
#include "product.hpp"
#include "industry.hpp"
#include "good.hpp"
#include "company.hpp"

/**
* Gets ID from pointer
 */
Province::Id Province::get_id(const World& world) {
    const std::vector<Province *>* provinces = &world.provinces;
    const auto province = std::find(provinces->begin(), provinces->end(), this);
    if(province != provinces->end()) {
        return (Province::Id)std::distance(provinces->begin(), province);
    }
    return (Province::Id)-1;
}

/**
 * Obtains the country that currently has a larger number of
 * tiles controlled from this province
 */
Nation& Province::get_occupation_controller(const World& world) const {
    std::vector<Nation::Id> nations_cnt;
    std::set<Nation::Id> unique_nations;

    for(size_t x = min_x; x < max_x; x++) {
        for(size_t y = min_y; y < max_y; y++) {
            nations_cnt.push_back(world.get_tile(x, y).owner_id);
        }
    }

    //unique_nations.insert(nations_cnt);
    for(const auto& nation_id: nations_cnt) {
        unique_nations.insert(nation_id);
    }

    Nation::Id nation_id = world.get_id(owner);
    size_t max_tiles_cnt = 0;
    for(const auto& curr_nation_id: unique_nations) {
        size_t tiles_cnt = 0;
        for(const auto& tile_owner: nations_cnt) {
            if(tile_owner == curr_nation_id) {
                tiles_cnt++;
            }
        }

        if(tiles_cnt > max_tiles_cnt) {
            max_tiles_cnt = tiles_cnt;
            nation_id = curr_nation_id;
        }
    }
    return *world.nations[nation_id];
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
