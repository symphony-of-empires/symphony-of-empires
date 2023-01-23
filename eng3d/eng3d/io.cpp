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
//      io.cpp
//
// Abstract:
//      Generalized "IO" functions for more abstract access to the OS resources
//      along with a custom "std::filesystem" implementation to aid and
//      implement custom paths.
// ----------------------------------------------------------------------------

#include <filesystem>
#include "eng3d/io.hpp"
#include "eng3d/state.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"

//
// Asset::File
//
void Eng3D::IO::Asset::File::open() {
    this->fp = std::fopen(abs_path.c_str(), "rb");
    if(fp == nullptr)
        CXX_THROW(std::runtime_error, Eng3D::translate_format("Can't open file %s", path.c_str()));
}

void Eng3D::IO::Asset::File::close() {
    std::fclose(this->fp);
    this->fp = nullptr;
}

void Eng3D::IO::Asset::File::read(void* buf, size_t n) {
    std::fread(buf, 1, n, this->fp);
}

void Eng3D::IO::Asset::File::write(const void* buf, size_t n) {
    std::fwrite(buf, 1, n, this->fp);
}

void Eng3D::IO::Asset::File::seek(SeekType type, int offset) {
    if(type == SeekType::CURRENT) {
        std::fseek(this->fp, offset, SEEK_CUR);
    } else if(type == SeekType::START) {
        std::fseek(this->fp, offset, SEEK_SET);
    } else if(type == SeekType::END) {
        std::fseek(this->fp, offset, SEEK_END);
    }
}

size_t Eng3D::IO::Asset::File::get_size(void) const {
    FILE* cfp = const_cast<FILE*>(this->fp);
    fpos_t pos;
    std::fgetpos(cfp, &pos); // Save
    std::fseek(cfp, 0, SEEK_END); // Go to the end
    long size = ::ftell(cfp); // Get size
    std::fsetpos(cfp, &pos); // Reset
    return static_cast<size_t>(size);
}

//
// Package manager
//
void Eng3D::IO::PackageManager::recursive_filesystem_walk(Eng3D::IO::Package& package, const std::string& root, const std::string& current) {
    // Register paths into our virtual filesystem
    for(const auto& entry : std::filesystem::recursive_directory_iterator(current)) {
        if(entry.is_directory()) continue;
        auto asset = std::make_shared<Eng3D::IO::Asset::File>();
        asset->path = entry.path().lexically_relative(root).string();
        asset->abs_path = entry.path().string();
#ifdef _WIN32
        std::replace(asset->path.begin(), asset->path.end(), '\\', '/');
        std::replace(asset->abs_path.begin(), asset->abs_path.end(), '\\', '/');
#endif
        package.assets.push_back(asset);
    }
}

const std::string_view Eng3D::IO::PackageManager::get_full_path() const {
#ifndef __switch__
#   ifdef NO_COPY_MODS
    return "../mods/";
#   else
    return "./mods/";
#   endif
#else
    return "romfs:/";
#endif
}
