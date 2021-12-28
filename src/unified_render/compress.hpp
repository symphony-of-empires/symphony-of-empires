#pragma once

size_t get_compressed_len(size_t len);
size_t compress(const void* src, size_t src_len, void* dest, size_t dest_len);
size_t decompress(const void* src, size_t src_len, void* dest, size_t dest_len);
