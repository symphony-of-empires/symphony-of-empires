// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.-----------------
// Name:
//      event.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

#include "eng3d/string.hpp"
#include "objects.hpp"

class Decision : public RefnameEntity<DecisionId> {
public:
    Decision() = default;
    ~Decision() = default;

    Eng3D::StringRef name;
    Eng3D::StringRef effects;

    /// @todo Reload these when serializing
    int do_decision_function = 0;
};

class Nation;
class Event : public RefnameEntity<EventId> {
public:
    Event() = default;
    ~Event() = default;
    void take_decision(Nation& sender, Decision& dec);

    Eng3D::StringRef name;
    std::vector<Nation *> receivers;
    std::vector<Decision> decisions;
    Eng3D::StringRef text;
    Eng3D::StringRef title;
    bool checked = false;

    /// @todo Reload these when serializing
    int conditions_function = 0;
    int do_event_function = 0;
};