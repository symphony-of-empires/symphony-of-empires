// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
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
//      eng3d/ui/image.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/image.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/state.hpp"

using namespace UI;

Image::Image(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{

}

Image::Image(int _x, int _y, unsigned w, unsigned h, std::shared_ptr<Eng3D::Texture> tex, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = tex;
}

Image::Image(int _x, int _y, unsigned w, unsigned h, const std::string_view texture_path, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    auto& s = Eng3D::State::get_instance();
    current_texture = s.tex_man.load(s.package_man.get_unique(texture_path));
}

Image::Image(int _x, int _y, unsigned w, unsigned h, const std::string_view texture_path, bool mipmap, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    Eng3D::TextureOptions options;
    if(mipmap) {
        options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
        options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
        options.wrap_s = Eng3D::TextureOptions::Wrap::CLAMP_TO_EDGE;
        options.wrap_t = Eng3D::TextureOptions::Wrap::CLAMP_TO_EDGE;
    }
    auto& s = Eng3D::State::get_instance();
    current_texture = s.tex_man.load(s.package_man.get_unique(texture_path), options);
}

Image* Image::make_transparent(int x, int y, unsigned w, unsigned h, const std::string_view tex_path, Widget* parent) {
    return make_transparent(x, y, w, h, tex_path, false, parent);
}

Image* Image::make_transparent(int x, int y, unsigned w, unsigned h, const std::string_view texture_path, bool mipmap, Widget* parent) {
    Eng3D::TextureOptions no_drop_options{};
    no_drop_options.editable = true;
    if(mipmap) {
        no_drop_options.min_filter = Eng3D::TextureOptions::Filter::LINEAR_MIPMAP;
        no_drop_options.mag_filter = Eng3D::TextureOptions::Filter::LINEAR;
    }
    auto& s = Eng3D::State::get_instance();
    auto texture = s.tex_man.load(s.package_man.get_unique(texture_path), no_drop_options);
    Image* image = new Image(x, y, w, h, texture, parent);
    image->is_transparent = true;
    return image;
}

AspectImage::AspectImage(int _x, int _y, unsigned w, unsigned h, std::shared_ptr<Eng3D::Texture> tex, Widget* _parent)
    : Image(_x, _y, w, h, tex, _parent)
{
    //width = w;this
    //height = (float)current_texture->height * ((float)w / (float)current_texture->width);

    this->width = (float)this->current_texture->width * ((float)h / (float)this->current_texture->height);
    this->height = h;
}
