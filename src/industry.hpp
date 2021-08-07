#ifndef INDUSTRY_HPP
#define INDUSTRY_HPP

#include "company.hpp"
#include "nation.hpp"
#include "good.hpp"

typedef uint8_t IndustryTypeId;
class IndustryType {
public:
    using Id = uint16_t;

    std::string name;
    std::string ref_name;

    // List of good required to create output
    std::vector<Good *> inputs;

    // List of good that this factory type creates
    std::vector<Good *> outputs;
};

class World;

typedef uint16_t IndustryId;
class Industry {
public:
    using Id = uint16_t;

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

#endif