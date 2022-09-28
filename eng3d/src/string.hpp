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
#include <stdexcept>
#include "eng3d/utils.hpp"

namespace Eng3D {
    /// @brief A reference to a string on the global string pool
    class StringRef {
    public:
        using Id = uint16_t;
        Id id;

        constexpr StringRef()
            : id{ static_cast<Eng3D::StringRef::Id>(-1) }
        {

        }

        StringRef(const std::string& str);

        constexpr StringRef(Id _id)
            : id{ _id }
        {

        }

        ~StringRef() {

        }

        const std::string& get_string() const;

        inline StringRef& operator=(const std::string& rhs) {
            *this = StringRef(rhs);
            return *this;
        }

        inline bool operator==(const StringRef& rhs) const {
            return this->get_string() == rhs.get_string();
        }

        inline bool operator==(const std::string& rhs) const {
            return this->get_string() == rhs;
        }

        inline const char* c_str() const {
            return this->get_string().c_str();
        }
    };

    inline std::string operator+(const char* lhs, const StringRef& rhs) {
        return std::string(lhs) + rhs.get_string();
    }

    inline std::string operator+(const StringRef& lhs, const char* rhs) {
        return lhs.get_string() + rhs;
    }

    inline std::string operator+(const std::string& lhs, const StringRef& rhs) {
        return lhs + rhs.get_string();
    }

    inline std::string operator+(const StringRef& lhs, const std::string& rhs) {
        return lhs.get_string() + rhs;
    }

    class State;
    /// @brief The string pool manager (singleton), used mainly for translation
    /// purpouses. But also helps to reduce the memory size of various objects.
    class StringManager {
        std::vector<std::string> strings;
        Eng3D::State& s;
    public:
        StringManager(Eng3D::State& _s);
        ~StringManager() = default;

        Eng3D::StringRef insert(const std::string& str) {
            const std::scoped_lock lock(this->strings_mutex);
            this->strings.push_back(str);
            this->strings.back().shrink_to_fit();
            return Eng3D::StringRef(this->strings.size() - 1);
        }

        const std::string& get_by_id(const Eng3D::StringRef ref) const {
            const std::scoped_lock lock(this->strings_mutex);
            return this->strings[ref.id];
        }

        static StringManager& get_instance();
        mutable std::mutex strings_mutex;
    };

    /// @brief String formatter
    /// @tparam Args Formatting argument type list
    /// @param format C-formatting string
    /// @param args Arguments for formatting
    /// @return std::string The resulting formatted text
    template<typename ... Args>
    std::string string_format(const std::string& format, Args&& ... args) {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), std::forward<decltype(args)>(args)...) + 1; // Extra space for '\0'
        if(size_s <= 0)
            CXX_THROW(std::runtime_error, "Error during formatting");
        size_t size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }
};
using Eng3D::string_format;

namespace Eng3D::Locale {
    void from_file(const std::string& filename);
    std::string translate(const std::string& str);
}
using Eng3D::Locale::translate;

namespace Eng3D {
    /// @brief String formatter, with translation
    /// @tparam Args Formatting argument type list
    /// @param format C-formatting string
    /// @param args Arguments for formatting
    /// @return std::string The resulting formatted text
    template<typename ... Args>
    std::string translate_format(const std::string& format, Args&& ... args) {
        return Eng3D::string_format(Eng3D::Locale::translate(format), std::forward<decltype(args)>(args)...);
    }
};
using Eng3D::translate_format;
