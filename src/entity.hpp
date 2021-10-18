#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>

// An entity which can only be made referenced by an id
template<typename IdType>
class IdEntity {
public:
    using Id = IdType;

    // Id used to speed up Id lookups on any context
    // NOTE: This depends that the container is sequential (as if it was
    // a contigous array) - Otherwise this optimization **WILL** break
    mutable Id cached_id = (Id)-1;
};

// An entity which can be referenced via a ref_name and also via id
template<typename IdType>
class RefnameEntity : public IdEntity<IdType> {
public:
    std::string ref_name;
    std::string name;
};

#endif