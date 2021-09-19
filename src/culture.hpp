#ifndef CULTURE_HPP
#define CULTURE_HPP
#include <string>

class Culture {
public:
    using Id = uint16_t;

    std::string name;
    std::string ref_name;
};

#endif