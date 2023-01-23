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
//      blocked_range.hpp
//
// Abstract:
//      Emulates blocked_range ranges from tbb on systems without a tbb
//      implementation.
// ----------------------------------------------------------------------------

#pragma once

namespace tbb {
    template<typename It>
    struct blocked_range {
        blocked_range(It _first, It _last)
            : first{ _first },
            last{ _last }
        {

        }

        It first;
        It last;
    };
}
