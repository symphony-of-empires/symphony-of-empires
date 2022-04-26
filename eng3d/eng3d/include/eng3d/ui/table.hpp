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
//      eng3d/ui/table.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>

#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/scrollbar.hpp"
namespace UI {
    /**
     * @ingroup UI
     * @brief Table widget
     *
     */

    class TableRow;
    class TableElement: public Widget {
    public:
        TableElement(TableRow* parent, int width, int height);
        virtual ~TableElement() override {};

        void set_key(std::string key);
        void set_key(float key);
        bool operator< (const TableElement& right) const;
    private:
        enum class KeyType { NUMBER = 0, STRING = 1, NONE = 2 };

        float key_number;
        std::string key_string;
        KeyType key_type = KeyType::NONE;
    };

    class TableRow: public Widget {
    public:
        TableRow(Widget* _parent, int width, int height, std::vector<int>& columns_width);
        virtual ~TableRow() override {};

        TableElement* get_element(size_t index);

        bool is_active = true;
    private:
        std::vector<TableElement*> elements;
        std::vector<int>& columns_width;
    };


    template <typename T>
    class Table: public Widget {
    public:
        Table(int _x, int _y, unsigned _w, unsigned _h, int _row_height, std::vector<int> _widths, std::vector<std::string> _header_labels, Widget* _parent = nullptr)
            : Widget(_parent, _x, _y, _w, _h, UI::WidgetType::TABLE),
            row_height{ _row_height }, columns_width{ _widths }
        {
            if(_widths.size() != _header_labels.size()) {
                throw std::runtime_error("Table width & header mismatched!");
            }
            this->width = 35;
            for(size_t i = 0; i < _widths.size(); i++) {
                this->width += _widths[i];
            }

            auto* header = new TableRow(this, this->width - 35, _row_height, this->columns_width);
            for(size_t i = 0; i < _header_labels.size(); i++) {
                auto& label = _header_labels[i];
                auto* column = header->get_element(i);
                column->text(label);
                column->set_on_click([this, i](Widget&) {
                    if(this->sorting_row == (int)i) {
                        this->sorting_ascending = !this->sorting_ascending;
                    }
                    else {
                        this->sorting_ascending = true;
                    }
                    this->sorting_row = i;
                    this->sort(i, this->sorting_ascending);
                });
            }

            this->on_update = [this](Widget&) {
                int total_height = this->row_height * this->rows.size();
                this->column_wrapper->height = total_height;
            };

            auto wrapper = new UI::Div(0, _row_height, this->width, this->height-row_height, this);
            wrapper->is_scroll = true;
            this->column_wrapper = new UI::Div(0, 0, this->width - 25, 0, wrapper);
            this->column_wrapper->flex = UI::Flex::COLUMN;
            this->column_wrapper->flex_justify = UI::FlexJustify::START;
            this->scrollbar = new UI::Scrollbar(this->width - 20, 0, 20, this->height-row_height, wrapper);
        }
        virtual ~Table() override {};

        void reserve(size_t _size) {
            this->rows.reserve(_size);
        }

        TableRow* get_row(T _row_id) {
            if(!this->rows.count(_row_id)) {
                auto* row = new TableRow(this->column_wrapper, this->width - 25, this->row_height, this->columns_width);
                this->rows.insert({ _row_id, row });
            }
            auto row =this->rows[_row_id];
            row->is_active = true;
            return row;
        }

        void remove_row(T _row_id) {
            if(this->rows.count(_row_id)) {
                this->rows[_row_id]->kill();
                this->rows.erase(_row_id);
            }
        }

        void clear_unactive_rows() {
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

        void sort(size_t _column_index, bool ascending) {
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

    private:
        std::unordered_map<T, TableRow*> rows;

        int sorting_row = -1;
        bool sorting_ascending = true;

        Widget* column_wrapper;
        Scrollbar* scrollbar;

        int row_height;
        std::vector<int> columns_width;
    };
};
