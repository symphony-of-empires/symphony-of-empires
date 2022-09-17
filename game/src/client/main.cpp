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

/// @brief Stub to transform the WinMain into a proper call for main so the game doesn't
/// even notice we're on windows!
#ifdef E3D_TARGET_WINDOWS
#   include <windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
extern "C" int main(int argc, char** argv)
#endif
{
    try {
#ifdef E3D_TARGET_WINDOWS
        start_client(__argc, __argv);
#else
        start_client(argc, argv);
#endif
    } catch(const std::exception& e) {
        Eng3D::Log::error("game", e.what());
        exit(EXIT_FAILURE);
    }
    return 0;
}
