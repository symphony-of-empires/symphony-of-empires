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
//      serializer.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/serializer.hpp"

void Archive::copy_to(void* ptr, size_t size) {
    if(size > buffer.size() - this->ptr) {
        throw SerializerException("Buffer too small for write");
    }

    std::memcpy(ptr, &buffer[this->ptr], size);
    this->ptr += size;
}

void Archive::copy_from(const void* ptr, size_t size) {
    if(size > buffer.size() - this->ptr) {
        throw SerializerException("Buffer too small for read");
    }
    
    std::memcpy(&buffer[this->ptr], ptr, size);
    this->ptr += size;
}

// Expands the archive to fit a new serialized object
void Archive::expand(size_t amount) {
    buffer.resize(buffer.size() + amount);
}

// Call when serialization has ended and it's ready to be sent to a fstream
void Archive::end_stream(void) {
    buffer.shrink_to_fit();
}

void Archive::rewind(void) {
    ptr = 0;
}

void Archive::to_file(const std::string& path) {
    FILE* fp = fopen(path.c_str(), "wb");
    fwrite(&buffer[0], 1, buffer.size(), fp);
    fclose(fp);
}

#include <fstream>
#include <iterator>
#include <cstdint>
void Archive::from_file(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    std::vector<uint8_t> tmpbuf(std::istreambuf_iterator<char>(ifs), {});
    buffer = tmpbuf;
    buffer.shrink_to_fit();
}

void* Archive::get_buffer(void) {
    return (void*)&buffer[0];
}
    
void Archive::set_buffer(void* buf, size_t size) {
    buffer.resize(size);
    std::memcpy(&buffer[0], buf, size);
}
    
size_t Archive::size(void) {
    return buffer.size();
}
