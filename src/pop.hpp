#pragma once

#include <vector>
#include <string>
#include "unified_render/entity.hpp"
#include "ideology.hpp"

enum class PopGroup {
    BURGEOISE,
    Slave,
    FARMER,
    LABORER,
    Other,
};
class PopType: public RefnameEntity<uint8_t> {
public:
    float social_value;
    PopGroup group;
};

#include "culture.hpp"
#include "religion.hpp"
struct Pop {
public:
    bool operator==(const Pop& rhs) const;
    Ideology* get_ideology(void) const;

    size_t size;
    size_t unemployed = 0;
    float literacy;

    float militancy = 0.f;
    float con = 0.f;
    float budget = 0.f;

    float savings = 0.f;

    float life_needs_met = 0.f;
    float everyday_needs_met = 0.f;
    float luxury_needs_met = 0.f;

    PopType* type;
    Culture* culture;
    Religion* religion;

    // Approval % of all the ideologies (1:1)
    std::vector<float> ideology_approval;
};