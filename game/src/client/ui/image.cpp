// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
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
//      client/ui/image.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/ui/image.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/state.hpp"

using namespace UI;

Image::Image(int _x, int _y, unsigned w, unsigned h, const UnifiedRender::Texture* tex, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = tex;
}

Image::Image(int _x, int _y, unsigned w, unsigned h, const std::string& texture_path, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get(texture_path));
}

Image* Image::make_transparent(int x, int y, unsigned w, unsigned h, const std::string& tex_path, Widget* parent) {
    UnifiedRender::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    auto texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get(tex_path), no_drop_options);
    Image* image = new Image(x, y, w, h, texture, parent);
    image->is_transparent = true;
    return image;
}