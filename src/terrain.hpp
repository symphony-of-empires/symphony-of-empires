#pragma once

#include <string>

#include "entity.hpp"

class TerrainType : public RefnameEntity<uint8_t>  {
public:
    uint32_t color;
};