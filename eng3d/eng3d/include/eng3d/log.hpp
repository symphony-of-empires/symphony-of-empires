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
//      log.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <memory>

namespace Eng3D::Log {
    inline void debug(const std::string& category, const std::string& msg) {
#if defined E3D_DEBUG || defined GS_DEBUG || 1
#   ifdef E3D_LOG_TO_CONSOLE
        printf("<debug:%s> %s\n", category.c_str(), msg.c_str());
#   endif
        std::unique_ptr<FILE, int(*)(FILE*)> fp(fopen("log.txt", "a+t"), fclose);
        if(fp != nullptr) {
            fprintf(fp.get(), "<debug:%s> %s\n", category.c_str(), msg.c_str());
        }
#endif
    };

    inline void warning(const std::string& category, const std::string& msg) {
        std::unique_ptr<FILE, int(*)(FILE*)> fp(fopen("log.txt", "a+t"), fclose);
        if(fp != nullptr) {
            fprintf(fp.get(), "<warning:%s> %s\n", category.c_str(), msg.c_str());
        }
    };

    inline void error(const std::string& category, const std::string& msg) {
        std::unique_ptr<FILE, int(*)(FILE*)> fp(fopen("log.txt", "a+t"), fclose);
        if(fp != nullptr) {
            fprintf(fp.get(), "<error:%s> %s\n", category.c_str(), msg.c_str());
        }
    };
};
