// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      main.cpp
//
// Abstract:
//      Code glue for entry points of the game on various platforms.
// ----------------------------------------------------------------------------

#include <cstring>
#include <stdexcept>
#include "eng3d/log.hpp"
#include "eng3d/utils.hpp"
#ifdef WIN32
#   include <windows.h>
#endif

#ifdef __ANDROID_API__
#   include <android_native_app_glue.h>
#   define APPNAME "baseapp"
void android_main(struct android_app* state)
{
    main(0, { NULL });
    ANativeActivity_finish(state->activity);
}
#elif defined __switch__
/// @brief Switch doesn't define a pathconf function so this is a kludge
/// while respecting newlib's stuff
/// @param pathname Path to the file
/// @param varcode Property to get value of
/// @return Value of the queried property
extern "C" long pathconf(const char *pathname, int varcode) {
    if(pathname == NULL)
        return 0;
    switch(varcode) {
    case _PC_LINK_MAX:
        return 1;
    case _PC_MAX_CANON:
        return 512;
    case _PC_MAX_INPUT:
        return 512;
    case _PC_NAME_MAX:
        return 512;
    case _PC_PATH_MAX:
        return 512;
    case _PC_PIPE_BUF:
        return 512;
    case _PC_CHOWN_RESTRICTED:
        return 1;
    case _PC_NO_TRUNC:
        return 1;
    case _PC_VDISABLE:
        return 1;
    }
    return 0;
}

extern "C" long sysconf(int) {
    return 0;
}
#elif defined WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch(fdwReason) {
    case DLL_PROCESS_ATTACH:
//      auto game_main = (MainFn)GetProcAddress(GetModuleHandle(nullptr), "game_main");
//      if(game_main == nullptr)
//         CXX_THROW(std::runtime_error, translate("No game_main found!"));
//      game_main(__argc, __argv);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if(lpvReserved != nullptr) break;
        break;
    }
    return TRUE;
}
#endif
