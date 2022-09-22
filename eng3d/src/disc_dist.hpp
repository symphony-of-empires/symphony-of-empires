// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      disc_dist.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <numeric>
#include <algorithm>
#include <cstddef>
#include <vector>
#include "eng3d/rand.hpp"

/// @brief Uses the Aiias method to generate a lookup table of with different probabilties
/// @tparam T The type of items to obtain the probability from
/// https://www.keithschwarz.com/darts-dice-coins/
/// https://en.wikipedia.org/wiki/Alias_method
template<typename T>
class DiscreteDistribution {
private:
    std::vector<T> _items;
    std::vector<float> alias;
    std::vector<float> prob;
    Eng3D::Rand _rand;
public:
    DiscreteDistribution(std::vector<T> items, std::vector<float> probabilities)
        : _items{ items },
        _rand{ 0 }
    {
        // Scale each probabilty
        float total = std::accumulate(probabilities.begin(), probabilities.end(), 0);
        float scale = probabilities.size() / total;
        for(auto& p : probabilities) p *= scale;

        // Fill two work tables with probabilities larger/smaller than 1
        std::vector<std::pair<float, size_t>> small;
        small.reserve(probabilities.size());
        std::vector<std::pair<float, size_t>> big;
        big.reserve(probabilities.size());
        for(size_t i = 0; i < probabilities.size(); i++) {
            if(probabilities[i] < 1.0f)
                small.emplace_back(std::make_pair(probabilities[i], i));
            else
                big.emplace_back(std::make_pair(probabilities[i], i));
        }

        // Remove from the bigger one and place on the smaller ones
        alias.resize(probabilities.size(), 0);
        prob.resize(probabilities.size(), 0);
        while(!small.empty() && !big.empty()) {
            const auto& less = small.back();
            auto greater = big.back();
            big.pop_back();
            prob[less.second] = less.first;
            alias[less.second] = greater.second;
            small.pop_back();
            greater.first = (greater.first + less.first) - 1;
            if(greater.first < 1.f)
                small.emplace_back(greater);
            else
                big.emplace_back(greater);
        }

        while(!big.empty()) {
            const auto& greater = big.back();
            prob[greater.second] = 1.f;
            big.pop_back();
        }

        while(!small.empty()) {
            const auto& less = small.back();
            prob[less.second] = 1.f;
            small.pop_back();
        }
    }

    ~DiscreteDistribution()
    {

    };

    // Get a random item with a certian probabilty
    T get_item() {
        auto index = _rand() % _items.size();
        auto r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        if(prob[index] < r) index = alias[index];
        return _items[index];
    }
};
