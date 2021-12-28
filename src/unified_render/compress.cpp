#include <zlib.h>
#include "compress.hpp"

size_t get_compressed_len(size_t len) {
    return (len + 6 + (((len + 16383) / 16384) * 5));
}

size_t compress(const void* src, size_t src_len, void* dest, size_t dest_len) {
    z_stream info = {0};
    info.total_in = info.avail_in = src_len;
    info.total_out = info.avail_out = dest_len;
    info.next_in = (Bytef*)src;
    info.next_out = (Bytef*)dest;

    int r;
    r = deflateInit(&info, Z_DEFAULT_COMPRESSION);
    if(r == Z_OK) {
        r = deflate(&info, Z_FINISH);
        if(r == Z_STREAM_END) {
            return info.total_out;
        }
    }
    deflateEnd(&info);
    return info.total_out;
}

size_t decompress(const void* src, size_t src_len, void* dest, size_t dest_len) {
    z_stream info = {0};
    info.total_in = info.avail_in = src_len;
    info.total_out = info.avail_out = dest_len;
    info.next_in = (Bytef*)src;
    info.next_out = (Bytef*)dest;

    int r;
    r = inflateInit(&info);
    if(r == Z_OK) {
        r = inflate(&info, Z_FINISH);
        if(r == Z_STREAM_END) {
            return info.total_out;
        }
    }
    inflateEnd(&info);
    return info.total_out;
}
