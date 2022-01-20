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
//      client/ui/progress_bar.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/ui/progress_bar.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include "client/ui/widget.hpp"
#include "client/ui/input.hpp"
#include "client/ui/ui.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

using namespace UI;

ProgressBar::ProgressBar(int _x, int _y, unsigned w, unsigned h, const float _min, const float _max, Widget* _parent)
    : max{ _max },
    min{ _min },
    Widget(_parent, _x, _y, w, h, UI::WidgetType::PROGRESS_BAR)
{

}

UnifiedRender::Rect get_rect(UnifiedRender::Rect rect_pos, UnifiedRender::Rect viewport) {
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

void ProgressBar::on_render(Context&, UnifiedRender::Rect viewport) {
    glColor3f(1.f, 1.f, 1.f);
    if(text_texture != nullptr) {
        if(!text_texture->gl_tex_num) {
            text_texture->to_opengl();
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    const float end_x = (value / max) * width;
    UnifiedRender::Rect pos_rect(0, 0, end_x, height);
    pos_rect = get_rect(pos_rect, viewport);

    // TODO: fix scaling of colors. They should scale depending on how of the bar that is hidden
    glBegin(GL_POLYGON);
    glColor3f(0.f, 0.f, 0.7f);
    glVertex2f(pos_rect.left, pos_rect.top);
    glVertex2f(pos_rect.right, pos_rect.top);
    glColor3f(0.f, 0.f, 0.4f);
    glVertex2f(pos_rect.right, pos_rect.bottom);
    glVertex2f(pos_rect.left, pos_rect.bottom);
    glColor3f(0.f, 0.f, 0.7f);
    glVertex2f(pos_rect.left, pos_rect.top);
    glEnd();

    if(text_texture != nullptr) {
        glColor3f(text_color.r, text_color.g, text_color.b);
        draw_rectangle(4, 2, text_texture->width, text_texture->height, viewport, text_texture->gl_tex_num);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glLineWidth(3.f);
    glColor3f(0.f, 0.f, 0.f);

    pos_rect = UnifiedRender::Rect(0, 0, width, height);
    pos_rect = get_rect(pos_rect, viewport);
    // Inner black border
    glBegin(GL_LINE_STRIP);
    glVertex2f(pos_rect.left, pos_rect.top);
    glVertex2f(pos_rect.right, pos_rect.top);
    glVertex2f(pos_rect.right, pos_rect.bottom);
    glVertex2f(pos_rect.left, pos_rect.bottom);
    glVertex2f(pos_rect.left, pos_rect.top);
    glEnd();
}