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
//      heap_ext.cpp
//
// Abstract:
//      Overrides default new/delete operators.
// ----------------------------------------------------------------------------

#ifdef E3D_MANAGED_HEAP
#include <cstdlib>
#include <new>

#include "eng3d/heap_ext.hpp"
#include "eng3d/utils.hpp"

void* operator new(std::size_t size) {
    if(size == 0) size++;
    void* p = std::malloc(size);
    if(p) return p;
    CXX_THROW(std::bad_alloc);
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

void* operator new[](std::size_t size) {
    if(size == 0) size++;
    void* p = std::malloc(size);
    if(p) return p;
    CXX_THROW(std::bad_alloc);
}

void operator delete[](void* ptr) noexcept {
    std::free(ptr);
}
#endif
