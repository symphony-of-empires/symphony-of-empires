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
//      eng3d/ui/progress_bar.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/input.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"
#include "eng3d/primitive.hpp"

using namespace UI;

ProgressBar::ProgressBar(int _x, int _y, unsigned w, unsigned h, const float _min, const float _max, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::PROGRESS_BAR),
    max{ _max },
    min{ _min }
{

}

Eng3D::Rect get_rect(Eng3D::Rect rect_pos, Eng3D::Rect viewport) {
    glm::vec2 pos_size = rect_pos.size();
    pos_size.x = pos_size.x > 0 ? pos_size.x : 1.f;
    pos_size.y = pos_size.y > 0 ? pos_size.y : 1.f;

    if(rect_pos.left < viewport.left)
        rect_pos.left = viewport.left;
    if(rect_pos.right > viewport.right)
        rect_pos.right = viewport.right;
    if(rect_pos.top < viewport.top)
        rect_pos.top = viewport.top;
    if(rect_pos.bottom > viewport.bottom)
        rect_pos.bottom = viewport.bottom;
    return rect_pos;
}

void ProgressBar::on_render(Context&, Eng3D::Rect viewport) {
    /// @todo Fix broken progress bar. Is it still broken, hmm ?
    g_ui_context->obj_shader->set_texture(0, "diffuse_map", *g_ui_context->button);
    g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(1.f, 1.f, 1.f, 1.f));
    Eng3D::Square(0, 0, width, height).draw();
    float ratio = value / max;
    Eng3D::Rect pos_rect(0, 0, width, height);
    switch(direction) {
    case UI::Direction::LEFT_TO_RIGHT:
        pos_rect.right = width * ratio;
        break;
    case UI::Direction::RIGHT_TO_LEFT:
        pos_rect.left = width * (1 - ratio);
        break;
    case UI::Direction::TOP_TO_BOTTOM:
        pos_rect.bottom = height * ratio;
        break;
    case UI::Direction::BOTTOM_TO_TOP:
        pos_rect.top = height * (1 - ratio);
        break;
    }

    g_ui_context->obj_shader->set_texture(0, "diffuse_map", *g_ui_context->window_top);
    g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(1.f, 1.f, 1.f, 1.f));
    Eng3D::Square(pos_rect.left, pos_rect.top, pos_rect.right, pos_rect.bottom).draw();

    if(text_texture.get() != nullptr) {
        g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(text_color.r, text_color.g, text_color.b, text_color.a));
        draw_rectangle(4, 2, text_texture->width, text_texture->height, viewport, text_texture.get());
    }
}