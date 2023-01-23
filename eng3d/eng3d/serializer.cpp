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
//      serializer.cpp
//
// Abstract:
//      Provides serializer functions for archives.
// ----------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <iterator>
#include <memory>
#include "eng3d/serializer.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"
#include "eng3d/compress.hpp"

constexpr char archive_signature[4] = { '>', ':', ')', ' ' };

#define MAX_CHUNK_SIZE (65536 * 128)
#define MAX_ARCHIVE_SIZE (65536 * 10000)
#define MIN_FILE_SIZE 4096

void Eng3D::Deser::Archive::to_file(const std::string& path) {
    Eng3D::Log::debug("archive", translate_format("Writing archive %s", path.c_str()));
    if(buffer.empty())
        CXX_THROW(Eng3D::Deser::Exception, translate("Can't output an empty archive to file"));
    
    std::unique_ptr<FILE, decltype(&std::fclose)> fp(::fopen(path.c_str(), "wb"), ::fclose);

    char signbuf[sizeof(archive_signature)];
    std::memcpy(signbuf, archive_signature, sizeof(archive_signature));
    std::fwrite(signbuf, 1, sizeof(signbuf), fp.get());
    uint32_t inf_len = buffer.size();
    std::fwrite(&inf_len, 1, sizeof(inf_len), fp.get());
    std::vector<uint8_t> dest_buffer(std::max<size_t>(buffer.size(), MIN_FILE_SIZE));
    auto r = Eng3D::Zlib::compress(buffer.data(), buffer.size(), dest_buffer.data(), dest_buffer.size());
    dest_buffer.resize(r);
    uint32_t def_len = dest_buffer.size();
    std::fwrite(&def_len, 1, sizeof(def_len), fp.get());
    std::fwrite(dest_buffer.data(), 1, dest_buffer.size(), fp.get());
    Eng3D::Log::debug("archive", string_format("%zu->%zu bytes compressed; return value is %zu", inf_len, def_len, r));
}

void Eng3D::Deser::Archive::from_file(const std::string& path) {
    Eng3D::Log::debug("archive", translate_format("Reading archive %s", path.c_str()));

    std::unique_ptr<FILE, decltype(&std::fclose)> fp(::fopen(path.c_str(), "rb"), ::fclose);
    if(fp == nullptr) CXX_THROW(std::runtime_error, translate("Can't read archive"));
    
    std::vector<uint8_t> src_buffer(buffer.size());
    
    char signbuf[sizeof(archive_signature)];
    std::fread(signbuf, 1, sizeof(signbuf), fp.get());
    if(std::memcmp(archive_signature, signbuf, sizeof(signbuf)) != 0)
        CXX_THROW(std::runtime_error, "Invalid archive");
    uint32_t inf_len;
    std::fread(&inf_len, 1, sizeof(inf_len), fp.get());
    uint32_t def_len;
    std::fread(&def_len, 1, sizeof(def_len), fp.get());
    if(def_len >= MAX_ARCHIVE_SIZE) CXX_THROW(std::runtime_error, "Exceeded archive size");
    src_buffer.resize(def_len);
    std::fread(src_buffer.data(), 1, src_buffer.size(), fp.get());

    buffer.resize(inf_len);
    auto r = Eng3D::Zlib::decompress(src_buffer.data(), src_buffer.size(), buffer.data(), buffer.size());
    Eng3D::Log::debug("archive", string_format("%zu<-%zu bytes decompressed; return value is %zu", inf_len, def_len, r));
    buffer.shrink_to_fit();
}

void Eng3D::Deser::Archive::copy_to(void* to_ptr, size_t size) {
    if(size > buffer.size() - this->ptr)
        CXX_THROW(Eng3D::Deser::Exception, string_format("Buffer too small for write of %zu bytes", size));
    std::memcpy(to_ptr, &buffer[this->ptr], size);
    this->ptr += size;
}

void Eng3D::Deser::Archive::copy_from(const void* from_ptr, size_t size) {
    this->expand(size);
    if(size > buffer.size() - this->ptr)
        CXX_THROW(Eng3D::Deser::Exception, string_format("Buffer too small for read of %zu bytes", size));
    std::memcpy(&buffer[this->ptr], from_ptr, size);
    this->ptr += size;
}
