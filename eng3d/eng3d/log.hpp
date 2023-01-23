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
//      log.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>

namespace Eng3D::Log {
    extern bool debug_show;
    void log(const std::string_view severity, const std::string_view category, const std::string_view msg);

    static inline void debug(const std::string_view category, const std::string_view msg) {
        if(debug_show)
            Eng3D::Log::log("Debug", category, msg);
    }

    static inline void warning(const std::string_view category, const std::string_view msg) {
        Eng3D::Log::log("Warning", category, msg);
    }

    static inline void error(const std::string_view category, const std::string_view msg) {
        Eng3D::Log::log("Error", category, msg);
    }
}
