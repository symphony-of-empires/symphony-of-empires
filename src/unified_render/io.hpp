#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace UnifiedRender::IO {
    struct Path {
        Path(void);
        Path(const std::string& path);
        ~Path(void);
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
            virtual void seek(SeekType, int);

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
    
    // A package containing a set of assets
    class Package {
    public:
        Package();
        ~Package();

        std::string name;
        std::vector<Asset::Base*> assets;
    };
};
