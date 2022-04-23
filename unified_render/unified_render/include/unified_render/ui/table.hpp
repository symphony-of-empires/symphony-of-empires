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
//      unified_render/ui/table.hpp
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

#include "unified_render/ui/widget.hpp"
namespace UI {
    /**
     * @ingroup UI
     * @brief Table widget
     *
     */

    class Scrollbar;
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


    class Table: public Widget {
    public:
        Table(int x, int y, unsigned w, unsigned h, int row_height, std::vector<int> widths, std::vector<std::string> header_labels, Widget* _parent = nullptr);
        virtual ~Table() override {};

        void reserve(size_t size);
        TableRow* get_row(size_t id);
        void remove_row(size_t id);
        void clear_unactive_rows();
        void sort(size_t column, bool ascending = true);

    private:
        std::unordered_map<size_t, TableRow*> rows;

        int sorting_row = -1;
        bool sorting_ascending = true;

        Widget* column_wrapper;
        Scrollbar* scrollbar;

        int row_height;
        std::vector<int> columns_width;
    };
};
