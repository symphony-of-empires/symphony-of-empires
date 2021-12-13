#pragma once

#include <string>
#include <vector>
#include "nation.hpp"
#include "unified_render/entity.hpp"

enum TechnologyType {
    STRATEGIC,
    MILITARY,
    NAVY,
    SOCIAL,
    ECONOMIC,
    POLITICS,
};

class Technology : public RefnameEntity<uint16_t> {
public:
    std::string description;
    float cost;

    TechnologyType type;
    std::vector<Technology*> req_technologies;
    std::vector<NationModifier*> modifiers;
};