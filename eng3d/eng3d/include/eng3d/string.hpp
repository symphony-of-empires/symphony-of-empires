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

namespace Eng3D {
    typedef std::string String;

    /**
     * @brief A reference to a string on the global string pool
     * 
     */
    class StringRef {
    public:
        using Id = uint16_t;
        Id id;

        StringRef() {
            this->id = (Eng3D::StringRef::Id)-1;
        }

        StringRef(const std::string& str);

        StringRef(Id _id)
            : id{ _id }
        {
            
        }

        ~StringRef() {

        }

        const std::string& get_string() const;

        inline StringRef& operator=(const String& rhs)
        {
            *this = StringRef(rhs);
            return *this;
        };

        inline bool operator==(const StringRef& rhs) const
        {
            return this->get_string() == rhs.get_string();
        };

        inline bool operator==(const String& rhs) const
        {
            return this->get_string() == rhs;
        };

        inline const char *c_str() const
        {
            return this->get_string().c_str();
        };
    };

    inline String operator+(const char *lhs, const StringRef& rhs)
    {
        return String(lhs) + rhs.get_string();
    };

    inline String operator+(const StringRef& lhs, const char *rhs)
    {
        return lhs.get_string() + rhs;
    };

    inline String operator+(const String& lhs, const StringRef& rhs)
    {
        return String(lhs) + rhs.get_string();
    };

    inline String operator+(const StringRef& lhs, const String& rhs)
    {
        return lhs.get_string() + rhs;
    };

    /**
     * @brief The string pool manager (singleton), used mainly for translation
     * purpouses. But also helps to reduce the memory size of various objects.
     * 
     */
    class StringManager {
        std::vector<Eng3D::String> strings;
    public:
        StringManager() {

        }

        ~StringManager() {

        }
        
        inline Eng3D::StringRef insert(const String& str) {
            this->strings.push_back(str);
            return Eng3D::StringRef(this->strings.size() - 1);
        }

        inline const String& get_by_id(const Eng3D::StringRef ref) const {
            return this->strings[ref.id];
        }

        static StringManager& get_instance();
    };
};
