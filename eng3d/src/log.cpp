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
//      log.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdio>
#include <memory>
#include "log.hpp"

#ifdef E3D_LOG_TO_FILE
static std::unique_ptr<FILE, int(*)(FILE*)> log_fp;
#endif
#ifdef E3D_TARGET_SWITCH
struct PrintConsole;
extern "C" void consoleUpdate(PrintConsole* console);
#endif

/// @brief Logs data to a file or console
void Eng3D::Log::log(const std::string& severity, const std::string& category, const std::string& msg) {
#ifdef E3D_LOG_TO_FILE
    if(log_fp == nullptr)
        log_fp = std::unique_ptr<FILE, int (*)(FILE *)>(fopen("log.txt", "a+t"), fclose);
    
    if(log_fp != nullptr)
        fprintf(log_fp.get(), "<%s:%s> %s\n", severity.c_str(), category.c_str(), msg.c_str());
#else
#   ifndef E3D_TARGET_SWITCH
    printf("<%s:%s> %s\n", severity.c_str(), category.c_str(), msg.c_str());
#   else
    char tmpbuf[256];
    snprintf(tmpbuf, sizeof(tmpbuf), "<%s:%s> %s", severity.c_str(), category.c_str(), msg.c_str());
    fprintf(stderr, tmpbuf);
#   endif
#endif
}

void Eng3D::Log::debug(const std::string& category, const std::string& msg) {
#if defined E3D_DEBUG || 1
    Eng3D::Log::log("debug", category, msg);
#endif
}

void Eng3D::Log::warning(const std::string& category, const std::string& msg) {
    Eng3D::Log::log("warning", category, msg);
}

void Eng3D::Log::error(const std::string& category, const std::string& msg) {
    Eng3D::Log::log("error", category, msg);
}
