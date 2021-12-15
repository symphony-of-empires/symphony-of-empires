#pragma once

#include <string>

#include "unified_render/entity.hpp"

class TerrainType : public RefnameEntity<uint8_t>  {
public:
    // Associative color (with bitmap)
    uint32_t color;

    // Should this terrain be treated as sea?
    bool is_water_body;
    
    float movement_penalty;
};