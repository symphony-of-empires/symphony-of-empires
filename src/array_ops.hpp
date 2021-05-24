#ifndef ARRAY_OPS_H
#define ARRAY_OPS_H

#include <cstddef>
#include <cstdint>

template<typename T>
constexpr size_t ptr_to_index(T * arr, T * ptr) {
	return ((ptrdiff_t)ptr - (ptrdiff_t)arr) / sizeof(T);
}

#endif