#ifndef BUILDING_HPP
#define BUILDING_HPP
#include "unit.hpp"

// Type for military outposts
class BuildingType {
public:
    using Id = uint8_t;

    std::string ref_name;

    bool is_naval;
    bool is_build_land_units;
    bool is_build_naval_units;

    // Defensive bonus given to units on the outpost
    float defense_bonus;

    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<Good *, size_t>> req_goods;
};

// A military outpost, on land serves as a "spawn" place for units
// When adjacent to a water tile this serves as a shipyard for spawning naval units
class Building {
public:
    using Id = uint8_t;

    // Position of outpost
    size_t x;
    size_t y;

    BuildingType* type;

    // Owner of the outpost
    Nation* owner;

    // Unit that is currently being built here (nullptr indicates no unit)
    UnitType* working_unit_type;
    BoatType* working_boat_type;

    // Remaining ticks until the unit is built
    uint16_t build_time;

    // Required goods for building the working unit
    std::vector<std::pair<Good *, size_t>> req_goods_for_unit;

    // Required goods for building (current) outpost, first describes the id of the good
    // and the second describes how many this is for the working unit
    std::vector<std::pair<Good *, size_t>> req_goods;
};

#endif