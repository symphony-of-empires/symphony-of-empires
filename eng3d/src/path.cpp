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
//      path.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <stdexcept>
#ifdef E3D_TARGET_UNIX
#	include <unistd.h>
#elif defined E3D_TARGET_WINDOWS
#   ifndef WINSOCK2_IMPORTED
#       define WINSOCK2_IMPORTED
#       include <winsock2.h>
#   endif
#	include <windows.h>
#endif
#include <climits>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <filesystem>

/* Visual Studio is not posix so we have to define PATH_MAX ourselves */
#ifndef MAX_PATH
#	define MAX_PATH 255
#endif

#include "eng3d/path.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"

static std::vector<std::string> mod_paths;

namespace Path {
    std::string get_full() {
		return "./mods/";
		
#ifdef E3D_TARGET_WINDOWS
        char buf[MAX_PATH];
        const auto len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
#else
        char buf[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
#endif
        if(len < 0)
            CXX_THROW(std::runtime_error, "Error reading exec path");
        buf[len] = '\0';
        std::string rsult = buf;
        size_t found = rsult.find_last_of("/\\");
        rsult = rsult.substr(0, found);
#ifdef NO_COPY_MODS
        found = rsult.find_last_of("/\\");
        rsult = rsult.substr(0, found);
#endif
        rsult += "/mods/";
        return rsult;
    }
};
