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
    size_t get_compressed_size(size_t len) {
        return ::compressBound(len);
    }
    
    size_t compress(const void* src, size_t src_len, void* dest, size_t dest_len) {
        return ::compress(static_cast<Bytef*>(dest), &dest_len, static_cast<const Bytef*>(src), src_len);
    }

    size_t decompress(const void* src, size_t src_len, void* dest, size_t dest_len) {
        const auto r = ::uncompress(static_cast<Bytef*>(dest), &dest_len, static_cast<const Bytef*>(src), src_len);
        if(r == Z_OK) return dest_len;
        CXX_THROW(std::runtime_error, "Insufficient zlib output buffer size for inflate");
    }
}
