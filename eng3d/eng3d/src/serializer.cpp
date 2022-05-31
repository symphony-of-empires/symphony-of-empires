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
//      serializer.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <iterator>
#include <memory>
#include "eng3d/serializer.hpp"
#include "eng3d/utils.hpp"

void Archive::to_file(const std::string& path) {
    if(!buffer.empty()) {
        std::unique_ptr<FILE, int(*)(FILE*)> fp(fopen(path.c_str(), "wb"), fclose);
        fwrite((const void*)&buffer[0], 1, buffer.size(), fp.get());
    }
}

void Archive::from_file(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    std::vector<uint8_t> tmpbuf(std::istreambuf_iterator<char>(ifs), {});
    buffer = tmpbuf;
    buffer.shrink_to_fit();
}