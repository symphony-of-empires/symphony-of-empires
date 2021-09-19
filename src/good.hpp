#ifndef GOOD_HPP
#define GOOD_HPP
#include <string>

// A good, mostly serves as a "product type"
class Good {
public:
    using Id = uint16_t;

    std::string name;
    std::string ref_name;

    // Determines if the good can be "eaten", this makes the product fullfill a life need
    bool is_edible;
};

#endif