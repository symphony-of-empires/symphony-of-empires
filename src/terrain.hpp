#ifndef TERRAIN_HPP
#define TERRAIN_HPP
#include <string>

#include "entity.hpp"

class TerrainType : public RefnameEntity<uint8_t>  {
public:
    uint32_t color;
};

#endif