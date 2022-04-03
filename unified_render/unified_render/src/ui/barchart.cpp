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
//      unified_render/ui/barchart.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include "unified_render/ui/barchart.hpp"
#include "unified_render/ui/widget.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

using namespace UI;

BarChart::BarChart(int _x, int _y, unsigned w, unsigned h, std::vector<ChartData> chart_data, Widget* _parent)
    : data{ chart_data },
    max{ 0 },
    Widget(_parent, _x, _y, w, h, UI::WidgetType::BAR_CHART)
{
    
}

BarChart::BarChart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : data{ std::vector<ChartData>() },
    max{ 0 },
    Widget(_parent, _x, _y, w, h, UI::WidgetType::BAR_CHART)
{
    
}

void BarChart::set_data(std::vector<ChartData> new_data) {
    data = new_data;
    max = 0;
    for(const auto& slice : data) {
        max += slice.num;
    }
}

void BarChart::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    float counter = 0.f;
    float last_ratio = 0.f;
    for(auto& slice : data) {
        counter += slice.num;
        const float ratio = counter / max;

        UnifiedRender::Rect pos_rect(last_ratio * width, 0u, ratio * width, height);
        UnifiedRender::Rect tex_rect(0u, 0u, 1u, 1u);
        glColor3f(slice.color.r, slice.color.g, slice.color.b);
        draw_rect(0, pos_rect, tex_rect, viewport);

        last_ratio = ratio;
    }
}
