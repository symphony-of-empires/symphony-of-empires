// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      compress.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <zlib.h>

namespace Eng3D::Zlib {
    size_t compress(const void* src, size_t src_len, void* dest, size_t dest_len) {
        z_stream info = {};
        info.avail_in = src_len;
        info.avail_out = dest_len;
        info.next_in = (Bytef*)src;
        info.next_out = (Bytef*)dest;
        info.data_type = Z_BINARY;

        int r = deflateInit(&info, Z_DEFAULT_COMPRESSION);
        if(r == Z_OK) {
            r = deflate(&info, Z_FINISH);
            if(r == Z_STREAM_END) {
                deflateEnd(&info);
                return info.total_out;
            }
        }
        CXX_THROW(std::runtime_error, "Insufficient zlib output buffer size for deflate");
    }

    size_t decompress(const void* src, size_t src_len, void* dest, size_t dest_len) {
        z_stream info = {};
        info.avail_in = src_len;
        info.avail_out = dest_len;
        info.next_in = (Bytef*)src;
        info.next_out = (Bytef*)dest;
        info.data_type = Z_BINARY;

        int r = inflateInit(&info);
        if(r == Z_OK) {
            r = inflate(&info, Z_FINISH);
            if(r == Z_STREAM_END) {
                inflateEnd(&info);
                return info.total_out;
            }
        }
        CXX_THROW(std::runtime_error, "Insufficient zlib output buffer size for inflate");
    }
}
