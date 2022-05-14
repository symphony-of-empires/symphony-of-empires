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

namespace Eng3D::IO {
    class Path {
    public:
        Path();
        Path(const std::string& path);
        Path(const char* path);
        ~Path();
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
            Base();
            ~Base();
            virtual void open(void);
            virtual void close(void);
            virtual void read(void*, size_t);
            virtual void write(const void*, size_t);
            virtual void seek(Eng3D::IO::SeekType, int);

            std::string path;
            std::string abs_path;

            /**
             * @brief Get the abs path object in a safe manner, such as that the access does not
             * occur on null pointers. Use this function because it also converts slashes
             * between paths
             * 
             * @param asset 
             * @return std::string 
             */
            static inline std::string get_abs_path(Eng3D::IO::Asset::Base* asset) {
                std::string path = asset != nullptr ? asset->abs_path : "";
#ifdef E3D_TARGET_WINDOWS
                std::replace(path.begin(), path.end(), '/', '\\');
#endif
                return path;
            };
        };

        class File : public Asset::Base {
        public:
            FILE* fp;
        //public:
            File();
            ~File();
            virtual void open(void);
            virtual void close(void);
            virtual void read(void* buf, size_t n);
            virtual void write(const void* buf, size_t n);
            virtual void seek(Eng3D::IO::SeekType type, int offset);
        };
    };

    class PackageException : public std::exception {
        std::string buffer;
    public:
        PackageException(const std::string& _buffer) : buffer(_buffer) {};
        virtual const char* what(void) const noexcept {
            return buffer.c_str();
        }
    };
    
    // A package containing a set of assets
    class Package {
    public:
        Package();
        ~Package();

        std::string name;
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> assets;
    };

    class PackageManager {
    public:
        PackageManager();
        ~PackageManager();
        std::shared_ptr<Eng3D::IO::Asset::Base> get_unique(const IO::Path& path);
        std::vector<std::shared_ptr<Eng3D::IO::Asset::Base>> get_multiple(const Eng3D::IO::Path& path);
        std::vector<Package> packages;
    };
};
