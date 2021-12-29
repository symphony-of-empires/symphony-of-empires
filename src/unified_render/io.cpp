#include "unified_render/io.hpp"

/*
 * The path class abstracts away most of the burden from handling system-dependant
 * filesystem paths
 */
UnifiedRender::IO::Path::Path(void) {

}

UnifiedRender::IO::Path::Path(const std::string& path)
    : str(path)
{

}

UnifiedRender::IO::Path::~Path(void) {

}

/*
 * Base for the Asset stream
 */
UnifiedRender::IO::Asset::Base::Base(void) {

}

UnifiedRender::IO::Asset::Base::~Base(void) {

}

void UnifiedRender::IO::Asset::Base::open(void) {

}

void UnifiedRender::IO::Asset::Base::close(void) {

}

void UnifiedRender::IO::Asset::Base::read(void* buf, size_t n) {

}

void UnifiedRender::IO::Asset::Base::write(const void* buf, size_t n) {

}

void UnifiedRender::IO::Asset::Base::seek(SeekType type, int offset) {

}

/*
 * A "file" version of the base asset, mostly to identify an asset on a physical disk
 */
UnifiedRender::IO::Asset::File::File(void) {

}

UnifiedRender::IO::Asset::File::~File(void) {

}

void UnifiedRender::IO::Asset::File::open(void) {
    fp = fopen(abs_path.c_str(), "rb");
    //if(fp == nullptr)
    //    throw std::runtime_error("Can't open file " + path);
}

void UnifiedRender::IO::Asset::File::close(void) {
    fclose(fp);
}

void UnifiedRender::IO::Asset::File::read(void* buf, size_t n) {
    fread(buf, 1, n, fp);
}

void UnifiedRender::IO::Asset::File::write(const void* buf, size_t n) {
    fwrite(buf, 1, n, fp);
}

void UnifiedRender::IO::Asset::File::seek(SeekType type, int offset) {
    if(type == SeekType::CURRENT) {
        fseek(fp, 0, SEEK_CUR);
    } else if(type == SeekType::START) {
        fseek(fp, 0, SEEK_SET);
    } else if(type == SeekType::END) {
        fseek(fp, 0, SEEK_END);
    }
}

UnifiedRender::IO::Package::Package(void) {

}

UnifiedRender::IO::Package::~Package(void) {

}