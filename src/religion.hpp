#pragma once

#include <string>

#include "unified_render/entity.hpp"

class Religion : public RefnameEntity<uint8_t> {
public:
    uint32_t color;
};