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
//      eng3d/ui/table.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/table.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

UI::TableElement::TableElement(int _width, int _height, UI::Widget* _parent)
    : UI::Widget(_parent, 0, 0, _width, _height, UI::WidgetType::TABLE_ELEMENT)
{
    this->text_align_x = UI::Align::END;
}

void UI::TableElement::set_key(const std::string_view key) {
    key_type = UI::TableElement::KeyType::STRING;
    key_string = key;
}

void UI::TableElement::set_key(float key) {
    key_type = UI::TableElement::KeyType::NUMBER;
    key_number = key;
}

void UI::TableElement::set_key(float key, const std::string_view format) {
    this->set_text(string_format("%.0f", key));
    key_type = UI::TableElement::KeyType::NUMBER;
    key_number = key;
}

bool UI::TableElement::operator<(const UI::TableElement& right) const {
    if(this->key_type != right.key_type)
        return this->key_type < right.key_type;
    
    switch(this->key_type) {
        case UI::TableElement::KeyType::NUMBER: return this->key_number < right.key_number;
        case UI::TableElement::KeyType::STRING: return this->key_string < right.key_string;
        case UI::TableElement::KeyType::NONE: return false;
    }
    CXX_THROW(std::runtime_error, "TableElement KeyType not supported");
}

UI::TableRow::TableRow(int _width, int _height, std::vector<int>& _columns_width, UI::Widget* _parent)
    : UI::Widget(_parent, 0, 0, _width, _height, UI::WidgetType::TABLE_ROW),
    columns_width{ _columns_width }
{
    this->flex = UI::Flex::ROW;
    this->flex_justify = UI::FlexJustify::START;

    elements.resize(this->columns_width.size());
    std::transform(this->columns_width.cbegin(), this->columns_width.cend(), elements.begin(), [this](const auto e) {
        return &this->make_widget<UI::TableElement>(e, this->height);
    });

    this->on_update = ([this](UI::Widget&) {
        this->is_active = false;
    });

    glm::ivec2 size(5, 5);
    glm::ivec2 texture_size(63, 63);
    this->border = UI::Border(nullptr, size, texture_size);
    this->on_pos_recalc = [this](UI::Widget&, int index) {
        auto& s = Eng3D::State::get_instance();
        auto& tex_man = s.tex_man;
        auto border_tex = tex_man.load(s.package_man.get_unique("gfx/test.png"));
        if(index % 2 == 0) {
            this->border.texture = border_tex;
        } else {
            this->border.texture = nullptr;
        }
    };
}

UI::TableElement* UI::TableRow::get_element(size_t index) {
    assert(index < elements.size());
    return elements[index];
}
