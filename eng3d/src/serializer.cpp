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
#include "eng3d/log.hpp"

void Archive::to_file(const std::string& path) {
    if(buffer.empty())
        CXX_THROW(SerializerException, "Can't output an empty archive to file " + path);
    Eng3D::Log::debug("fs", "Writing archive " + path);
    std::unique_ptr<FILE, decltype(&std::fclose)> fp(::fopen(path.c_str(), "wb"), ::fclose);
    std::fwrite(buffer.data(), 1, buffer.size(), fp.get());
}

void Archive::from_file(const std::string& path) {
    Eng3D::Log::debug("fs", "Reading archive " + path);
    std::unique_ptr<FILE, decltype(&std::fclose)> fp(::fopen(path.c_str(), "rb"), ::fclose);
    if(fp == nullptr)
        CXX_THROW(std::runtime_error, "Can't read an empty archive " + path);
    std::fseek(fp.get(), 0, SEEK_END);
    buffer.resize(std::ftell(fp.get()));
    std::rewind(fp.get());
    std::fread(buffer.data(), 1, buffer.size(), fp.get());
    buffer.shrink_to_fit();
}
