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
//      io.cpp
//
// Abstract:
//      Generalized "IO" functions for more abstract access to the OS resources
//      along with a custom "std::filesystem" implementation to aid and
//      implement custom paths.
// ----------------------------------------------------------------------------

#include "eng3d/io.hpp"
#include "eng3d/utils.hpp"

//
// IO::Path
//
// The path class abstracts away most of the burden from handling system-dependant
// filesystem paths
//
Eng3D::IO::Path::Path() {

}

Eng3D::IO::Path::Path(const std::string& path)
    : str(path)
{

}

Eng3D::IO::Path::Path(const char* path)
    : str(std::string(path))
{

}

Eng3D::IO::Path::~Path() {

}

//
// Asset::File
//
// A "file" version of the base asset, mostly to identify an asset on a physical disk
//
void Eng3D::IO::Asset::File::open() {
    this->fp = ::fopen(abs_path.c_str(), "rb");
    //if(fp == nullptr)
    //    throw std::runtime_error("Can't open file " + path);
}

void Eng3D::IO::Asset::File::close() {
    ::fclose(this->fp);
}

void Eng3D::IO::Asset::File::read(void* buf, size_t n) {
    ::fread(buf, 1, n, this->fp);
}

void Eng3D::IO::Asset::File::write(const void* buf, size_t n) {
    ::fwrite(buf, 1, n, this->fp);
}

void Eng3D::IO::Asset::File::seek(SeekType type, int offset) {
    if(type == SeekType::CURRENT) {
        ::fseek(this->fp, offset, SEEK_CUR);
    } else if(type == SeekType::START) {
        ::fseek(this->fp, offset, SEEK_SET);
    } else if(type == SeekType::END) {
        ::fseek(this->fp, offset, SEEK_END);
    }
}

//
// Package
//
#include <filesystem>
#include "eng3d/path.hpp"
//
// Package manager
//
Eng3D::IO::PackageManager::PackageManager(const std::vector<std::string>& pkg_paths) {
    if(pkg_paths.empty()) {
        const std::string asset_path = ::Path::get_full();

        /// @todo Replace with a custom filesystem implementation
        // Register packages
        for(const auto& entry : std::filesystem::directory_iterator(asset_path)) {
            if(!entry.is_directory()) continue;
            auto package = Eng3D::IO::Package();
            package.name = entry.path().lexically_relative(asset_path).string();
            for(const auto& _entry : std::filesystem::recursive_directory_iterator(entry.path())) {
                if(_entry.is_directory()) continue;
                std::shared_ptr<Eng3D::IO::Asset::File> asset = std::make_shared<Eng3D::IO::Asset::File>();
                asset->path = _entry.path().lexically_relative(entry.path()).string();
                asset->abs_path = _entry.path().string();
#ifdef E3D_TARGET_WINDOWS
                std::replace(asset->path.begin(), asset->path.end(), '\\', '/');
                std::replace(asset->abs_path.begin(), asset->abs_path.end(), '\\', '/');
#endif
                package.assets.push_back(asset);
            }
            packages.push_back(package);
        }
    } else {
        /// @todo Replace with a custom filesystem implementation
        // Register packages using pkg_paths
        for(const auto& entry : pkg_paths) {
            auto package = Eng3D::IO::Package();
            package.name = entry;
            for(const auto& _entry : std::filesystem::recursive_directory_iterator(entry)) {
                if(_entry.is_directory()) continue;
                std::shared_ptr<Eng3D::IO::Asset::File> asset = std::make_shared<Eng3D::IO::Asset::File>();
                asset->path = _entry.path().lexically_relative(entry).string();
                asset->abs_path = _entry.path().string();
#ifdef E3D_TARGET_WINDOWS
                std::replace(asset->path.begin(), asset->path.end(), '\\', '/');
                std::replace(asset->abs_path.begin(), asset->abs_path.end(), '\\', '/');
#endif
                package.assets.push_back(asset);
            }
            packages.push_back(package);
        }
    }
}

Eng3D::IO::PackageManager::~PackageManager() {

}

// Obtaining an unique asset means the "first-found" policy applies
std::shared_ptr<Eng3D::IO::Asset::Base> Eng3D::IO::PackageManager::get_unique(const Eng3D::IO::Path& path) {
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> list = this->get_multiple(path);
    if(list.empty())
        return std::shared_ptr<Eng3D::IO::Asset::Base>(nullptr);
    return list.at(0);
}

std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> Eng3D::IO::PackageManager::get_multiple(const Eng3D::IO::Path& path) {
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> list;

    std::vector<Eng3D::IO::Package>::const_iterator package;
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>>::const_iterator asset;

    for(package = packages.begin(); package != packages.end(); package++) {
        for(asset = (*package).assets.begin(); asset != (*package).assets.end(); asset++) {
            if((*asset)->path == path.str)
                list.push_back(*asset);
        }
    }
    return list;
}