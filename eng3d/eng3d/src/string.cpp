// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      string.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/string.hpp"

//
// StringRef
//
Eng3D::StringRef::StringRef(const std::string& str) {
    *this = Eng3D::StringManager::get_instance().insert(str);
}

static Eng3D::String g_empty_str = " ";
const Eng3D::String& Eng3D::StringRef::get_string(void) const
{
    if(this->id == (Eng3D::StringRef::Id)-1) {
        return g_empty_str;
    }
    return Eng3D::StringManager::get_instance().get_by_id(this->id);
}

//
// StringManager
//
static Eng3D::StringManager g_string_man;
Eng3D::StringManager& Eng3D::StringManager::get_instance(void)
{
    return g_string_man;
}
