#ifndef ARRAY_OPS_H
#define ARRAY_OPS_H

#include <cstddef>
#include <cstdint>

// (DEPRECATED) Converts a pointer to an index
template<typename T>
constexpr inline size_t ptr_to_index(const T * arr, const T * ptr) noexcept {
	return ((ptrdiff_t)ptr - (ptrdiff_t)arr) / sizeof(T);
}

#endif