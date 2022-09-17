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
//      main.cpp
//
// Abstract:
//      Code glue for entry points of the game on various platforms.
// ----------------------------------------------------------------------------

#include <cstring>

extern "C" int main(int argc, char** argv);

#ifdef E3D_TARGET_ANDROID
#   include <android_native_app_glue.h>
#   define APPNAME "baseapp"
void android_main(struct android_app* state)
{
    main(0, { NULL });
    ANativeActivity_finish(state->activity);
}
#endif
#ifdef E3D_TARGET_WINDOWS
#	include <windows.h>
/// @brief Stub to transform the WinMain into a proper call for main so the game doesn't
/// even notice we're on windows!
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgument, int iShow) {
    char* argv[1];
    argv[0] = new char[2];
    strcpy((char*)argv[0], "/");
    main(1, argv);
    free(argv[0]);
    return 0;
}
#endif
#ifdef E3D_TARGET_SWITCH
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
#endif
