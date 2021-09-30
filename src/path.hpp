#ifndef PATH_H_
#define PATH_H_

#include <string>
#include <vector>

namespace Path {
    std::string get_full(void);
    void add_path(const std::string& path);
    std::string get(const std::string& str);
    std::vector<std::string> get_data(const std::string& str);
    std::vector<std::string> get_paths(void);
    std::string get_dir(const std::string& str);
};

#endif
