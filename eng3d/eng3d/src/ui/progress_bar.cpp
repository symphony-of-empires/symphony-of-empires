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

#include <GL/glew.h>
#include <GL/gl.h>

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
    : max{ _max },
    min{ _min },
    Widget(_parent, _x, _y, w, h, UI::WidgetType::PROGRESS_BAR)
{

}

Eng3D::Rect get_rect(Eng3D::Rect rect_pos, Eng3D::Rect viewport) {
    glm::vec2 pos_size = rect_pos.size();
    pos_size.x = pos_size.x > 0? pos_size.x : 1.f;
    pos_size.y = pos_size.y > 0? pos_size.y : 1.f;

    if(rect_pos.left < viewport.left) {
        //float x_ratio = (viewport.left - rect_pos.left) / pos_size.x;
        rect_pos.left = viewport.left;
    }
    if(rect_pos.right > viewport.right) {
        //float x_ratio = (rect_pos.right - viewport.right) / pos_size.x;
        rect_pos.right = viewport.right;
    }
    if(rect_pos.top < viewport.top) {
        //float y_ratio = (viewport.top - rect_pos.top) / pos_size.y;
        rect_pos.top = viewport.top;
    }
    if(rect_pos.bottom > viewport.bottom) {
        //float y_ratio = (rect_pos.bottom - viewport.bottom) / pos_size.y;
        rect_pos.bottom = viewport.bottom;
    }
    return rect_pos;
}

void ProgressBar::on_render(Context&, Eng3D::Rect viewport) {
    value = std::clamp(value, min, max);
    // TODO: Fix broken progress bar
    const Eng3D::Rect pos_rect(0, 0, (value / max) * width, height);
    g_ui_context->obj_shader->set_texture(0, "diffuse_map", *Eng3D::State::get_instance().tex_man->get_white());
    g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(0.f, 1.f, 0.f, 1.f));
    Eng3D::Square(pos_rect.left, pos_rect.top, pos_rect.right, pos_rect.bottom).draw();
    g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(0.f, 0.f, 1.f, 1.f));
    Eng3D::Square(pos_rect.right, pos_rect.top, width, pos_rect.bottom).draw();

    if(text_texture != nullptr) {
        if(!text_texture->gl_tex_num) {
            text_texture->upload();
        }
        g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(text_color.r, text_color.g, text_color.b, text_color.a));
        draw_rectangle(4, 2, text_texture->width, text_texture->height, viewport, text_texture);
    }
}