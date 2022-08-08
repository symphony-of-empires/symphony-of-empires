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
//      string.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>

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

        inline const char *c_str() const {
            return this->get_string().c_str();
        }
    };

    inline std::string operator+(const char *lhs, const StringRef& rhs) {
        return std::string(lhs) + rhs.get_string();
    }

    inline std::string operator+(const StringRef& lhs, const char *rhs) {
        return lhs.get_string() + rhs;
    }

    inline std::string operator+(const std::string& lhs, const StringRef& rhs) {
        return std::string(lhs) + rhs.get_string();
    }

    inline std::string operator+(const StringRef& lhs, const std::string& rhs) {
        return lhs.get_string() + rhs;
    }

    /// @brief The string pool manager (singleton), used mainly for translation
    /// purpouses. But also helps to reduce the memory size of various objects.
    class StringManager {
        std::vector<std::string> strings;
    public:
        StringManager() = default;
        ~StringManager() = default;
        
        inline Eng3D::StringRef insert(const std::string& str) {
            const std::scoped_lock lock(this->strings_mutex);
            this->strings.push_back(str);
            this->strings[this->strings.size() - 1].shrink_to_fit();
            return Eng3D::StringRef(this->strings.size() - 1);
        }

        inline const std::string& get_by_id(const Eng3D::StringRef ref) const {
            const std::scoped_lock lock(this->strings_mutex);
            return this->strings[ref.id];
        }

        static StringManager& get_instance();
        mutable std::mutex strings_mutex;
    };
};
