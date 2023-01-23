// Eng3D - General purpouse game engine
// Copyright (C) 2022, Eng3D contributors
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
//      combinable.hpp
//
// Abstract:
//      Emulates combinable from tbb on systems without a tbb
//      implementation.
// ----------------------------------------------------------------------------

#pragma once

#include <ranges>

namespace tbb {
    template<typename T>
    struct combinable {
        T value;

        const T& local() const {
            return value;
        }

        T& local() {
            return value;
        }

        template<typename F>
        void combine_each(F&& func)
        {
            func(value);
        }
    };
}
