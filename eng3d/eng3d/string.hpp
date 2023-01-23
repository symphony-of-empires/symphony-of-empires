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
//      string.hpp
//
// Abstract:
//      Provides automatic pooling of strings and string formatting utilities.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include <memory>
#include <compare>
#include <iterator>
#include <stdexcept>
#include "eng3d/utils.hpp"

namespace Eng3D {
    /// @brief A reference to a string on the global string pool
    struct StringRef {
        constexpr StringRef() = default;
        StringRef(const std::string_view str);
        constexpr StringRef(size_t _id)
            : id{ _id }
        {

        }
        StringRef& operator=(const std::string& rhs) {
            *this = StringRef(rhs);
            return *this;
        }
        const std::string_view get_string() const;
        constexpr bool operator==(const StringRef&) const noexcept = default;
        constexpr auto operator<=>(const StringRef&) const noexcept = default;

        const char* c_str() const {
            return this->get_string().data();
        }
        
        operator std::string() const {
            return std::string(this->get_string());
        }

        size_t get_id() const { return id; }
        size_t id = 0;
    };
    
    class State;
    /// @brief The string pool manager (singleton), used mainly for translation
    /// purpouses. But also helps to reduce the memory size of various objects.
    class StringManager {
        Eng3D::State& s;
    public:
        StringManager(Eng3D::State& _s);
        ~StringManager() = default;

        Eng3D::StringRef insert(const std::string& str) {
            const std::scoped_lock lock(strings_mutex);
            const auto id = strings.size();
            std::copy(str.begin(), str.end(), std::back_inserter(strings));
            strings.push_back('\0');
            return Eng3D::StringRef(id);
        }

        const std::string_view get_by_id(const Eng3D::StringRef ref) const {
            const std::scoped_lock lock(strings_mutex);
            return &strings[ref.get_id()];
        }

        std::vector<char> strings;
        static StringManager& get_instance();
        mutable std::mutex strings_mutex;
    };

    /// @brief String formatter
    /// @tparam Args Formatting argument type list
    /// @param format C-formatting string
    /// @param args Arguments for formatting
    /// @return std::string The resulting formatted text
    template<typename ... Args>
    std::string string_format(const std::string_view format, Args&& ... args) {
#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wformat-security"
#endif
        int size_s = std::snprintf(nullptr, 0, format.data(), std::forward<decltype(args)>(args)...) + 1; // Extra space for '\0'
        if(size_s <= 0)
            CXX_THROW(std::runtime_error, "Error during formatting");
        size_t size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.data(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
#ifdef __clang__
#   pragma clang diagnostic pop
#endif
    }
}
using Eng3D::string_format;

namespace Eng3D::Locale {
    void from_file(const std::string& filename);
    std::string translate(const std::string_view str);
    std::string format_number(double num);
}
using Eng3D::Locale::translate;

namespace Eng3D {
    /// @brief String formatter, with translation
    /// @tparam Args Formatting argument type list
    /// @param format C-formatting string
    /// @param args Arguments for formatting
    /// @return std::string The resulting formatted text
    template<typename ... Args>
    std::string translate_format(const std::string_view format, Args&& ... args) {
        return Eng3D::string_format(Eng3D::Locale::translate(format), std::forward<decltype(args)>(args)...);
    }
}
using Eng3D::translate_format;
