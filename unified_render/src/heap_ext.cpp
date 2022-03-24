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
//      heap_ext.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#ifdef UR_MANAGED_HEAP
#include <cstdlib>
#include <new>

#include "unified_render/heap_ext.hpp"

void* operator new(std::size_t size) {
#ifdef UR_HEAP_DEBUG
    std::printf("Alloc size = %zu\n", size);
#endif
    if(size == 0) {
        size++;
    }

    void* p = std::malloc(size);
    if(p) {
        return p;
    }
    throw std::bad_alloc{};
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

void* operator new[](std::size_t size) {
#ifdef UR_HEAP_DEBUG
    std::printf("Alloc[] size = %zu\n", size);
#endif
    if(size == 0) {
        size++;
    }

    void* p = std::malloc(size);
    if(p) {
        return p;
    }
    throw std::bad_alloc{};
}

void operator delete[](void* ptr) noexcept {
    std::free(ptr);
}
#endif
