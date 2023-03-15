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
//      cfggen.hpp
//
// Abstract:
//      Create configuration header for the game to use
// ----------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <memory>

int main(int argc, char* argv[]) {
    std::string path{ argv[1] };
    std::unique_ptr<FILE, decltype(&std::fclose)> fp(::fopen(path.c_str(), "wb"), ::fclose);

    std::string output;
    output += "#pragma once\n";
#ifdef E3D_BACKEND_OPENGL
    output += "#define E3D_BACKEND_OPENGL 1";
#endif
#ifdef E3D_BACKEND_VULKAN
    output += "#define E3D_BACKEND_VULKAN 1";
#endif
#ifdef E3D_BACKEND_GLES
    output += "#define E3D_BACKEND_GLES 1";
#endif
#ifdef E3D_BACKEND_RGX
    output += "#define E3D_BACKEND_RGX 1";
#endif
#ifdef E3D_MANAGED_HEAP
    output += "#define E3D_MANAGED_HEAP 1";
#endif

#ifdef E3D_BACKEND_D3D9
    output += "#define E3D_BACKEND_D3D9 1";
#endif
#ifdef E3D_BACKEND_D3D10
    output += "#define E3D_BACKEND_D3D10 1";
#endif
#ifdef E3D_BACKEND_D3D11
    output += "#define E3D_BACKEND_D3D11 1";
#endif
    output += "\n";
    return 0;
}
