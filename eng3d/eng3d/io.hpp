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
//      io.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <algorithm>

namespace Eng3D {
    class State;
}

/// @brief Implements the I/O functions for interacting with assets, please note that
/// this is however outdated because <filesystem> now exists, but we are
/// given more flexibility if we roll our own implementation to make a "mini Virtual-Filesystem"
namespace Eng3D::IO {
    /// @brief The path class abstracts away most of the burden from handling system-dependant
    /// filesystem paths
    struct Path {
        Path() = default;
        Path(const std::string_view path)
            : str(path)
        {

        }
        Path(const std::string& path)
            : str(path)
        {

        }
        Path(const char *path)
            : str(path)
        {

        }

        ~Path() = default;
        std::string str;
    };

    namespace StreamFlags {
        enum {
            READ = 0x01,
            WRITE = 0x02,
            TRUNCATE = 0x04,
        };
    }

    enum class SeekType {
        START,
        END,
        CURRENT,
    };

    namespace Asset {
        struct Base {
            virtual ~Base() = default;
            virtual void open() {}
            virtual void close() {}
            virtual void read(void*, size_t) {}
            virtual void write(const void*, size_t) {}
            virtual void seek(Eng3D::IO::SeekType, int) {}
            virtual size_t get_size(void) const { return 0; }
            std::string path;
            std::string abs_path;
            /// @brief Read the entire file into a string
            /// @return std::string The file contents
            inline std::string read_all(void) {
                this->open();
                const size_t size = this->get_size();
                std::string str;
                str.resize(size + 1, ' ');
                this->read(&str[0], size);
                str[size] = '\0';
                this->close();
                return str;
            }
        };

        /// @brief A "file" version of the base asset, mostly to identify an asset on a physical disk
        struct File : Asset::Base {
            FILE* fp = nullptr;
            virtual void open();
            virtual void close();
            virtual void read(void* buf, size_t n);
            virtual void write(const void* buf, size_t n);
            virtual void seek(Eng3D::IO::SeekType type, int offset);
            virtual size_t get_size(void) const;
        };
    }

    class PackageException : public std::exception {
        std::string buffer;
    public:
        PackageException(const std::string_view _buffer) : buffer(_buffer) {}
        virtual const char* what() const noexcept {
            return buffer.c_str();
        }
    };

    /// @brief A package containing a set of assets
    struct Package {
        std::string name;
        std::string abs_path; // Absolute path of this package root
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> assets;
        std::string user_abs_path; // Absolute path for the user files
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> user_assets;
    };

    class PackageManager {
        Eng3D::State& s;
    public:
        PackageManager() = delete;
        PackageManager(Eng3D::State& _s, const std::vector<std::string>& pkg_paths)
            : s{ _s }
        {
            if(pkg_paths.empty()) {
                const std::string_view asset_path = this->get_full_path();
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
        ~PackageManager() = default;
        void recursive_filesystem_walk(Eng3D::IO::Package& package, const std::string_view root, const std::string_view current);
        const std::string_view get_full_path() const;
        
        /// @brief Obtaining an unique asset means the "first-found" policy applies
        /// @param path The path to obtain
        /// @return std::shared_ptr<Eng3D::IO::Asset::Base> Obtained asset object
        std::shared_ptr<Eng3D::IO::Asset::Base> get_unique(const Eng3D::IO::Path& path) {
            std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> list = this->get_multiple(path);
            if(list.empty())
                return std::shared_ptr<Eng3D::IO::Asset::Base>(nullptr);
            return list.at(0);
        }

        /// @brief Obtains multiple assets iff they share a common path (useful for concating
        /// files that might clash, such as lua scripts)
        /// @param path
        /// @return std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>>
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> get_multiple(const Eng3D::IO::Path& path) {
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
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> get_multiple_prefix(const Eng3D::IO::Path& prefix) {
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
        std::vector<std::string> get_paths(void) const {
            std::vector<std::string> paths;
            for(const auto& package : this->packages)
                paths.push_back(package.abs_path);
            return paths;
        }

        std::vector<Package> packages;
        std::string userdata_path;
    };
}
