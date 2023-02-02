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
//      eng3d/ui/table.hpp
//
// Abstract:
//      Dynamic table widget, it's sortable and very flexible.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <numeric>
#include <stdexcept>
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/utils.hpp"

namespace UI {
    /// @ingroup UI
    /// @brief Table widget
    class TableRow;

    /// @ingroup UI
    /// @brief An element on a table, operates on a k=v fashion where the key is
    /// used for sorting the table.
    class TableElement : public UI::Widget {
    public:
        TableElement(int width, int height, UI::Widget* _parent);
        virtual ~TableElement() override {}

        void set_key(const std::string_view key);
        void set_key(float key);
        void set_key(float key, const std::string_view format);
        bool operator< (const UI::TableElement& right) const;
    private:
        enum class KeyType {
            NUMBER = 0,
            STRING = 1,
            NONE = 2
        };
        float key_number;
        std::string key_string;
        UI::TableElement::KeyType key_type = UI::TableElement::KeyType::NONE;
    };

    /// @ingroup UI
    /// @brief A row of a table, contains table elements and specifies the width
    /// of the entire row.
    class TableRow : public UI::Widget {
    public:
        TableRow(int width, int height, std::vector<int>& columns_width, UI::Widget* _parent);
        virtual ~TableRow() override {}
        UI::TableElement* get_element(size_t index);

        bool is_active = true;
    private:
        std::vector<UI::TableElement*> elements;
        std::vector<int>& columns_width;
    };

    /// @ingroup
    /// @brief A dynamic/smart table that can sort elements by ascending/descending order
    /// @tparam T Type of Id to use for row identification
    template <typename T>
    class Table : public UI::Widget {
    public:
        Table(int _x, int _y, unsigned _h, int _row_height, std::vector<int> _widths, std::vector<std::string> _header_labels, UI::Widget* _parent = nullptr)
            : UI::Widget(_parent, _x, _y, 0, _h, UI::WidgetType::TABLE),
            row_height{ _row_height }, columns_width{ _widths }
        {
            assert(_widths.size() == _header_labels.size());
            this->width = std::accumulate(_widths.begin(), _widths.end(), 35);
            auto& header = this->template make_widget<UI::TableRow>(this->width - 35, _row_height, this->columns_width);
            for(size_t i = 0; i < _header_labels.size(); i++) {
                auto& label = _header_labels[i];
                auto* column = header.get_element(i);
                column->set_text(label);
                column->set_on_click([this, i](Widget&) {
                    this->sorting_ascending = (this->sorting_row == (int)i) ? !this->sorting_ascending : true;
                    this->sorting_row = i;
                    this->sort(i, this->sorting_ascending);
                });
            }
            auto& wrapper = this->template make_widget<UI::Div>(0, _row_height, this->width, this->height - row_height);
            wrapper.is_scroll = true;
            this->column_wrapper = &wrapper.template make_widget<UI::Div>(0, 0, this->width - 25, 0);
            this->column_wrapper->flex = UI::Flex::COLUMN;
            this->column_wrapper->flex_justify = UI::FlexJustify::START;
            this->scrollbar = &wrapper.template make_widget<UI::Scrollbar>(this->width - 20, 0, wrapper.height - 40);
            this->on_update = [this](Widget&) {
                int total_height = this->row_height * this->rows.size();
                this->column_wrapper->height = total_height;
            };
        }
        virtual ~Table() override {}

        void reserve(size_t _size) {
            this->rows.reserve(_size);
        }

        UI::TableRow& get_row(T _row_id) {
            if(!this->rows.count(_row_id)) {
                auto& row = this->column_wrapper->template make_widget<UI::TableRow>(this->width - 25, this->row_height, this->columns_width);
                this->rows.insert({ _row_id, &row });
            }
            auto* row = this->rows[_row_id];
            row->is_active = true;
            return *row;
        }

        void remove_row(T _row_id) {
            if(this->rows.count(_row_id)) {
                this->rows[_row_id]->kill();
                this->rows.erase(_row_id);
            }
        }

        void make_rows_unactive() {
            for(auto it = rows.begin(); it != rows.end(); it++) {
                it->second->is_active = false;
            }
        }

        void clear_unactive_rows() {
            for(auto it = rows.begin(); it != rows.end(); ) {
                if(!it->second->is_active) {
                    it->second->kill();
                    it = rows.erase(it);
                } else {
                    ++it;
                }
            }
        }

        void sort(size_t _column_index, bool ascending) {
            this->column_wrapper->sort_children([_column_index, ascending](const auto& a, const auto& b) {
                UI::TableRow &row_a = static_cast<UI::TableRow&>(*a);
                auto &element_a = *(row_a.get_element(_column_index));
                UI::TableRow &row_b = static_cast<UI::TableRow&>(*b);
                auto &element_b = *(row_b.get_element(_column_index));
                return ascending ? (element_a < element_b) : (element_b < element_a);
            });
        }
    private:
        std::unordered_map<T, UI::TableRow*> rows;
        int sorting_row = -1;
        bool sorting_ascending = true;
        UI::Div* column_wrapper;
        UI::Scrollbar* scrollbar;
        int row_height;
        std::vector<int> columns_width;
    };
}
