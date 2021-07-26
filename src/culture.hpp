#ifndef CULTURE_HPP
#define CULTURE_HPP

typedef uint16_t CultureId;
class Culture {
public:
    using Id = uint16_t;

    std::string name;
    std::string ref_name;
};

#endif