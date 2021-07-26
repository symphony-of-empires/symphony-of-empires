#ifndef RELIGION_HPP
#define RELIGION_HPP

typedef uint8_t ReligionId;
class Religion {
public:
    using Id = uint8_t;

    std::string name;
    std::string ref_name;
};

#endif