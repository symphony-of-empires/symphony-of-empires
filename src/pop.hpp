#ifndef POP_H
#define POP_H
#include <vector>
#include <string>

enum {
    POP_TYPE_ENTRPRENEUR = 0,
    POP_TYPE_ARTISAN = 1,
    POP_TYPE_CRAFTSMEN = 2,
    POP_TYPE_BUREAUCRAT = 3,
    POP_TYPE_ARISTOCRAT = 4,
    POP_TYPE_CLERGYMEN = 5,
    POP_TYPE_FARMER = 6,
    POP_TYPE_SOLDIER = 7,
    POP_TYPE_OFFICER = 8,
    POP_TYPE_LABORER = 9,
    POP_TYPE_SLAVE = 10,
    POP_TYPE_WOMAN = 11,
    POP_TYPE_CHILDREN = 12,
    POP_TYPE_AGED = 13,
};

class PopType {
public:
    using Id = uint8_t;
    
    std::string name;
    std::string ref_name;
    float average_budget;
};

#include "culture.hpp"
#include "religion.hpp"
#include "policy.hpp"
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
    
    PopType::Id type_id;
    Culture::Id culture_id;
    Religion::Id religion_id;
    Policies policies;
};

#endif
