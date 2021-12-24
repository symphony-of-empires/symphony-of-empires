#pragma once

#include <string>
#include <vector>

namespace Path {
    std::string get(const std::string& str);
    std::vector<std::string> get_all(const std::string& str);

    std::string get_full(void);
    void add_path(const std::string& path);
    std::vector<std::string> get_data(const std::string& str);
    std::vector<std::string> get_paths(void);
    std::vector<std::string> get_all_recursive(const std::string& str);
};