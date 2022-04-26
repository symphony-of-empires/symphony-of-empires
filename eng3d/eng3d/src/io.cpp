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
Eng3D::IO::Path::Path(void) {

}

Eng3D::IO::Path::Path(const std::string& path)
    : str(path)
{

}

Eng3D::IO::Path::Path(const char* path)
    : str(std::string(path))
{

}

Eng3D::IO::Path::~Path(void) {

}

//
// Asset::Base
//
// Base for the Asset stream
//
Eng3D::IO::Asset::Base::Base(void) {

}

Eng3D::IO::Asset::Base::~Base(void) {

}

void Eng3D::IO::Asset::Base::open(void) {

}

void Eng3D::IO::Asset::Base::close(void) {

}

void Eng3D::IO::Asset::Base::read(void* buf, size_t n) {

}

void Eng3D::IO::Asset::Base::write(const void* buf, size_t n) {

}

void Eng3D::IO::Asset::Base::seek(SeekType type, int offset) {

}

//
// Asset::File
//
// A "file" version of the base asset, mostly to identify an asset on a physical disk
//
Eng3D::IO::Asset::File::File(void) {

}

Eng3D::IO::Asset::File::~File(void) {

}

void Eng3D::IO::Asset::File::open(void) {
    this->fp = ::fopen(abs_path.c_str(), "rb");
    //if(fp == nullptr)
    //    throw std::runtime_error("Can't open file " + path);
}

void Eng3D::IO::Asset::File::close(void) {
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
Eng3D::IO::Package::Package(void) {

}

Eng3D::IO::Package::~Package(void) {
    //for(const auto& asset : assets) {
    //    delete asset;
    //}
}

#include <filesystem>
#include "eng3d/path.hpp"
//
// Package manager
//
Eng3D::IO::PackageManager::PackageManager(void) {
    const std::string asset_path = ::Path::get_full();
    
    // TODO: Replace with a custom filesystem implementation
    // Register packages
    for(const auto& entry : std::filesystem::directory_iterator(asset_path)) {
        if(!entry.is_directory()) {
            continue;
        }

        auto package = Eng3D::IO::Package();
        package.name = entry.path().lexically_relative(asset_path).string();
        for(const auto& _entry : std::filesystem::recursive_directory_iterator(entry.path())) {
            if(_entry.is_directory()) {
                continue;
            }

            std::shared_ptr<Eng3D::IO::Asset::File> asset = std::make_shared<Eng3D::IO::Asset::File>();
            asset->path = _entry.path().lexically_relative(entry.path()).string();
            asset->abs_path = _entry.path().string();
            package.assets.push_back(asset);
        }
        packages.push_back(package);
    }

    //for(const auto& package : packages) {
    //    for(const auto& asset : package.assets) {
    //        
    //    }
    //}
}

Eng3D::IO::PackageManager::~PackageManager(void) {

}

// Obtaining an unique asset means the "first-found" policy applies
std::shared_ptr<Eng3D::IO::Asset::Base> Eng3D::IO::PackageManager::get_unique(const Eng3D::IO::Path& path) {
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> list = this->get_multiple(path);
    if(list.empty()) {
        return std::shared_ptr<Eng3D::IO::Asset::Base>(nullptr);
    }
    return list.at(0);
}

std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> Eng3D::IO::PackageManager::get_multiple(const Eng3D::IO::Path& path) {
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> list;

    std::vector<Eng3D::IO::Package>::const_iterator package;
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>>::const_iterator asset;

    for(package = packages.begin(); package != packages.end(); package++) {
        for(asset = (*package).assets.begin(); asset != (*package).assets.end(); asset++) {
            if((*asset)->path == path.str) {
                list.push_back(*asset);
            }
        }
    }
    return list;
}