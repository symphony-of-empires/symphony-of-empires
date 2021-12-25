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

    namespace Asset {
        class Base {
        public:
            Base(){};
            ~Base(){};
            virtual void open(void){};
            virtual void close(void){};
            virtual void read(void* buf, size_t n){};
            virtual void write(const void* buf, size_t n){};
            virtual void seek(SeekType type, int offset){};

            std::string path;
            std::string abs_path;
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
            virtual void seek(SeekType type, int offset);
        };
    };

    class Package {
    public:
        Package();
        ~Package();

        std::string name;
        std::vector<Asset::Base*> assets;
    };
};

