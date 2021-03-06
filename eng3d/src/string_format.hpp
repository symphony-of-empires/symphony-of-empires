// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      string_format.hpp
//
// Abstract:
//      Formatting of strings
// ----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <string>
#include <stdexcept>
#include "eng3d/utils.hpp"

namespace Eng3D {
    /**
     * @brief String formatter
     * 
     * @tparam Args Formatting argument type list
     * @param format C-formatting string
     * @param args Arguments for formatting
     * @return std::string The resulting formatted text
     */
    template<typename ... Args>
    std::string string_format(const std::string& format, Args ... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        if(size_s <= 0)
            CXX_THROW(std::runtime_error, "Error during formatting");
        size_t size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }
}