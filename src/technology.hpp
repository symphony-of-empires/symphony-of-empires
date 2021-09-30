#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H
#include <string>
#include "nation.hpp"

enum TechnologyType {
    STRATEGIC,
    MILITARY,
    NAVY,
    SOCIAL,
    ECONOMIC,
    POLITICS,
};

class Invention {
public:
    using Id = uint8_t;

    std::string ref_name;
    std::string name;
    std::string description;
    
    NationModifier* mod;
};

class Technology {
public:
    using Id = uint16_t;
    
    std::string ref_name;
    std::string name;
    std::string description;
    float cost;

    TechnologyType type;

    std::vector<Technology*> req_technologies;
    std::vector<Invention*> inventions;
};

#endif