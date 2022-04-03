// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      compress.cpp
//
// Abstract:
//      Functions for compressing a block of memory using the zlib library.
// ----------------------------------------------------------------------------

#include <zlib.h>
#include "unified_render/compress.hpp"

size_t get_compressed_len(size_t len) {
    return (len + 6 + (((len + 16383) / 16384) * 5));
}

size_t compress(const void* src, size_t src_len, void* dest, size_t dest_len) {
    z_stream info = {};
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
    z_stream info = {};
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
