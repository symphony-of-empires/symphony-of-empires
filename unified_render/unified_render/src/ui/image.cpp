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
//      unified_render/ui/image.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/ui/image.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/state.hpp"

using namespace UI;

Image::Image(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    
}

Image::Image(int _x, int _y, unsigned w, unsigned h, std::shared_ptr<UnifiedRender::Texture> tex, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = tex;
}

Image::Image(int _x, int _y, unsigned w, unsigned h, const std::string& texture_path, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = UnifiedRender::State::get_instance().tex_man->load(Path::get(texture_path));
}

Image::Image(int _x, int _y, unsigned w, unsigned h, const std::string& texture_path, bool mipmap, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    UnifiedRender::TextureOptions options;
    if(mipmap) {
        options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        options.mag_filter = GL_LINEAR;
    }
    current_texture = UnifiedRender::State::get_instance().tex_man->load(Path::get(texture_path), options);
}

Image* Image::make_transparent(int x, int y, unsigned w, unsigned h, const std::string& tex_path, Widget* parent) {
    return make_transparent(x, y, w, h, tex_path, false, parent);
}

Image* Image::make_transparent(int x, int y, unsigned w, unsigned h, const std::string& tex_path, bool mipmap, Widget* parent) {
    UnifiedRender::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    if(mipmap) {
        no_drop_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        no_drop_options.mag_filter = GL_LINEAR;
    }
    auto texture = UnifiedRender::State::get_instance().tex_man->load(Path::get(tex_path), no_drop_options);
    Image* image = new Image(x, y, w, h, texture, parent);
    image->is_transparent = true;
    return image;
}

AspectImage::AspectImage(int _x, int _y, unsigned w, unsigned h, std::shared_ptr<UnifiedRender::Texture> tex, Widget* _parent)
    : Image(_x, _y, w, h, tex, _parent)
{
    //width = w;this
    //height = (float)current_texture->height * ((float)w / (float)current_texture->width);

    this->width = (float)this->current_texture->width * ((float)h / (float)this->current_texture->height);
    this->height = h;
}
