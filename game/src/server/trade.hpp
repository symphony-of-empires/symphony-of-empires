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
    struct Trade final {
        Trade() = default;
        ~Trade() = default;
        void recalculate(const World& world);

        struct Vertex {
            constexpr Vertex(float _cost, Province::Id _key) : cost{_cost}, key{_key}
            {

            }
            ~Vertex() = default;
            float cost = 0.f;
            Province::Id key = Province::invalid();
        };

        /// @brief Cost-evaluatable provinces, we discard sea and ocean provinces
        /// from this formula to save space and time since goods directly transport
        /// to the land provinces
        std::vector<Province::Id> cost_eval;
        std::vector<std::vector<float>> trade_costs;
    private:
        inline glm::vec3 get_sphere_coord(const Province& province, glm::vec2 world_size);
        inline float get_trade_cost(const Province& province1, const Province& province2, glm::vec2 world_size);
        inline void initialize(const World& world);
        std::vector<std::vector<Vertex>> neighbours;
    };
};