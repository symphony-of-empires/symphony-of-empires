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
//      eng3d/ui/piechart.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <glm/vec2.hpp>

#include "eng3d/ui/piechart.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

using namespace UI;

/// @brief Calculate the positon of the triangle
/// and then uses its barycentric coordinates to check if the point p is inside it
PieChart::PieChart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::PIE_CHART),
    data{ std::vector<ChartData>() },
    max{ 0 }
{
    on_hover = &PieChart::on_hover_default;
}

void PieChart::set_data(std::vector<ChartData> new_data) {
    data = new_data;
    max = 0;
    for(const auto& slice : data)
        max += slice.num;
}

inline void PieChart::draw_triangle(float start_ratio, float end_ratio, Eng3D::Color color) {
    const float x_center = width / 2.f;
    const float y_center = height / 2.f;
    const float radius = glm::min<float>(width, height) * 0.5;
    float x_offset, y_offset, scale;

    x_offset = glm::cos((start_ratio - 0.25f) * 2 * glm::pi<float>());
    y_offset = glm::sin((start_ratio - 0.25f) * 2 * glm::pi<float>());
    scale = glm::min<float>(1.f / glm::abs(x_offset), 1.f / glm::abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;

    auto mesh = Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLES);

    Eng3D::State::get_instance().ui_ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(color.r, color.g, color.b, 1.f));
    mesh.buffer.push_back(Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(x_center + x_offset * radius, y_center + y_offset * radius), glm::vec2(0.5f + x_offset * 0.5f, 0.5f + y_offset * 0.5f)));

    x_offset = glm::cos((end_ratio - 0.25f) * 2 * glm::pi<float>());
    y_offset = glm::sin((end_ratio - 0.25f) * 2 * glm::pi<float>());
    scale = glm::min<float>(1.f / glm::abs(x_offset), 1.f / glm::abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    mesh.buffer.push_back(Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(x_center + x_offset * radius, y_center + y_offset * radius), glm::vec2(0.5f + x_offset * 0.5f, 0.5f + y_offset * 0.5f)));
    mesh.buffer.push_back(Eng3D::MeshData<glm::vec2, glm::vec2>(glm::vec2(x_center, y_center), glm::vec2(0.5f, 0.5f)));
    mesh.upload();
    mesh.draw();
}

void PieChart::on_render(UI::Context& ctx, Eng3D::Rect) {
    ctx.piechart_shader->set_texture(0, "diffuse_map", *ctx.piechart_overlay);
    float counter = 0;
    float last_corner = -0.125f;
    float last_ratio = 0;
    for(auto& slice : data) {
        if(slice.num == 0.f) continue;
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
}

static inline bool in_triangle(glm::vec2 p, glm::vec2 center, float radius, float start_ratio, float end_ratio) {
    if(start_ratio == 0.f || end_ratio == 0.f) return false;

    auto x_offset = glm::cos((start_ratio - 0.25f) * 2.f * glm::pi<float>());
    auto y_offset = glm::sin((start_ratio - 0.25f) * 2.f * glm::pi<float>());

    if(abs(x_offset) == 0.f || abs(y_offset) == 0.f) return false;
    auto scale = glm::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glm::vec2 a{ center.x + x_offset * radius, center.y + y_offset * radius };

    x_offset = glm::cos((end_ratio - 0.25f) * 2.f * glm::pi<float>());
    y_offset = glm::sin((end_ratio - 0.25f) * 2.f * glm::pi<float>());

    if(abs(x_offset) == 0.f || abs(y_offset) == 0.f) return false;
    scale = glm::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glm::vec2 b{ center.x + x_offset * radius, center.y + y_offset * radius };
    glm::vec2 c{ center.x, center.y };

    float A = 0.5f * (-b.y * c.x + a.y * (-b.x + c.x) + a.x * (b.y - c.y) + b.x * c.y);
    float sign = A < 0 ? -1 : 1;
    float s = (a.y * c.x - a.x * c.y + (c.y - a.y) * p.x + (a.x - c.x) * p.y) * sign;
    float t = (a.x * b.y - a.y * b.x + (a.y - b.y) * p.x + (b.x - a.x) * p.y) * sign;

    return s > 0 && t > 0 && (s + t) < 2 * A * sign;
}

void PieChart::on_hover_default(Widget& w, glm::ivec2 mouse_pos, glm::ivec2 widget_pos) {
    auto& piechart = static_cast<PieChart&>(w);
    mouse_pos -= widget_pos;

    float x_center = piechart.width / 2.f;
    float y_center = piechart.height / 2.f;
    glm::ivec2 center{ x_center, y_center };
    float radius = glm::min<float>(piechart.width, piechart.height) * 0.5;

    glm::vec2 centered_pos = mouse_pos - center;
    if(glm::length(centered_pos) > radius) return;

    float counter = 0.f, last_corner = -0.125f, last_ratio = 0.f;
    for(auto& slice : piechart.data) {
        if(slice.num == 0.f) continue;
        counter += slice.num;
        float ratio = counter / piechart.max;
        while(ratio > last_corner + 0.25f) {
            last_corner += 0.25f;
            bool is_inside = in_triangle(mouse_pos, center, radius, last_ratio, last_corner);
            if(is_inside) {
                piechart.set_tooltip(slice.info);
                return;
            }
            last_ratio = last_corner;
        }

        bool is_inside = in_triangle(mouse_pos, center, radius, last_ratio, last_corner);
        if(is_inside) {
            piechart.set_tooltip(slice.info);
            break;
        }
        last_ratio = ratio;
    }
}
