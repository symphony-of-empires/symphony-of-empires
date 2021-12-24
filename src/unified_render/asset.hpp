#pragma once

#include <cstdio>
#include <vector>
#include <string>

namespace UnifiedRender {
    enum class SeekType {
        START,
        END,
        CURRENT,
    };

    class Asset {
    public:
        FILE* fp;
    //public:
        Asset();
        ~Asset();

        std::string path;
        std::string abs_path;

        virtual void open(void);
        virtual void close(void);
        virtual void read(void* buf, size_t n);
        virtual void write(const void* buf, size_t n);
        virtual void seek(SeekType type, int offset);
    };

    class Package {
    public:
        Package();
        ~Package();

        std::string name;
        std::vector<Asset*> assets;
    };
};

