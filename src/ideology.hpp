#ifndef IDEOLOGY_HPP
#define IDEOLOGY_HPP

#include <string>

class Ideology {
public:
    using Id = uint8_t;

    std::string name;
    std::string ref_name;

    // Checks that the policies of a given nation are "matching"
    // with our ideology (so we set eyecandy and stuff)
    std::string check_policies_fn;
};

#endif