#pragma once

#include <string>
#include "nation.hpp"
#include "entity.hpp"

enum TechnologyType {
    STRATEGIC,
    MILITARY,
    NAVY,
    SOCIAL,
    ECONOMIC,
    POLITICS,
};

class Invention : public RefnameEntity<uint8_t> {
public:
    std::string description;
    NationModifier* mod;
};

class Technology : public RefnameEntity<uint16_t> {
public:
    std::string description;
    float cost;

    TechnologyType type;

    std::vector<Technology*> req_technologies;
    std::vector<Invention*> inventions;
};