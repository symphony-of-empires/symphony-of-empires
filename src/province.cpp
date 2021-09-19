#include "province.hpp"
#include "world.hpp"
#include "product.hpp"
#include "industry.hpp"
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

// Adds a new industry in the province and adds it's output
// products into the world accordingly
void Province::add_industry(World& world, Industry* industry) {
    IndustryType* type = industry->type;

    // Add a product for each output
    for(const auto& output: type->outputs) {
        Product* new_product = new Product();
        new_product->industry = industry;
        new_product->good = output;
        new_product->owner = industry->owner;
        new_product->origin = this;

        industry->output_products.push_back(new_product);
        world.products.push_back(new_product);

        industry->employees_needed_per_output.push_back(500);
    }

    // We will set inputs_satisfied to same size as inputs
    // Industries start with 100 of stockpiles
    industry->stockpile.insert(industry->stockpile.begin(), industry->type->inputs.size(), 100);

    // Finally add the industry to the province's vector
    industries.push_back(*industry);
}

// Removes an industry and their products from the entire world
// this is only used when industries go bankrupt!
void Province::remove_industry(World& world, Industry* industry) {
    // Remove products of this industry from world market
    for(size_t i = 0; i < industry->output_products.size(); i++) {
        Product* product = industry->output_products[i];
        Product::Id product_id = world.get_id(product);

        // Delete from province stockpiles
        for(auto& province: world.provinces) {
            province->stockpile.erase(province->stockpile.begin() + product_id);
        }

        // Delete product from the world list
        delete product;
        world.products.erase(world.products.begin() + product_id);
    }

    // TODO: Remove outputs from province's product list

    // Remove this industry totally from the list of industries
    size_t industry_id = world.get_id(*this, industry);
    industries.erase(industries.begin() + industry_id);
    return;
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