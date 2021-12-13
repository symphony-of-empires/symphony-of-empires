#pragma once

#include "unified_render/entity.hpp"
#include <string>
#include <vector>

class Descision : public RefnameEntity<uint8_t> {
public:
    std::string do_descision_function;
    std::string effects;
};

class Nation;
class Event : public RefnameEntity<uint16_t> {
public:
    std::string conditions_function;
    std::string do_event_function;
    std::vector<Nation *> receivers;
    std::vector<Descision> descisions;
    std::string text;
    std::string title;

    bool checked = false;
    
    // Takes a descision by a country
    void take_descision(Nation* sender, Descision* dec);
};