#include "unified_render/asset.hpp"
#include <exception>

using namespace UnifiedRender;

Asset::File::Asset(void) {

}

Asset::File::~Asset(void) {

}

void Asset::File::open(void) {
    fp = fopen(abs_path.c_str(), "rb");
    //if(fp == nullptr)
    //    throw std::runtime_error("Can't open file " + path);
}

void Asset::File::close(void) {
    fclose(fp);
}

void Asset::File::read(void* buf, size_t n) {
    fread(buf, 1, n, fp);
}

void Asset::File::write(const void* buf, size_t n) {
    fwrite(buf, 1, n, fp);
}

void Asset::File::seek(SeekType type, int offset) {
    if(type == SeekType::CURRENT) {
        fseek(fp, 0, SEEK_CUR);
    } else if(type == SeekType::START) {
        fseek(fp, 0, SEEK_SET);
    } else if(type == SeekType::END) {
        fseek(fp, 0, SEEK_END);
    }
}

Package::Package(void) {

}

Package::~Package(void) {

}
