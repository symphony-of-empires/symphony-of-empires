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

namespace Eng3D {
    class State;
}

/// @brief Implements the I/O functions for interacting with assets, please note that
/// this is however outdated because <filesystem> now exists, but we are
/// given more flexibility if we roll our own implementation to make a "mini Virtual-Filesystem"
namespace Eng3D::IO {
    /**
     * @brief The path class abstracts away most of the burden from handling system-dependant
     * filesystem paths
     * 
     */
    class Path {
    public:
        Path() = default;
        Path(const std::string& path)
            : str(path)
        {

        }

        Path(const char* path)
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
    };

    enum class SeekType {
        START,
        END,
        CURRENT,
    };

    namespace Asset {
        class Base {
        public:
            Base() = default;
            ~Base() = default;
            virtual void open() {};
            virtual void close() {};
            virtual void read(void*, size_t) {};
            virtual void write(const void*, size_t) {};
            virtual void seek(Eng3D::IO::SeekType, int) {};
            virtual size_t get_size(void) const { return 0; };

            std::string path;
            std::string abs_path;

            /**
             * @brief Read the entire file into a string
             * 
             * @return std::string The file contents
             */
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

            /**
             * @brief Get the abs path object in a safe manner, such as that the access does not
             * occur on null pointers. Use this function because it also converts slashes
             * between paths
             * 
             * @param asset 
             * @return std::string 
             */
            inline std::string get_abs_path(void) const {
                std::string path = this->abs_path;
#ifdef E3D_TARGET_WINDOWS
                std::replace(path.begin(), path.end(), '/', '\\');
#endif
                return path;
            }
        };

        /**
         * @brief A "file" version of the base asset, mostly to identify an asset on a physical disk
         * 
         */
        class File : public Asset::Base {
        public:
            FILE* fp;
        //public:
            File() = default;
            ~File() = default;
            virtual void open();
            virtual void close();
            virtual void read(void* buf, size_t n);
            virtual void write(const void* buf, size_t n);
            virtual void seek(Eng3D::IO::SeekType type, int offset);
            virtual size_t get_size(void) const;
        };
    };

    class PackageException : public std::exception {
        std::string buffer;
    public:
        PackageException(const std::string& _buffer) : buffer(_buffer) {};
        virtual const char* what() const noexcept {
            return buffer.c_str();
        }
    };
    
    // A package containing a set of assets
    class Package {
    public:
        Package() = default;
        ~Package() = default;

        std::string name;
        std::string abs_path; // Absolute path of this package root
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> assets;
    };

    class PackageManager {
        Eng3D::State& s;
    public:
        PackageManager() = delete;
        PackageManager(Eng3D::State& s, const std::vector<std::string>& pkg_paths);
        ~PackageManager() = default;
        void recursive_filesystem_walk(Eng3D::IO::Package& package, const std::string& root, const std::string& current);
        std::shared_ptr<Eng3D::IO::Asset::Base> get_unique(const IO::Path& path);
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> get_multiple(const Eng3D::IO::Path& path);
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> get_multiple_prefix(const Eng3D::IO::Path& path);
        std::vector<std::string> get_paths(void) const;
        
        std::vector<Package> packages;
    };
};
