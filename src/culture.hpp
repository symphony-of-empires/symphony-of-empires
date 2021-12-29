#pragma once

#include <string>

#include "unified_render/entity.hpp"

class Culture : public RefnameEntity<uint16_t> {
public:
    uint32_t color;
    std::string adjective;
    std::string noun;
    std::string combo_form;
};