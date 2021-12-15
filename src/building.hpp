#pragma once

#include "unified_render/entity.hpp"
#include "unit.hpp"
#include "company.hpp"

// Type for military outposts
class BuildingType : public RefnameEntity<uint8_t> {
public:
    bool is_plot_on_sea;
    bool is_plot_on_land;
    bool is_build_land_units;
    bool is_build_naval_units;

    // Defensive bonus given to units on the outpost
    float defense_bonus;

    // Is this a factory?
    bool is_factory;
    // List of goods required to create output
    std::vector<Good*> inputs;
    // List of goods that this factory type creates
    std::vector<Good*> outputs;

    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<Good*, size_t>> req_goods;
};

// A military outpost, on land serves as a "spawn" place for units
// When adjacent to a water tile this serves as a shipyard for spawning naval units
class Building : public IdEntity<uint16_t> {
public:
    Building(){};
    ~Building();
    bool can_do_output(void);
    void add_to_stock(const Good* good, size_t add);
    Province* get_province(void);
    Nation* get_owner(void);
    void create_factory(void);
    void delete_factory(void);
    std::pair<float, float> get_pos(void) const;

#if !defined TILE_GRANULARITY
    
#else
    // Position of outpost
    size_t x, y;
#endif

    BuildingType* type;

    // Unit that is currently being built here (nullptr indicates no unit)
    UnitType* working_unit_type;

    // Remaining ticks until the unit is built
    uint16_t build_time;

    // Required goods for building the working unit
    // TODO: change this to a struct instead of a pair for readablity
    std::vector<std::pair<Good*, size_t>> req_goods_for_unit;
    // Required goods for building this, or repairing this after a military attack
    std::vector<std::pair<Good*, size_t>> req_goods;

    // Nation owner of this building
    Nation* owner;
    
    // Province that "supplies" this building
    Province* province;
    
    // Corporate owner of this building
    Company* corporate_owner;

    // Total money that the factory has
    float budget = 0.f;
    
    // Days that the factory has not been operational
    size_t days_unoperational = 0;
    
    // Money needed to produce - helps determine the price of the output products
    float production_cost = 0.f;
    
    // Stockpile of inputs in the factory
    std::vector<size_t> stockpile;
    
    // Used for faster lookups
    std::vector<Product*> output_products;
    
    // The employees needed per output
    std::vector<size_t> employees_needed_per_output;

    // Level of building (all starts at 0)
    size_t level = 0;
    
    // The pay we are willing to give
    size_t willing_payment = 0;
    
    // How many workers are in the industry
    size_t workers = 0;
};