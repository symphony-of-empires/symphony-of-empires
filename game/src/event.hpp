// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      event.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "unified_render/entity.hpp"
#include <string>
#include <vector>

class Descision : public RefnameEntity<uint8_t> {
public:
    //Descision();
    //~Descision();

    std::string do_descision_function;
    std::string effects;
};

class Nation;
class Event : public RefnameEntity<uint16_t> {
public:
    //Event();
    //~Event();
    void take_descision(Nation& sender, Descision& dec);
    
    std::string conditions_function;
    std::string do_event_function;
    std::vector<Nation *> receivers;
    std::vector<Descision> descisions;
    std::string text;
    std::string title;

    bool checked = false;
};