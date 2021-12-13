#pragma once

#include "unified_render/entity.hpp"
#include "policy.hpp"
#include <string>

class Ideology : public RefnameEntity<uint8_t> {
public:
    // Checks that the policies of a given nation are "matching"
    // with our ideology (so we set eyecandy and stuff)
    std::string check_policies_fn;

    Policies policies;
};