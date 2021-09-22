#include "province.hpp"
#include "world.hpp"
#include "product.hpp"
#include "good.hpp"
#include "company.hpp"

// Gets ID from pointer
Province::Id Province::get_id(const World& world) {
    const std::vector<Province *>* provinces = &world.provinces;
    const auto province = std::find(provinces->begin(), provinces->end(), this);
    if(province != provinces->end()) {
        return (Province::Id)std::distance(provinces->begin(), province);
    }
    return (Province::Id)-1;
}

// Obtains the country that currently has a larger number of
// tiles controlled from this province
Nation& Province::get_occupation_controller(const World& world) const {
    std::vector<Nation::Id> nations_cnt;
    for(size_t x = min_x; x < max_x; x++) {
        for(size_t y = min_y; y < max_y; y++) {
            nations_cnt.push_back(world.get_tile(x, y).owner_id);
        }
    }

    // We are going to obtain all nations who have a foothold on this province
    // then we are going to obtain the one with the highest tile count
    std::set<Nation::Id> unique_nations(nations_cnt.begin(), nations_cnt.end());
    Nation::Id nation_id = world.get_id(owner);
    size_t max_tiles_cnt = 0;
    for(const auto& curr_nation_id: unique_nations) {
        // This will count the tiles for this nation
        const size_t tiles_cnt = std::count(nations_cnt.begin(), nations_cnt.end(), curr_nation_id);
        if(tiles_cnt > max_tiles_cnt) {
            max_tiles_cnt = tiles_cnt;
            nation_id = curr_nation_id;
        }
    }
    return (*world.nations[nation_id]);
}

// Calculates the total number of POPs in this province (total population)
size_t Province::total_pops(void) const {
    size_t total = 0;
    for(const auto& pop: pops) {
        total += pop.size;
    }
    return total;
}

// Create a vector containing a list of all products available on this province
std::vector<Product*> Province::get_products(const World& world) const {
    std::vector<Product *> products;
    products.reserve(world.products.size());
    for(const auto& product: world.products) {
        Product::Id product_id = world.get_id(product);

        // Only valid indices
        if(product_id == (Product::Id)-1) {
            continue;
        }

        // Province must have stockpile
        if(!stockpile.at(product_id)) {
            continue;
        }
           
        products.push_back(product);
    }
    return products;
}