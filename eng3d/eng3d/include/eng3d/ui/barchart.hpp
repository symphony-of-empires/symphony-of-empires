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
//      eng3d/ui/barchart.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/widget.hpp"

namespace UI {
    /**
     * @ingroup UI
     * @brief Bar chart that draws horizontal bar chart
     *
     */
    class BarChart: public Widget {
    public:
        BarChart(int x, int y, unsigned w, unsigned h, std::vector<ChartData> data = std::vector<ChartData>(), Widget* _parent = nullptr);
        BarChart(int x, int y, unsigned w, unsigned h, Widget* _parent = nullptr);
        virtual ~BarChart() override {};
        virtual void on_render(Context& ctx, Eng3D::Rect viewport);
        void set_data(std::vector<ChartData> data);
    private:
        std::vector<ChartData> data;
        float max = 1.f;
    };
};
