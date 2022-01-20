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

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <atomic>
#include <future>
#define _XOPEN_SOURCE 700
#include <cstdio>
#include <filesystem>
#if defined windows
#	define WIN32_LEAN_AND_MEAN 1
#   include <windows.h>
#   include <WinCon.h>
#endif
#include <vector>
#include <map>
#include "world.hpp"
#include "unified_render/io.hpp"
#include "unified_render/path.hpp"
#include "unified_render/network.hpp"
#include "unified_render/locale.hpp"
#include "io_impl.hpp"
#include "action.hpp"

void start_client(int argc, char** argv);

int main(int argc, char** argv) {
#if defined windows
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
#endif

    // Clean the log files
    if(1) {
        FILE* fp = fopen("log.txt", "wt");
        if(fp) {
            fputs("=== LOG.TXT ===\n", fp);
            fclose(fp);
        }
    }

    const int seed = time(NULL);
    print_info("Using random seed of %i", seed);
    std::srand(seed);
    
#ifndef UNIT_TEST
    try {
        start_client(argc, argv);
    } catch(const std::exception& e) {
        print_error(e.what());
        exit(EXIT_FAILURE);
    }
#endif
    return 0;
}

#ifdef windows
#include <windows.h>
#include <cstring>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgument, int iShow) {
    char* argv[1];
    argv[0] = new char[2];
    strcpy((char*)argv[0], "/");
    
    main(1, argv);
    
    free(argv[0]);
    return 0;
}
#endif