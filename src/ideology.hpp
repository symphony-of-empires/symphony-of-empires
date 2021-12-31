#pragma once

#include <string>

#include "unified_render/entity.hpp"
#include "policy.hpp"

class Ideology : public RefnameEntity<uint8_t> {
public:
    uint32_t color;
    
    // Checks that the policies of a given nation are "matching"
    // with our ideology (so we set eyecandy and stuff)
    std::string check_policies_fn;

    Policies policies;
};