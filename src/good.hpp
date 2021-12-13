#pragma once

#include "unified_render/entity.hpp"
#include <string>

// A good, mostly serves as a "product type"
class Good : public RefnameEntity<uint16_t> {
public:
    // Determines if the good can be "eaten", this makes the product fullfill a life need
    bool is_edible;
};
