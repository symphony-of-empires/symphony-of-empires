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
//      eng3d/ui/barchart.cpp
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

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#endif

#include "eng3d/ui/barchart.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

using namespace UI;

BarChart::BarChart(int _x, int _y, unsigned w, unsigned h, std::vector<ChartData> chart_data, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::BAR_CHART),
    data{ chart_data },
    max{ 0 }
{

}

BarChart::BarChart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::BAR_CHART),
    data{ std::vector<ChartData>() },
    max{ 0 }
{

}

void BarChart::set_data(std::vector<ChartData> new_data) {
    data = new_data;
    max = 0;
    for(const auto& slice : data)
        max += slice.num;
}

void BarChart::on_render(Context&, Eng3D::Rect viewport) {
    float counter = 0.f;
    float last_ratio = 0.f;
    for(auto& slice : data) {
        counter += slice.num;
        const float ratio = counter / max;

        Eng3D::Rect pos_rect(last_ratio * width, 0u, ratio * width, height);
        Eng3D::Rect tex_rect(0u, 0u, 1u, 1u);
        g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(slice.color.r, slice.color.g, slice.color.b, 1.f));
        draw_rect(0, pos_rect, tex_rect, viewport);

        last_ratio = ratio;
    }
}
