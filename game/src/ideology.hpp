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
//      ideology.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <cstdint>
#include "eng3d/entity.hpp"

#include "policy.hpp"

class Ideology : public RefnameEntity<uint8_t> {
public:
    std::uint32_t color;
    Eng3D::StringRef name;
    Policies policies;
    // Checks that the policies of a given nation are "matching"
    // with our ideology (so we set eyecandy and stuff)
    Eng3D::StringRef check_policies_fn;
};
