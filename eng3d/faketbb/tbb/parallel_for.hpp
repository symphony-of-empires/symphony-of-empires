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
//      parallel_for.hpp
//
// Abstract:
//      Emulates parallel_for from tbb on systems without a tbb implementation.
// ----------------------------------------------------------------------------

#pragma once

#include <ranges>
#include <tbb/blocked_range.hpp>

namespace tbb {
    template<typename T, typename F>
    void parallel_for(T range, F&& func) {
        func(std::ranges::make_iterator_range(range.first, range.last));
    }

    template<typename It, typename F>
    void parallel_for(It first, It last, F&& func) {
        for(auto it = first; it != last; it++)
            func(*it);
    }
}
