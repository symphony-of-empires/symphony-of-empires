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
//      server/trade.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include "province.hpp"

class World;

namespace Economy {
    class Trade final {
    public:
        Trade() {};

        void recalculate(const World& world);

        struct Vertex
        {
            Vertex(float _cost, Province::Id _province_id) : cost{_cost}, province_id{_province_id} {};
            float cost;
            Province::Id province_id;
        };

        std::vector<std::vector<float>> trade_cost;
    private:
        void initialize(const World& world);

        std::vector<std::vector<Vertex>> neighbours;
    };
};