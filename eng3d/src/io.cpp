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
//      io.cpp
//
// Abstract:
//      Generalized "IO" functions for more abstract access to the OS resources
//      along with a custom "std::filesystem" implementation to aid and
//      implement custom paths.
// ----------------------------------------------------------------------------

#include <filesystem>
#include "eng3d/path.hpp"
#include "eng3d/io.hpp"
#include "eng3d/state.hpp"
#include "eng3d/utils.hpp"

//
// Asset::File
//
void Eng3D::IO::Asset::File::open() {
    this->fp = ::fopen(abs_path.c_str(), "rb");
    if(fp == nullptr)
        CXX_THROW(std::runtime_error, "Can't open file " + path);
}

void Eng3D::IO::Asset::File::close() {
    ::fclose(this->fp);
    this->fp = nullptr;
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

size_t Eng3D::IO::Asset::File::get_size(void) const {
    FILE* cfp = const_cast<FILE*>(this->fp);
    fpos_t pos;
    ::fgetpos(cfp, &pos); // Save
    ::fseek(cfp, 0, SEEK_END); // Go to the end
    long size = ::ftell(cfp); // Get size
    ::fsetpos(cfp, &pos); // Reset
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
#ifdef E3D_TARGET_WINDOWS
        std::replace(asset->path.begin(), asset->path.end(), '\\', '/');
        std::replace(asset->abs_path.begin(), asset->abs_path.end(), '\\', '/');
#endif
        package.assets.push_back(asset);
    }
}

//
// PackageManager
//
Eng3D::IO::PackageManager::PackageManager(Eng3D::State& _s, const std::vector<std::string>& pkg_paths)
    : s{ _s }
{
    if(pkg_paths.empty()) {
        const std::string asset_path = ::Path::get_full();
        // All folders inside mods/
        for(const auto& entry : std::filesystem::directory_iterator(asset_path)) {
            if(!entry.is_directory()) continue;
            Eng3D::IO::Package package{};
            package.name = entry.path().lexically_relative(asset_path).string(); // Relative (for nicer names)
            package.abs_path = entry.path().string(); // Absolute
            recursive_filesystem_walk(package, entry.path().string(), entry.path().string());
            this->packages.push_back(package);
        }
    } else {
        // Manually specified paths (can be outside mods/)
        for(const auto& entry : pkg_paths) {
            Eng3D::IO::Package package{};
            package.name = entry;
            package.abs_path = entry;
            recursive_filesystem_walk(package, entry, entry);
            this->packages.push_back(package);
        }
    }
}

/// @brief Obtaining an unique asset means the "first-found" policy applies
/// @param path The path to obtain
/// @return std::shared_ptr<Eng3D::IO::Asset::Base> Obtained asset object
std::shared_ptr<Eng3D::IO::Asset::Base> Eng3D::IO::PackageManager::get_unique(const Eng3D::IO::Path& path) {
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> list = this->get_multiple(path);
    if(list.empty())
        return std::shared_ptr<Eng3D::IO::Asset::Base>(nullptr);
    return list.at(0);
}

/// @brief Obtains multiple assets iff they share a common path (useful for concating
/// files that might clash, such as lua scripts)
/// @param path
/// @return std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>>
std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> Eng3D::IO::PackageManager::get_multiple(const Eng3D::IO::Path& path) {
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> list;
    for(const auto& package : this->packages)
        for(const auto& asset : package.assets)
            if(asset->path == path.str)
                list.push_back(asset);
    return list;
}

/// @brief Obtains all assets starting with a given prefix
/// @param prefix The prefix to check for
/// @return std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>>
std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> Eng3D::IO::PackageManager::get_multiple_prefix(const Eng3D::IO::Path& prefix) {
    std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> list;
    for(const auto& package : this->packages)
        for(const auto& asset : package.assets)
            if(asset->path.substr(0, prefix.str.length()) == prefix.str)
                list.push_back(asset);
    return list;
}

/// @brief Obtain all the paths that are currently under the management of a package, that is
/// return the absolute root directory of all packages
/// @return std::vector<std::string> The list of paths
std::vector<std::string> Eng3D::IO::PackageManager::get_paths(void) const {
    std::vector<std::string> paths;
    for(const auto& package : this->packages)
        paths.push_back(package.abs_path);
    return paths;
}
