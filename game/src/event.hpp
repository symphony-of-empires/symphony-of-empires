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

#include "eng3d/entity.hpp"
#include <string>
#include <vector>

class Decision : public RefnameEntity<uint8_t> {
public:
    //Decision();
    //~Decision();

    std::string do_decision_function;
    std::string effects;
};

class Nation;
class Event : public RefnameEntity<uint16_t> {
public:
    //Event();
    //~Event();
    void take_decision(Nation& sender, Decision& dec);
    
    std::string conditions_function;
    std::string do_event_function;
    std::vector<Nation *> receivers;
    std::vector<Decision> decisions;
    std::string text;
    std::string title;

    bool checked = false;
};