// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      Does some important stuff.
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

void Archive::to_file(const std::string& path) {
    Eng3D::Log::debug("archive", translate_format("Writing archive %s", path.c_str()));
    if(buffer.empty())
        CXX_THROW(SerializerException, translate("Can't output an empty archive to file"));
    
    std::unique_ptr<FILE, decltype(&std::fclose)> fp(::fopen(path.c_str(), "wb"), ::fclose);

    char signbuf[sizeof(archive_signature)];
    std::memcpy(signbuf, archive_signature, sizeof(archive_signature));
    std::fwrite(signbuf, 1, sizeof(signbuf), fp.get());
    uint32_t len = buffer.size();
    std::fwrite(&len, 1, sizeof(len), fp.get());

    std::vector<uint8_t> dest_buffer(buffer.size());
    auto r = Eng3D::Zlib::compress(buffer.data(), buffer.size(), dest_buffer.data(), dest_buffer.size());
    dest_buffer.resize(r);
    std::fwrite(dest_buffer.data(), 1, dest_buffer.size(), fp.get());
    Eng3D::Log::debug("archive", string_format("%zu bytes compressed; return value is %zu", len, r));
}

void Archive::from_file(const std::string& path) {
    Eng3D::Log::debug("archive", translate_format("Reading archive %s", path.c_str()));

    std::unique_ptr<FILE, decltype(&std::fclose)> fp(::fopen(path.c_str(), "rb"), ::fclose);
    if(fp == nullptr) CXX_THROW(std::runtime_error, translate("Can't read archive"));
    
    std::vector<uint8_t> src_buffer(buffer.size());
    std::fseek(fp.get(), 0, SEEK_END);
    src_buffer.resize(std::ftell(fp.get()));
    std::rewind(fp.get());

    char signbuf[sizeof(archive_signature)];
    std::fread(signbuf, 1, sizeof(signbuf), fp.get());
    if(memcmp(archive_signature, signbuf, sizeof(signbuf)) != 0)
        CXX_THROW(std::runtime_error, "Invalid archive");

    uint32_t len;
    std::fread(&len, 1, sizeof(len), fp.get());

    src_buffer.resize(src_buffer.size() - (sizeof(signbuf) + sizeof(len)));
    std::fread(src_buffer.data(), 1, src_buffer.size(), fp.get());

    buffer.resize(len);
    auto r = Eng3D::Zlib::decompress(src_buffer.data(), src_buffer.size(), buffer.data(), buffer.size());
    Eng3D::Log::debug("archive", string_format("%zu bytes decompressed; return value is %zu", len, r));
    
    buffer.shrink_to_fit();
}
