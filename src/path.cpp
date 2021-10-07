#include <stdexcept>
#ifdef unix
#	include <unistd.h>
#elif defined windows
#	ifndef _WINDOWS_
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
#include "path.hpp"
#include "print.hpp"

/* Visual Studio is not posix so we have to define PATH_MAX ourselves */
#ifndef MAX_PATH
#	define MAX_PATH 255
#endif

static std::vector<std::string> mod_paths;

namespace Path {
    static inline bool file_exists(const std::string& str) {
        std::ifstream f(str.c_str());
        return f.good();
    }

    std::string get_full(void) {
#ifdef windows
        char buf[MAX_PATH];
        const auto len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
#else
        char buf[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
#endif
        if(len < 0)
            throw std::runtime_error("Error reading exec path");
        
        buf[len] = '\0';

        std::string rsult = buf;
        size_t found = rsult.find_last_of("/\\");
        rsult = rsult.substr(0, found);
        found = rsult.find_last_of("/\\");
        rsult = rsult.substr(0, found);

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
        if(str[0] == '/' || str[0] == 'C')
            return str;
        
        std::string end_path;
        for(const auto& path: mod_paths) {
            std::string rsult = get_full() + path + str;
            if(file_exists(rsult) == true) {
                end_path += rsult;
                print_info("Path '%s' exists", end_path.c_str());
                break;
            }
        }
#ifdef windows
        std::replace(end_path.begin(), end_path.end(), '/', '\\');
#endif
        return end_path;
    }

    // Gets all paths where a file with this name exists
    std::vector<std::string> get_all(const std::string& str) {
        std::vector<std::string> list;
        if(str[0] == '/' || str[0] == 'C')
            return list;
        
        for(const auto& path: mod_paths) {
            std::string end_path = get_full() + path + str;
            if(file_exists(end_path) == true) {
#ifdef windows
                std::replace(end_path.begin(), end_path.end(), '/', '\\');
#endif
                print_info("Path '%s' exists", end_path.c_str());
                list.push_back(end_path);
            }
        }
        return list;
    }

    std::vector<std::string> get_data(const std::string& str) {
        std::vector<std::string> files_text;
        for(const auto& path: mod_paths) {
            std::string rsult = get_full() + path + str;
#ifdef windows
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
                print_info("Path '%s' exists (added to string list)", rsult.c_str());
            } else {
                print_info("Path '%s' does not exist so not added", rsult.c_str());
            }
        }
        return files_text;
    }

    std::vector<std::string> get_paths(void) {
        std::vector<std::string> p_list;
        for(const auto& path: mod_paths) {
            std::string rsult = get_full() + path;
#ifdef windows
            std::replace(rsult.begin(), rsult.end(), '/', '\\');
#endif
            p_list.push_back(rsult);
        }
        return p_list;
    }

    std::string get_dir(const std::string& str) {
        if(str[0] == '/' || str[0] == 'C')
            return str;
        
        std::string rsult = get_full() + mod_paths[0] + str;
#ifdef windows
        std::replace(rsult.begin(), rsult.end(), '/', '\\');
#endif
        return rsult;
    }
};
