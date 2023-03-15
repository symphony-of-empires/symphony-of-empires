// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      freelist.hpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <cassert>
#include <optional>
#include "eng3d/utils.hpp"

namespace Eng3D {
    template<typename T>
    struct Freelist {
        size_t add(T& e) {
            size_t index = 0;
            if(slots.empty()) {
                index = data.size();
                data.emplace_back(e);
            } else {
                index = slots.back();
                slots.pop_back();
                data[index].emplace(e);
            }
            return index;
        }

        void remove(size_t index) {
            assert(data[index].has_value());
            data[index].reset();
            slots.push_back(index);
        }

        T& operator[](size_t index) {
            assert(index < data.size());
            return data[index].value();
        }

        const T& operator[](size_t index) const {
            assert(index < data.size());
            return data[index].value();
        }

        T& at(size_t index) {
            assert(index < data.size());
            return data[index].value();
        }

        const T& at(size_t index) const {
            assert(index < data.size());
            return data[index].value();
        }

        void for_each(const auto& lambda) const {
            for(const auto& e : data)
                if(e.has_value())
                    lambda(e.value());
        }

        void for_each(const auto& lambda) {
            for(auto& e : data)
                if(e.has_value())
                    lambda(e.value());
        }

        std::vector<std::optional<T>> data;
        std::vector<size_t> slots;
    };
}
