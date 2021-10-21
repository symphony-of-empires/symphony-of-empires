#ifndef POP_H
#define POP_H
#include <vector>
#include <string>
#include "entity.hpp"
#include "ideology.hpp"

class PopType : public RefnameEntity<uint8_t> {
public:
    float social_value;

    bool is_entrepreneur;
    bool is_slave;
    bool is_farmer;
    bool is_laborer;

    // TODO: Ideology spectrum
    //std::vector<Ideology*> spectrum;
};

#include "culture.hpp"
#include "religion.hpp"
class Pop {
public:
    size_t size;
    size_t unemployed = 0;
    float literacy;
    
    float militancy = 0.f;
    float consciousness = 0.f;
    float budget = 0.f;
    
    float savings = 0.f;
    
    float life_needs_met = 0.f;
    float everyday_needs_met = 0.f;
    float luxury_needs_met = 0.f;
    
    PopType* type;
    Culture* culture;
    Religion* religion;

    // TODO: On serializer
    Ideology* ideology;

    bool operator==(const Pop& rhs) {
        return (this->culture == rhs.culture
            && this->religion == rhs.religion
            && this->type == rhs.type
            && this->ideology == rhs.ideology);
    }
};

#endif
