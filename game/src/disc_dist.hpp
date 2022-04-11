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

// https://www.keithschwarz.com/darts-dice-coins/
// https://en.wikipedia.org/wiki/Alias_method

// Uses the Aiias method to generate a lookup table of with different probabilties
template<typename T>
class DiscreteDistribution {
private:
	std::vector<T> _items;
	std::vector<float> alias;
	std::vector<float> prob;
public:
	DiscreteDistribution(std::vector<T> items, std::vector<float> probabilities)
		: _items{ items }
	{
		// Scale each probabilty
		float total = std::accumulate(probabilities.begin(), probabilities.end(), 0);
		float scale = probabilities.size() / total;
		for(size_t i = 0; i < probabilities.size(); i++) {
			probabilities[i] *= scale;
		}

		// Fill two work tables with probabilities larger/smaller than 1
		std::vector<std::pair<float, size_t>> small;
		std::vector<std::pair<float, size_t>> big;
		for(size_t i = 0; i < probabilities.size(); i++) {
			if(probabilities[i] < 1.0f) {
				small.push_back(std::make_pair(probabilities[i], i));
			}
			else {
				big.push_back(std::make_pair(probabilities[i], i));
			}
		}

		// Remove from the bigger one and place on the smaller ones
		alias.resize(probabilities.size(), 0);
		prob.resize(probabilities.size(), 0);
		while(small.size() > 0 && big.size() > 0) {
			std::pair<float, size_t> less = small.back();
			small.pop_back();
			std::pair<float, size_t> greater = big.back();
			big.pop_back();
			prob[less.second] = less.first;
			alias[less.second] = greater.second;
			greater.first = (greater.first + less.first) - 1;
			if(greater.first < 1.f) {
				small.push_back(greater);
			} else {
				big.push_back(greater);
			}
		}

		while(big.size() > 0) {
			std::pair<float, int> greater = big.back();
			big.pop_back();
			prob[greater.second] = 1.f;
		}
		
		while(small.size() > 0) {
			std::pair<float, int> less = small.back();
			small.pop_back();
			prob[less.second] = 1.f;
		}
	}
	
	~DiscreteDistribution(void)
	{

	};

	// Get a random item with a certian probabilty
	T get_item(void) {
		size_t index = std::rand() % _items.size();
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		if(prob[index] < r) {
			index = alias[index];
		}
		return _items[index];
	}
};