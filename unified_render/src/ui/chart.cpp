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
//      unified_render/ui/chart.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>

#include "unified_render/ui/widget.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/chart.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

using namespace UI;

Chart::Chart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::LABEL)
{

}

void Chart::on_render(Context&, UnifiedRender::Rect viewport) {
    glColor3f(1.f, 1.f, 1.f);
    if(text_texture != nullptr) {
        if(!text_texture->gl_tex_num) {
            text_texture->to_opengl();
        }
    }

    if(current_texture != nullptr && current_texture->gl_tex_num) {
        draw_rectangle(
            0, 0,
            width, height,
            viewport,
            current_texture->gl_tex_num);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    if(1) {
        glLineWidth(2.f);

        // Outer black border
        glBegin(GL_LINE_STRIP);

        glColor3f(0.f, 0.f, 0.f);
        glVertex2f(0, height);
        glVertex2f(0, 0);
        glVertex2f(width, 0);

        glColor3f(1.f, 1.f, 1.f);
        glVertex2f(width, 0);
        glVertex2f(width, height);
        glVertex2f(0, height);
        glEnd();
    }

    if(data.size() > 1) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glLineWidth(2.f);

        // Obtain the highest and lowest values
        double max = *std::max_element(data.begin(), data.end());
        double min = 0.f;

        if(max < min) {
            min = *std::min_element(data.begin(), data.end());
        }

        // Works on zero-only graphs
        if(max == min) {
            max += 1.f;
        }

        // Do not allow graph lines to be on the "border" black thing
        size_t real_height = height - 3;

        size_t time = 0;
        glBegin(GL_LINE_STRIP);
        glColor3f(0.f, 0.f, 0.f);
        time = 0;
        for(const auto& node : data) {
            glVertex2f(
                time * (width / (data.size() - 1)),
                (real_height - (((node - min) / (max - min)) * real_height)) + 2.f);
            time++;
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        glColor3f(1.f, 0.f, 0.f);
        time = 0;
        for(const auto& node : data) {
            glVertex2f(
                time * (width / (data.size() - 1)),
                real_height - (((node - min) / (max - min)) * real_height));
            time++;
        }
        glEnd();
    }

    if(text_texture != nullptr) {
        glColor3f(text_color.r, text_color.g, text_color.b);
        draw_rectangle(4, 2, text_texture->width, text_texture->height, viewport, text_texture->gl_tex_num);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}