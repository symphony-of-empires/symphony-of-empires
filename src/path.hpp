#ifndef PATH_H_
#define PATH_H_

#include <string>

namespace Path {
    std::string get_full(void);
    void add_path(const std::string& path);
    std::string get(std::string str);
    std::string get_dir(const std::string& str);
};

#endif
