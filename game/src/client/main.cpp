// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
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
//      client/main.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdio>
#include <filesystem>
#include "eng3d/io.hpp"
#include "eng3d/path.hpp"
#include "eng3d/network.hpp"
#include "eng3d/locale.hpp"
#include "eng3d/log.hpp"

void start_client(int argc, char** argv);

extern "C" int main(int argc, char** argv) {
#ifndef E3D_TARGET_SWITCH
    // Clean the log files
    if(1) {
        FILE* fp = ::fopen("log.txt", "wt");
        if(fp) {
            ::fputs("=== LOG.TXT ===\n", fp);
            ::fclose(fp);
        }
    }
#endif
#ifndef UNIT_TEST
    try {
        ::start_client(argc, argv);
    } catch(const std::exception& e) {
        Eng3D::Log::error("game", e.what());
        ::exit(EXIT_FAILURE);
    }
#endif
    return 0;
}
