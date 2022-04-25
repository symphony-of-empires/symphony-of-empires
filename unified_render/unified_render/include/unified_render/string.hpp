// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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

namespace UnifiedRender {
    typedef std::string String;

    class StringRef {
    public:
        using Id = uint16_t;
        Id id;

        StringRef(void);
        StringRef(const std::string& str);
        StringRef(Id id);
        ~StringRef(void);
        const std::string& get_string(void) const;

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

        const char *c_str(void) const
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

    class StringManager {
        std::vector<UnifiedRender::String> strings;
    public:
        StringManager(void);
        ~StringManager(void);
        UnifiedRender::StringRef insert(const String& str);
        const String& get_by_id(const UnifiedRender::StringRef ref) const;
        static StringManager& get_instance(void);
    };
};
