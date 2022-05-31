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
//      eng3d/ui/table.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <GL/glew.h>
#include <GL/gl.h>

#include "eng3d/ui/table.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

using namespace UI;

TableElement::TableElement(TableRow* _parent, int _width, int _height)
    : Widget(_parent, 0, 0, _width, _height, UI::WidgetType::TABLE_ELEMENT)
{
    this->text_align_x = UI::Align::END;
}

void TableElement::set_key(std::string key) {
    key_type = KeyType::STRING;
    key_string = key;
}

void TableElement::set_key(float key) {
    key_type = KeyType::NUMBER;
    key_number = key;
}

bool TableElement::operator< (const TableElement& right) const {
    if(this->key_type != right.key_type)
        return this->key_type < right.key_type;
    
    switch(this->key_type) {
    case KeyType::NUMBER: return this->key_number < right.key_number;
    case KeyType::STRING: return this->key_string < right.key_string;
    case KeyType::NONE: return false;
    }
}

TableRow::TableRow(Widget* _parent, int _width, int _height, std::vector<int>& _columns_width)
    : Widget(_parent, 0, 0, _width, _height, UI::WidgetType::TABLE_ROW),
    columns_width{ _columns_width }
{
    this->flex = UI::Flex::ROW;
    this->flex_justify = UI::FlexJustify::START;
    for(size_t i = 0; i < this->columns_width.size(); i++) {
        auto element_width = columns_width[i];
        auto* element = new TableElement(this, element_width, this->height);
        elements.push_back(element);
    }
    this->on_update = [this](Widget&) {
        this->is_active = false;
    };

    glm::ivec2 size(5, 5);
    glm::ivec2 texture_size(63, 63);
    auto border = UI::Border(nullptr, size, texture_size);
    this->border = border;
    this->on_pos_recalc = [this](Widget&, int index) {
        auto tex_man = Eng3D::State::get_instance().tex_man;
        Eng3D::TextureOptions tex_options{};
        tex_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        tex_options.mag_filter = GL_LINEAR;
        auto border_tex = tex_man->load(Path::get("gfx/test.png"), tex_options);
        if(index % 2 == 0) {
            this->border.texture = border_tex;
        } else {
            this->border.texture = nullptr;
        }
    };
}

TableElement* TableRow::get_element(size_t index) {
    CXX_THROW(std::runtime_error, "Table - Index out of bounds");
    return elements[index];
}
