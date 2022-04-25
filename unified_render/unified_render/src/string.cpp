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
//      string.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/string.hpp"

//
// StringRef
//
UnifiedRender::StringRef::StringRef(void)
{
    this->id = (UnifiedRender::StringRef::Id)-1;
}

UnifiedRender::StringRef::StringRef(const UnifiedRender::String& str)
{
    *this = UnifiedRender::StringManager::get_instance().insert(str);
}

UnifiedRender::StringRef::StringRef(UnifiedRender::StringRef::Id id)
{
    this->id = id;
}

UnifiedRender::StringRef::~StringRef(void)
{

}

static UnifiedRender::String g_empty_str = " ";
const UnifiedRender::String& UnifiedRender::StringRef::get_string(void) const
{
    if(this->id == (UnifiedRender::StringRef::Id)-1) {
        return g_empty_str;
    }
    return UnifiedRender::StringManager::get_instance().get_by_id(this->id);
}

//
// StringManager
//
static UnifiedRender::StringManager g_string_man;

UnifiedRender::StringManager::StringManager(void)
{

}

UnifiedRender::StringManager::~StringManager(void)
{

}

UnifiedRender::StringRef UnifiedRender::StringManager::insert(const UnifiedRender::String& str)
{
    this->strings.push_back(str);
    return UnifiedRender::StringRef(this->strings.size() - 1);
}

const UnifiedRender::String& UnifiedRender::StringManager::get_by_id(const UnifiedRender::StringRef ref) const
{
    return this->strings[ref.id];
}

UnifiedRender::StringManager& UnifiedRender::StringManager::get_instance(void)
{
    return g_string_man;
}
