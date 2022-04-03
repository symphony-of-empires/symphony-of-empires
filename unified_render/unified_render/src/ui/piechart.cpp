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
//      unified_render/ui/piechart.cpp
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

#include "unified_render/ui/piechart.hpp"
#include "unified_render/ui/widget.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

using namespace UI;

// Calculate the positon of the triangle
// and then uses its barycentric coordinates to check if the point p is inside it
PieChart::PieChart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : data{ std::vector<ChartData>() },
    max{ 0 },
    Widget(_parent, _x, _y, w, h, UI::WidgetType::PIE_CHART)
{
    tooltip = new Tooltip(this, 512, 24);
    tooltip->is_render = false;
    on_hover = &PieChart::on_hover_default;
}

void PieChart::set_data(std::vector<ChartData> new_data) {
    data = new_data;
    max = 0;
    for(const auto& slice : data) {
        max += slice.num;
    }
}

void PieChart::draw_triangle(float start_ratio, float end_ratio, UnifiedRender::Color color) {
    float x_center = width / 2.f;
    float y_center = height / 2.f;
    float radius = std::min<float>(width, height) * 0.5;
    float x_offset, y_offset, scale;

    glColor3f(color.r, color.g, color.b);
    x_offset = cos((start_ratio - 0.25f) * 2 * M_PI);
    y_offset = sin((start_ratio - 0.25f) * 2 * M_PI);
    scale = std::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glTexCoord2f(0.5f + x_offset * 0.5f, 0.5f + y_offset * 0.5f);
    glVertex2f(x_center + x_offset * radius, y_center + y_offset * radius);

    x_offset = cos((end_ratio - 0.25f) * 2 * M_PI);
    y_offset = sin((end_ratio - 0.25f) * 2 * M_PI);
    scale = std::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glTexCoord2f(0.5f + x_offset * 0.5f, 0.5f + y_offset * 0.5f);
    glVertex2f(x_center + x_offset * radius, y_center + y_offset * radius);

    glTexCoord2f(0.5f, 0.5f);
    glVertex2f(x_center, y_center);
}

void PieChart::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    glBindTexture(GL_TEXTURE_2D, ctx.piechart_overlay->gl_tex_num);
    glBegin(GL_TRIANGLES);
    float counter = 0;
    float last_corner = -0.125f;
    float last_ratio = 0;

    for(auto& slice : data) {
        counter += slice.num;
        float ratio = counter / max;
        while(ratio > last_corner + 0.25f) {
            last_corner += 0.25f;
            draw_triangle(last_ratio, last_corner, slice.color);
            last_ratio = last_corner;
        }
        draw_triangle(last_ratio, ratio, slice.color);
        last_ratio = ratio;
    }
    glEnd();
}

static inline bool in_triangle(glm::vec2 p, glm::vec2 center, float radius, float start_ratio, float end_ratio) {
    float x_offset, y_offset, scale;
    x_offset = cos((start_ratio - 0.25f) * 2 * M_PI);
    y_offset = sin((start_ratio - 0.25f) * 2 * M_PI);
    scale = std::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glm::vec2 a(center.x + x_offset * radius, center.y + y_offset * radius);

    x_offset = cos((end_ratio - 0.25f) * 2 * M_PI);
    y_offset = sin((end_ratio - 0.25f) * 2 * M_PI);
    scale = std::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glm::vec2 b(center.x + x_offset * radius, center.y + y_offset * radius);

    glm::vec2 c(center.x, center.y);

    float A = 0.5f * (-b.y * c.x + a.y * (-b.x + c.x) + a.x * (b.y - c.y) + b.x * c.y);
    float sign = A < 0 ? -1 : 1;
    float s = (a.y * c.x - a.x * c.y + (c.y - a.y) * p.x + (a.x - c.x) * p.y) * sign;
    float t = (a.x * b.y - a.y * b.x + (a.y - b.y) * p.x + (b.x - a.x) * p.y) * sign;

    return s > 0 && t > 0 && (s + t) < 2 * A * sign;
}

void PieChart::on_hover_default(Widget& w, glm::ivec2 mouse_pos, glm::ivec2 widget_pos) {
    PieChart& piechart = static_cast<PieChart&>(w);
    mouse_pos -= widget_pos;
    float counter = 0;
    float last_corner = -0.125f;
    float last_ratio = 0;

    float x_center = piechart.width / 2.f;
    float y_center = piechart.height / 2.f;
    glm::ivec2 center = glm::ivec2(x_center, y_center);
    float radius = std::min<float>(piechart.width, piechart.height) * 0.5;

    glm::vec2 centered_pos = mouse_pos - center;
    if(glm::length(centered_pos) > radius) {
        piechart.tooltip->is_render = false;
        return;
    }

    for(auto& slice : piechart.data) {
        counter += slice.num;
        float ratio = counter / piechart.max;
        while(ratio > last_corner + 0.25f) {
            last_corner += 0.25f;
            bool is_inside = in_triangle(mouse_pos, center, radius, last_ratio, last_corner);
            if(is_inside) {
                piechart.tooltip->text(slice.info);
                piechart.tooltip->is_render = true;
                return;
            }
            last_ratio = last_corner;
        }

        bool is_inside = in_triangle(mouse_pos, center, radius, last_ratio, last_corner);
        if(is_inside) {
            piechart.tooltip->text(slice.info);
            piechart.tooltip->is_render = true;
            return;
        }
        last_ratio = ratio;
    }
}
