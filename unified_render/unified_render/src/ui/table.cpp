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
//      unified_render/ui/table.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <GL/glew.h>
#include <GL/gl.h>

#include "unified_render/ui/table.hpp"
#include "unified_render/ui/widget.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/div.hpp"
#include "unified_render/ui/label.hpp"
#include "unified_render/ui/scrollbar.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

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
    if(this->key_type != right.key_type) {
        return this->key_type < right.key_type;
    }
    switch(this->key_type) {
    case KeyType::NUMBER:
        return this->key_number < right.key_number;
    case KeyType::STRING:
        return this->key_string < right.key_string;
    case KeyType::NONE:
        return false;
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
}

TableElement* TableRow::get_element(size_t index) {
    if(index > elements.size()) {
        throw std::runtime_error("Table - Index out of bounds");
    }
    return elements[index];
}

Table::Table(int _x, int _y, unsigned _w, unsigned _h, int _row_height, std::vector<int> _widths, std::vector<std::string> _header_labels, Widget* _parent)
    : Widget(_parent, _x, _y, _w, _h, UI::WidgetType::TABLE),
    row_height{ _row_height }, columns_width{ _widths }
{
    if(_widths.size() != _header_labels.size()) {
        throw std::runtime_error("Table width & header mismatched!");
    }
    this->width = 20;
    for(size_t i = 0; i < _widths.size(); i++) {
        this->width += _widths[i];
    }

    auto* header = new TableRow(this, this->width, _row_height, this->columns_width);
    for(size_t i = 0; i < _header_labels.size(); i++) {
        auto& label = _header_labels[i];
        auto* column = header->get_element(i);
        column->text(label);
        column->set_on_click([this, i](Widget&) {
            if(this->sorting_row == i) {
                this->sorting_ascending = !this->sorting_ascending;
            }
            else {
                this->sorting_ascending = true;
            }
            this->sorting_row = i;
            this->sort(i, this->sorting_ascending);
        });
    }

    this->is_scroll = true;
    this->on_update = [this](Widget&) {
        int total_height = this->row_height * this->rows.size();
        this->column_wrapper->height = total_height;
    };

    this->column_wrapper = new UI::Div(0, _row_height, this->width, 0, this);
    this->column_wrapper->flex = UI::Flex::COLUMN;
    this->column_wrapper->flex_justify = UI::FlexJustify::START;
    this->scrollbar = new UI::Scrollbar(this->width - 20, _row_height, 20, this->height, this);
}

void Table::reserve(size_t _size) {
    this->rows.reserve(_size);
}

TableRow* Table::get_row(size_t _row_id) {
    if(!this->rows.count(_row_id)) {
        auto* row = new TableRow(this->column_wrapper, this->width, this->row_height, this->columns_width);
        this->rows.insert({ _row_id, row });
    }
    auto row =this->rows[_row_id];
    row->is_active = true;
    return row;
}

void Table::remove_row(size_t _row_id) {
    if(this->rows.count(_row_id)) {
        this->rows[_row_id]->kill();
        this->rows.erase(_row_id);
    }
}

void Table::clear_unactive_rows() {
    for(auto it = begin(rows); it != end(rows);)
    {
        if(!it->second->is_active)
        {
            it->second->kill();
            it = rows.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Table::sort(size_t _column_index, bool ascending) {
    auto comp = [_column_index, ascending](const std::unique_ptr<Widget>& a, const std::unique_ptr<Widget>& b) {
        TableRow* row_a = static_cast<TableRow*>(a.get());
        TableElement& element_a  = *(row_a->get_element(_column_index));
        TableRow* row_b = static_cast<TableRow*>(b.get());
        TableElement& element_b  = *(row_b->get_element(_column_index));
        if(ascending)
            return element_a < element_b;
        else
            return !(element_a < element_b);
    };
    this->column_wrapper->sort_children(comp);
}
