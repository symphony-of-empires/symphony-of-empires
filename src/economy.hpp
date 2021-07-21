#ifndef ECONOMY_H
#define ECONOMY_H

#include <algorithm>
#include <string>
#include <vector>
#include <deque>
#include <cstdint>
#include <cstddef>

#include "province.hpp"

// Functions that do a economy simulation in various steps, divided to reduce
// overhead, they are distributed accross 48 ticks
namespace Economy {
    void do_phase_1(World& world);
    void do_phase_2(World& world);
    void do_phase_3(World& world);
    void do_phase_4(World& world);
}

typedef uint16_t CompanyId;
/**
* A company that operates one or more factories and is able to build even more factories
 */
class Company {
public:
    using Id = CompanyId;
    
    // Name of this company
    std::string name;
    
    // How many money this company has
    float money;

    bool is_transport;
    bool is_retailer;
    bool is_industry;

    // Nation that controls this company, essentially making it a state-controlled company (can be null)
    Nation* nation = nullptr;

    // List of province IDs where this company operates (mostly used for transport companies)
    std::vector<Province *> operating_provinces;

    inline bool in_range(Province* province) {
        return (std::find(operating_provinces.begin(), operating_provinces.end(), province)
            != operating_provinces.end());
    }

    void name_gen() {
        size_t r = (rand() % 12) + 1;
        for(size_t i = 0; i < r; i++) {
            name += 'a' + (rand() % 25);
        }
    }
};

typedef uint8_t GoodId;
/**
* A good, mostly serves as a "product type"
 */
class Good {
public:
    using Id = GoodId;

    std::string name = "default";
    std::string ref_name = "default";

    // Determines if the good can be "eaten", this makes the product fullfill a life need
    bool is_edible;
};

typedef uint8_t IndustryTypeId;
class IndustryType {
public:
    using Id = IndustryTypeId;

    std::string name;
    std::string ref_name;

    // List of good required to create output
    std::vector<Good *> inputs;

    // List of good that this factory type creates
    std::vector<Good *> outputs;
};

#include "nation.hpp"
class World;

typedef uint16_t IndustryId;
class Industry {
public:
    using Id = IndustryId;

    bool can_do_output(const World& world);
    void add_to_stock(const World& world, const Good* good, size_t add);

    // Owner nation of this factory
    Company* owner;

    // Type of factory
    IndustryType* type;

    // Total money that the factory has
    float budget = 0.f;

    // Days that the factory has not been operational
    size_t days_unoperational = 0;

    // Money needed to produce - helps determine the price of the output products
    float production_cost = 0.f;

    // Stockpile of inputs in the factory
    std::vector<size_t> stockpile;
    
    // Used for faster lookups
    std::vector<Product *> output_products;

    // The desired quality of a product (otherwise it's not accepted)
    float min_quality = 0.f;

    // The pay we are willing to give
    size_t willing_payment = 0;
    
    // How many workers are in the industry
    size_t workers = 0;
};

typedef uint16_t ProductId;
/**
* A product (based off a Good) which can be bought by POPs, converted by factories and transported
* accross the world
 */
class Product {
public:
    using Id = ProductId;

    // Onwer (companyId) of this product
    Company* owner;
    
    // Origin province (where this product was made)
    Province* origin;
    
    // Industry in province that made this product
    Industry* industry;
    
    // Good that this product is based on
    Good* good;
    
    // Price of the product
    float price;
    
    // Velocity of change of price of the product
    float price_vel;
    
    // Quality of the product
    float quality;
    
    // Total supply (worldwide) of the product
    size_t supply;
    
    // Total demand (worldwide) of the product
    size_t demand;

    // History of price, supply and demand for the past 30 days
    std::deque<float> price_history;
    std::deque<size_t> supply_history;
    std::deque<size_t> demand_history;
};

#endif
