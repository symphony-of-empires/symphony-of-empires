#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>

// An entity which can only be made referenced by an id
template<typename IdType>
class IdEntity {
public:
    using Id = IdType;
};

// An entity which can be referenced via a ref_name and also via id
template<typename IdType>
class RefnameEntity : public IdEntity<IdType> {
public:
    std::string ref_name;
    std::string name;
};

#endif