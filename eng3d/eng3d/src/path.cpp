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
//      path.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <stdexcept>
#ifdef E3D_TARGET_UNIX
#	include <unistd.h>
#elif defined E3D_TARGET_WINDOWS
#	ifndef _WINDOWS_
#       ifndef NOMINMAX
#	        define NOMINMAX 1
#       endif
#		define WIN32_LEAN_AND_MEAN 1
#		include <windows.h>
#		undef WIN32_LEAN_AND_MEAN
#	endif
#endif
#include <climits>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <filesystem>

/* Visual Studio is not posix so we have to define PATH_MAX ourselves */
#ifndef MAX_PATH
#	define MAX_PATH 255
#endif

#include "eng3d/path.hpp"
#include "eng3d/print.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"

static std::vector<std::string> mod_paths;

namespace Path {
    static inline bool file_exists(const std::string& str) {
        std::ifstream f(str.c_str());
        return f.good();
    }

    std::string get_full(void) {
#ifdef E3D_TARGET_WINDOWS
        char buf[MAX_PATH];
        const auto len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
#else
        char buf[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
#endif
        if(len < 0) {
            CXX_THROW(std::runtime_error, "Error reading exec path");
        }

        buf[len] = '\0';

        std::string rsult = buf;
        size_t found = rsult.find_last_of("/\\");
        rsult = rsult.substr(0, found);
#ifdef NO_COPY_MODS
        found = rsult.find_last_of("/\\");
        rsult = rsult.substr(0, found);
#endif

        rsult += "/mods/";
        return rsult;
    }


    void add_path(const std::string& path) {
        std::string end_path;
        end_path += path;
        end_path += "/";

        mod_paths.push_back(end_path);
    }

    std::string get(const std::string& str) {
        if(str[0] == '/' || str[1] == ':') {
            return str;
        }

        std::string end_path;
        bool file_found = false;
        for(const auto& path : mod_paths) {
            std::string rsult = get_full() + path + str;
            if(file_exists(rsult) == true) {
                end_path += rsult;
                Eng3D::Log::debug("path", "Path " + end_path + " exists");
                file_found = true;
                break;
            }
        }
        if(!file_found) {
            Eng3D::Log::error("path", "Path could not find file " + str);
        }
#ifdef E3D_TARGET_WINDOWS
        std::replace(end_path.begin(), end_path.end(), '/', '\\');
#endif
        return end_path;
    }

    // Gets all paths where a file with this name exists
    std::vector<std::string> get_all(const std::string& str) {
        std::vector<std::string> list;
        if(str[0] == '/' || str[1] == ':') {
            return list;
        }

        for(const auto& path : mod_paths) {
            std::string end_path = get_full() + path + str;
            if(file_exists(end_path) == true) {
#ifdef E3D_TARGET_WINDOWS
                std::replace(end_path.begin(), end_path.end(), '/', '\\');
#endif
                Eng3D::Log::debug("path", "Path " + end_path + " exists");
                list.push_back(end_path);
            }
        }

        if(list.empty()) {
            Eng3D::Log::error("path", "File " + str + " does not exist");
        }
        return list;
    }

    std::vector<std::string> get_data(const std::string& str) {
        std::vector<std::string> files_text;
        if(mod_paths.size() == 0) {
            Eng3D::Log::error("path", "No mods founds");
        }
        bool found = false;
        for(const auto& path : mod_paths) {
            std::string rsult = get_full() + path + str;
#ifdef E3D_TARGET_WINDOWS
            std::replace(rsult.begin(), rsult.end(), '/', '\\');
#endif
            if(file_exists(rsult) == true) {
                std::ifstream ifs(rsult.c_str());
                std::string content;

                ifs.seekg(std::ios::end);
                files_text.reserve(ifs.tellg());
                ifs.seekg(std::ios::beg);
                content.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

                files_text.push_back(content);
                Eng3D::Log::debug("path", "Path exists (added to string list) " + rsult);
                found = true;
            }
        }
        if(!found) {
            Eng3D::Log::debug("path", "Path does not exist so not added " + str);
        }
        return files_text;
    }

    std::vector<std::string> get_paths(void) {
        std::vector<std::string> p_list;
        for(const auto& path : mod_paths) {
            std::string rsult = get_full() + path;
#ifdef E3D_TARGET_WINDOWS
            std::replace(rsult.begin(), rsult.end(), '/', '\\');
#endif
            p_list.push_back(rsult);
        }
        return p_list;
    }

    std::string clean_path(std::string path) {
#ifdef E3D_TARGET_WINDOWS
        std::replace(path.begin(), path.end(), '/', '\\');
#endif
        return path;
    }

    std::vector<std::string> get_all_recursive(const std::string& str) {
        std::vector<std::string> list;
        if(str[0] == '/' || str[1] == ':') {
            return list;
        }

        for(const auto& path : mod_paths) {
            std::string end_path = get_full() + path + str;
            if(std::filesystem::is_directory(end_path)){
                auto entries = std::filesystem::recursive_directory_iterator(end_path);
                for(const auto& entry : entries) {
                    if(entry.is_regular_file()) {
                        list.push_back(clean_path(entry.path().string()));
                    }
                }
            }
        }
        return list;
    }

    std::string cat_strings(const std::vector<std::string>& vec)
    {
        std::string str;
        for(const auto& _str : vec) {
            str += _str;
        }
        return str;
    }
};
