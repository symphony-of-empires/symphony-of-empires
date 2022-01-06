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
//      locale.cpp
//
// Abstract:
//      Does important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/locale.hpp"

std::map<std::string, std::string> trans_msg;

std::string UnifiedRender::Locale::translate(const std::string& str) {
    if(trans_msg[str].empty()) {
        return str;
    }
    return trans_msg[str];
}