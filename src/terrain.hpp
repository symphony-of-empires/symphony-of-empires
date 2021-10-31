#pragma once

#include <string>

#include "entity.hpp"

class TerrainType : public RefnameEntity<uint8_t>  {
public:
    // Associative colour (with bitmap)
    uint32_t color;
    
    float movement_penalty;
};